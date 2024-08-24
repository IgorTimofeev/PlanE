#include "transceiver.h"
#include "aircraft.h"
#include "packet.h"

#define RF_FREQUENCY                                915000000 // Hz  center frequency
#define TX_OUTPUT_POWER                             22        // dBm tx output power
#define LORA_BANDWIDTH                              4         // bandwidth
// 2: 31.25Khz
// 3: 62.5Khz
// 4: 125Khz
// 5: 250KHZ
// 6: 500Khz
#define LORA_SPREADING_FACTOR                       7         // spreading factor [SF5..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
//  2: 4/6,
//  3: 4/7,
//  4: 4/8]

#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_PAYLOADLENGTH                          0         // 0: Variable length packet (explicit header)
// 1..255  Fixed length packet (implicit header)

void Transceiver::begin() {
	_AESLib.gen_iv(_AESIV);
	_AESLib.set_paddingmode(paddingMode::CMS);

	Serial.println("[SX1262] Initializing");

	auto state = _radio.begin(RF_FREQUENCY, TX_OUTPUT_POWER);

	if (state == ERR_NONE) {

	}
	else {
		Serial.print("[SX1262] Failure, code: ");
		Serial.println(state);

		while (true)
			delay(100);
	}

	_radio.LoRaConfig(
		LORA_SPREADING_FACTOR,
		LORA_BANDWIDTH,
		LORA_CODINGRATE,
		LORA_PREAMBLE_LENGTH,
		LORA_PAYLOADLENGTH,
		true,               //crcOn
		false);             //invertIrq
}

void Transceiver::tick(Aircraft &aircraft) {
	auto& ahrs = aircraft.getAHRS();

	const uint32_t packetHeader = 0x506C416E;

	Packet<AHRSPacket> packet(
		packetHeader,
		PacketType::AHRS,
		AHRSPacket {
			.pitch = ahrs.getPitch(),
			.roll = ahrs.getRoll(),
			.yaw = ahrs.getYaw(),

			.temperature = ahrs.getTemperature(),
			.pressure = ahrs.getPressure(),

			.qnh = ahrs.getQnh(),
			.altitude = ahrs.getAltitude()
		}
	);

	Serial.println("[AES] Encrypting packet");

	uint16_t packetLength = sizeof(packet);
	uint16_t prefixLength = sizeof(packet.header);
	uint16_t typeLength = sizeof(packet.type);

//	Serial.println("Bytes: ");
//
//	auto govo = (uint8_t*) &packet + prefixLength;
//
//	for (uint16_t i = 0; i < packetLength - prefixLength; i++) {
//		Serial.print((uint8_t)*(govo + i));
//		Serial.print(" ");
//	}
//
//	Serial.println();

	// Copying header
	mempcpy(&_AESBuffer, &packet, prefixLength);

	// Encrypting
	uint8_t iv[sizeof(_AESIV)];
	memcpy(iv, _AESIV, sizeof(_AESIV));

	auto encryptedBodyLength = _AESLib.encrypt(
		(uint8_t*) &packet + prefixLength,
		packetLength - prefixLength,
		(uint8_t*) &_AESBuffer + prefixLength,
		_AESKey,
		sizeof(_AESKey),
		iv
	);

	// Twemp, for sdimulation
	uint8_t receiveBuffer[255];
	uint16_t receivedPacketLength = prefixLength + encryptedBodyLength;
	memcpy(&receiveBuffer, &_AESBuffer, receivedPacketLength);

	if (((uint32_t*) &receiveBuffer)[0] == packetHeader) {
		Serial.println("Norm header");

		Serial.println("[AES] Decrypting packet");

		memcpy(iv, _AESIV, sizeof(_AESIV));

		_AESLib.decrypt(
			(uint8_t*) &receiveBuffer + prefixLength,
			receivedPacketLength - prefixLength,
			_AESBuffer,
			_AESKey,
			sizeof(_AESKey),
			iv
		);

		auto decryptedPacketType = (PacketType) _AESBuffer[0];
		Serial.print("Decrypted packet type: ");
		Serial.println(decryptedPacketType);

		//	Serial.println("Bytes: ");
		//
		//	for (uint16_t i = 0; i < receivedPacketLength; i++) {
		//		Serial.print(_AESBuffer[i]);
		//		Serial.print(" ");
		//	}
		//
		//	Serial.println();

		switch (decryptedPacketType) {
			case PacketType::AHRS:
				auto eblo = &_AESBuffer;

				auto decryptedPacket = (AHRSPacket*) ((uint8_t*) &_AESBuffer + typeLength);

				decryptedPacket->print();

				break;
		}
	}
	else {
		Serial.println("Bad header");
	}



//	Serial.println("[SX1262] Sending packet");
//
//	if (_radio.Send((uint8_t*) &packet, sizeof(GovnoPacket), SX126x_TXMODE_SYNC)) {
//
//	}
//	else {
//		// some other error occurred
//		Serial.print("[SX1262] failed, code ");
//	}

}