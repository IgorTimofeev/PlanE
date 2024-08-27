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

}

void Transceiver::tick(Aircraft &aircraft) {
	auto& ahrs = aircraft.getAHRS();

	send(
		PacketType::AircraftAHRS,
		AircraftAHRSPacket {
			.throttle = ahrs.getRemoteData().getThrottle(),
			.ailerons = ahrs.getRemoteData().getAilerons(),
			.rudder = ahrs.getRemoteData().getRudder(),
			.flaps = ahrs.getRemoteData().getFlaps(),

			.pitch = ahrs.getLocalData().getPitch(),
			.roll = ahrs.getLocalData().getRoll(),
			.yaw = ahrs.getLocalData().getYaw(),

			.temperature = ahrs.getLocalData().getTemperature(),
			.pressure = ahrs.getLocalData().getPressure(),

			.altimeterMode = ahrs.getRemoteData().getAltimeterMode(),
			.altimeterPressure = ahrs.getRemoteData().getAltimeterPressure(),

			.altitude = ahrs.getLocalData().getAltitude(),
			.speed = ahrs.getLocalData().getSpeed(),

			.strobeLights = ahrs.getRemoteData().getStrobeLights(),

//			.throttle = 1,
//			.ailerons = 2,
//			.rudder = 3,
//			.flaps = 4,
//
//			.pitch =5,
//			.roll =6,
//			.yaw =7,
//
//			.temperature =8,
//			.pressure =9,
//
//			.altimeterMode = AltimeterMode::QNH,
//			.altimeterPressure = 1,
//
//			.altitude =11,
//			.speed =12,
//
//			.strobeLights = true,
		}
	);

//	receive(aircraft);
}

template<typename T>
void Transceiver::send(PacketType packetType, const T& packet) {
	auto wrapper = PacketTypeWrapper<T>(packetType, packet);

	uint8_t wrapperLength = sizeof(wrapper);
	uint8_t encryptedWrapperLength = wrapperLength + 16 - (wrapperLength % 16);

	auto header = settings::transceiver::packetHeader;
	uint8_t headerLength = sizeof(header);
	uint8_t totalLength = encryptedWrapperLength + headerLength;

	// Copying header
	mempcpy(&_AESBuffer, &header, headerLength);

	// Encrypting body
	auto aes = esp_aes_context();
	esp_aes_init(&aes);
	esp_aes_setkey(&aes, _AESKey, sizeof(_AESKey) * 8);

	memcpy(_AESIVCopy, _AESIV, sizeof(_AESIV));

	esp_aes_crypt_cbc(
		&aes,
		ESP_AES_ENCRYPT,
		encryptedWrapperLength,
		_AESIVCopy,
		(uint8_t*) &wrapper,
		(uint8_t*) &_AESBuffer + headerLength
	);

	esp_aes_free(&aes);

//	Serial.printf("[SX1262] Sending packet with type %d of %d bytes\n", packetType, totalLength);

	if (_sx1262.Send(_AESBuffer, totalLength, SX126x_TXMODE_SYNC)) {

	}
	else {
		Serial.print("[SX1262] Sending failed");
	}
}

void Transceiver::receive(Aircraft &aircraft) {
	uint8_t receivedLength = _sx1262.Receive(_sx1262Buffer, sizeof(_sx1262Buffer));

	if (receivedLength == 0)
		return;

//	Serial.printf("[Transceiver] Got packet of %d bytes\n", receivedLength);

	// Checking header
	auto header = ((uint32_t*) &_sx1262Buffer)[0];
	uint8_t headerLength = sizeof(settings::transceiver::packetHeader);

	if (header != settings::transceiver::packetHeader) {
		Serial.printf("[Transceiver] Unsupported header: %02X\n", header);
		return;
	}

	uint8_t encryptedLength = receivedLength - headerLength;

	// Decrypting
	auto aes = esp_aes_context();
	esp_aes_init(&aes);
	esp_aes_setkey(&aes, _AESKey, sizeof(_AESKey) * 8);

	memcpy(_AESIVCopy, _AESIV, sizeof(_AESIV));

	auto decryptState = esp_aes_crypt_cbc(
		&aes,
		ESP_AES_DECRYPT,
		encryptedLength,
		_AESIVCopy,
		(uint8_t*) &_sx1262Buffer + headerLength,
		_AESBuffer
	);

	esp_aes_free(&aes);

	if (decryptState == 0) {
		parsePacket(aircraft, _AESBuffer);
	}
	else {
		Serial.printf("Decrypting failed: %d\n", encryptedLength);
	}
}

void Transceiver::parsePacket(Aircraft &aircraft, uint8_t* bufferPtr) {
	auto& ahrs = aircraft.getAHRS();

	auto packetType = (PacketType) *bufferPtr;
	bufferPtr += sizeof(PacketType);

	switch (packetType) {
		case PacketType::ControllerCommand:
			{
				auto controllerCommandPacket = (ControllerCommandPacket*) bufferPtr;

				controllerCommandPacket->print();

				ahrs.getRemoteData().setThrottle(controllerCommandPacket->throttle);
				ahrs.getRemoteData().setAilerons(controllerCommandPacket->ailerons);
				ahrs.getRemoteData().setRudder(controllerCommandPacket->rudder);
				ahrs.getRemoteData().setFlaps(controllerCommandPacket->flaps);

				ahrs.getRemoteData().setAltimeterPressure(controllerCommandPacket->altimeterPressure);
				ahrs.getRemoteData().setAltimeterMode(controllerCommandPacket->altimeterMode);

				ahrs.getRemoteData().setStrobeLights(controllerCommandPacket->strobeLights);
			}

			break;

		default:
			Serial.print("[Transceiver] Unsupported packet type: ");
			Serial.println(packetType);

			break;
	}
}
