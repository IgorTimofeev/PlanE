#include "transceiver.h"
#include "aircraft.h"
#include "packet.h"

void Transceiver::begin() {
	Serial.println("[SX1262] Initializing");

	auto state = _radio.begin(
		// Hz  center frequency
		915000000,
		// dBm tx output power
		22
	);

	if (state == ERR_NONE) {

	}
	else {
		Serial.print("[SX1262] Failure, code: ");
		Serial.println(state);

		while (true)
			delay(100);
	}

	_radio.LoRaConfig(
		// spreading factor [SF5..SF12]
		7,
		// bandwidth
		// 2: 31.25Khz
		// 3: 62.5Khz
		// 4: 125Khz
		// 5: 250KHZ
		// 6: 500Khz
		4,
		// [1: 4/5,
		//  2: 4/6,
		//  3: 4/7,
		//  4: 4/8]
		1,
		// Same for Tx and Rx
		8,
		// 0: Variable length packet (explicit header)
		// 1..255  Fixed length packet (implicit header)
		0,
		//crcOn
		true,
		//invertIrq
		false
	);

	Serial.println("[AES] Initializing");

	esp_aes_init(&_AESContext);
	esp_aes_setkey(&_AESContext, _AESKey, sizeof(_AESKey) * 8);
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

	Serial.println("[Transceiver] Encrypting packet");

	uint16_t packetLength = sizeof(packet);
	uint16_t headerLength = sizeof(packet.header);
	uint16_t typeLength = sizeof(packet.type);

	auto bodyLength = packetLength - headerLength;
	auto encryptedLength = bodyLength + 16 - (bodyLength % 16);

	// Copying header
	mempcpy(&_AESBuffer, &packet, headerLength);

	// Encrypting body
	memcpy(_AESIVCopy, _AESIV, sizeof(_AESIV));
	esp_aes_crypt_cbc(
		&_AESContext,
		ESP_AES_ENCRYPT,
		encryptedLength,
		_AESIVCopy,
		(uint8_t*) &packet + headerLength,
		(uint8_t*) &_AESBuffer + headerLength
	);

	// Twemp, for sdimulation
	uint8_t receiveBuffer[255];
	uint16_t receivedPacketLength = headerLength + encryptedLength;
	memcpy(&receiveBuffer, &_AESBuffer, receivedPacketLength);

	// Checking if header is correct
	if (((uint32_t*) &receiveBuffer)[0] == packetHeader) {
		Serial.println("[Transceiver] Valid header, decrypting packet");

		encryptedLength = receivedPacketLength - headerLength;
		encryptedLength = encryptedLength + 16 - (encryptedLength % 16);

		memcpy(_AESIVCopy, _AESIV, sizeof(_AESIV));
		esp_aes_crypt_cbc(
			&_AESContext,
			ESP_AES_DECRYPT,
			encryptedLength,
			_AESIVCopy,
			(uint8_t*) &receiveBuffer + headerLength,
			_AESBuffer
		);

		auto decryptedPacketType = (PacketType) _AESBuffer[0];
		Serial.print("[Transceiver] Decrypted packet type: ");
		Serial.println(decryptedPacketType);

		switch (decryptedPacketType) {
			case PacketType::AHRS:
				auto decryptedPacket = (AHRSPacket*) ((uint8_t*) &_AESBuffer + typeLength);

				decryptedPacket->print();

				break;
		}
	}
	else {
		Serial.println("[Transceiver] Bad header");
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