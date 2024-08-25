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
}

void Transceiver::tick(Aircraft &aircraft) {
	auto& ahrs = aircraft.getAHRS();

	const uint32_t packetHeader = 0x506C416E;

	Packet<AHRSPacket> packet(
		packetHeader,
		PacketType::AHRS,
//		AHRSPacket {
//			.pitch = ahrs.getPitch(),
//			.roll = ahrs.getRoll(),
//			.yaw = ahrs.getYaw(),
//
//			.temperature = ahrs.getTemperature(),
//			.pressure = ahrs.getPressure(),
//
//			.qnh = ahrs.getQnh(),
//			.altitude = ahrs.getAltitude()
//		}
		AHRSPacket {
			.pitch = 10,
			.roll = 10,
			.yaw = 10,

			.temperature = 10,
			.pressure = 10,

			.qnh = 10,
			.altitude = 10
		}
	);

	Serial.println("[AES] Initializing");

	uint8_t _AESIV[16] = { 0x17, 0x5D, 0x25, 0x2A, 0xFD, 0x72, 0x1E, 0x01, 0x02, 0x60, 0x88, 0x92, 0x9A, 0x9B, 0x2A, 0xA9 };


	uint8_t _AESBuffer[255];

	esp_aes_context _AESContext = esp_aes_context();
	esp_aes_init(&_AESContext);

	Serial.println("[AES] Encrypting packet");

	uint16_t packetLength = sizeof(packet);
	uint16_t headerLength = sizeof(packet.header);
	uint16_t typeLength = sizeof(packet.type);

	// Copying header
	mempcpy(&_AESBuffer, &packet, headerLength);

	// Encrypting

	auto nonEncryptedBodyLength = packetLength - headerLength;

	Serial.print("[AES] nonEncryptedBodyLength: ");
	Serial.println(nonEncryptedBodyLength);

	Serial.println("[AES] Bytes: ");

	auto nonEncryptedPtr = (uint8_t*) &packet + headerLength;

	for (uint8_t* i = nonEncryptedPtr; i < nonEncryptedPtr + nonEncryptedBodyLength; i++) {
		Serial.print(*i);
		Serial.print(" ");
	}

	Serial.println();

	uint8_t _AESKey[16] = { 0x02, 0xEB, 0x46, 0x45, 0x96, 0xB0, 0xD6, 0xB9, 0x7C, 0x34, 0xBE, 0x77, 0x75, 0xF2, 0xBE, 0x1B };
	esp_aes_setkey(&_AESContext, _AESKey, 128);

	esp_aes_crypt_cbc(
		&_AESContext,
		ESP_AES_ENCRYPT,
		nonEncryptedBodyLength,
		_AESIV,
		(uint8_t*) &packet + headerLength,
		(uint8_t*) &_AESBuffer + headerLength
	);

	auto encryptedBodyLength = nonEncryptedBodyLength + 16 - (nonEncryptedBodyLength % 16);

	Serial.print("[AES] encryptedBodyLength: ");
	Serial.println(encryptedBodyLength);

	// Twemp, for sdimulation

	uint8_t receiveBuffer[255];
	uint16_t receivedPacketLength = headerLength + encryptedBodyLength;
	memcpy(&receiveBuffer, &_AESBuffer, receivedPacketLength);

	Serial.print("[AES] receivedPacketLength: ");
	Serial.println(receivedPacketLength);

	auto packetPtr = (uint8_t*) &receiveBuffer;
	Serial.println("[AES] Bytes: ");

	for (uint8_t* i = packetPtr; i < packetPtr + receivedPacketLength; i++) {
		Serial.print(*i);
		Serial.print(" ");
	}

	Serial.println();

	if (((uint32_t*) &receiveBuffer)[0] == packetHeader) {
		Serial.println("[AES] Norm header");

		Serial.println("[AES] Decrypting packet");

		uint8_t pizda_AESIV[16] = { 0x17, 0x5D, 0x25, 0x2A, 0xFD, 0x72, 0x1E, 0x01, 0x02, 0x60, 0x88, 0x92, 0x9A, 0x9B, 0x2A, 0xA9 };
		uint8_t pizda_AESKey[16] = { 0x02, 0xEB, 0x46, 0x45, 0x96, 0xB0, 0xD6, 0xB9, 0x7C, 0x34, 0xBE, 0x77, 0x75, 0xF2, 0xBE, 0x1B };
		uint8_t pizda_AESBuffer[255];

		esp_aes_context pizda_AESContext = esp_aes_context();
		esp_aes_init(&pizda_AESContext);

		esp_aes_setkey(&pizda_AESContext, pizda_AESKey, 128);

		esp_aes_crypt_cbc(
			&pizda_AESContext,
			ESP_AES_DECRYPT,
			receivedPacketLength - headerLength,
			pizda_AESIV,
			(uint8_t*) &receiveBuffer + headerLength,
			pizda_AESBuffer
		);

		auto decryptedPacketType = (PacketType) pizda_AESBuffer[0];
		Serial.print("[AES] Decrypted packet type: ");
		Serial.println(decryptedPacketType);

		Serial.println("[AES] Bytes: ");

		for (uint16_t i = 0; i < receivedPacketLength; i++) {
			Serial.print(_AESBuffer[i]);
			Serial.print(" ");
		}

		Serial.println();

		switch (decryptedPacketType) {
			case PacketType::AHRS:
				auto decryptedPacket = (AHRSPacket*) ((uint8_t*) &pizda_AESBuffer + typeLength);

				decryptedPacket->print();

				break;
		}

		esp_aes_free(&pizda_AESContext);
	}
	else {
		Serial.println("Bad header");
	}

	esp_aes_free(&_AESContext);


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