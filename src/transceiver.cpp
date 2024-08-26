#include "transceiver.h"
#include "aircraft.h"
#include "packet.h"

void Transceiver::begin() {
	Serial.println("[SX1262] Initializing");

	auto state = _sx1262.begin(
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

	_sx1262.LoRaConfig(
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

	send(
		PacketType::AircraftAHRS,
		AircraftAHRSPacket {
			.pitch = ahrs.getPitch(),
			.roll = ahrs.getRoll(),
			.yaw = ahrs.getYaw(),

			.temperature = ahrs.getTemperature(),
			.pressure = ahrs.getPressure(),
		}
	);

	receive(aircraft);
}

template<typename T>
void Transceiver::send(PacketType packetType, const T& packet) {
	Serial.println("[Transceiver] Encrypting packet");

	auto wrapper = PacketTypeWrapper<T>(packetType, packet);

	uint8_t wrapperLength = sizeof(wrapper);
	uint8_t encryptedWrapperLength = wrapperLength + 16 - (wrapperLength % 16);

	auto header = Settings::Transceiver::packetHeader;
	uint8_t headerLength = sizeof(header);
	uint8_t totalLength = wrapperLength + headerLength;

	// Copying header
	mempcpy(&_AESBuffer, &header, headerLength);

	// Encrypting body
	memcpy(_AESIVCopy, _AESIV, sizeof(_AESIV));
	esp_aes_crypt_cbc(
		&_AESContext,
		ESP_AES_ENCRYPT,
		encryptedWrapperLength,
		_AESIVCopy,
		(uint8_t*) &wrapper,
		(uint8_t*) &_AESBuffer + headerLength
	);

	Serial.println("[SX1262] Sending packet");

	if (_sx1262.Send((uint8_t*) &_AESBuffer, totalLength, SX126x_TXMODE_SYNC)) {

	}
	else {
		// some other error occurred
		Serial.print("[SX1262] Sending failed");
	}
}

void Transceiver::receive(Aircraft &aircraft) {
	uint8_t receivedLength = _sx1262.Receive(_sx1262Buffer, sizeof(_sx1262Buffer));

	if (receivedLength <= 0)
		return;

	Serial.println("[Transceiver] Got packet");

	auto sx1262BufferPtr = (uint8_t*) &_sx1262Buffer;
	auto header = ((uint32_t*) sx1262BufferPtr)[0];

	// Checking header
	if (header != Settings::Transceiver::packetHeader) {
		Serial.printf("[Transceiver] Unsupported header: %02X", header);
		return;
	}

	uint8_t headerLength = sizeof(Settings::Transceiver::packetHeader);
	sx1262BufferPtr += headerLength;

	Serial.println("[Transceiver] Decrypting packet");

	uint8_t encryptedLength = receivedLength - headerLength;
	encryptedLength = encryptedLength + 16 - (encryptedLength % 16);

	// Decrypting
	memcpy(_AESIVCopy, _AESIV, sizeof(_AESIV));
	esp_aes_crypt_cbc(
		&_AESContext,
		ESP_AES_DECRYPT,
		encryptedLength,
		_AESIVCopy,
		sx1262BufferPtr,
		_AESBuffer
	);

	parsePacket((uint8_t *) &_AESBuffer);
}

void Transceiver::parsePacket(uint8_t* bufferPtr) {
	auto packetType = (PacketType) *bufferPtr;
	bufferPtr += sizeof(PacketType);

	switch (packetType) {
		case PacketType::ControllerCommand:
			{
				auto controllerCommandPacket = (ControllerCommandPacket*) bufferPtr;

				controllerCommandPacket->print();
			}

			break;

		default:
			Serial.print("[Transceiver] Unsupported packet type: ");
			Serial.println(packetType);

			break;
	}
}
