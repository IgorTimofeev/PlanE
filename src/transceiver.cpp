#include "transceiver.h"
#include "aircraft.h"

void Transceiver::begin() {
	Serial.println("[SX1262] Initializing");

	auto state = _radio.begin(
		866.0,
		125.0,
		9,
		7,
		RADIOLIB_SX126X_SYNC_WORD_PRIVATE,
		10,
		8,
		0
	);

	if (state == RADIOLIB_ERR_NONE) {

	}
	else {
		Serial.print("[SX1262] Failure, code: ");
		Serial.println(state);

		while (true)
			delay(100);
	}
}

void Transceiver::tick(Aircraft &aircraft) {
	auto& ahrs = aircraft.getAHRS();

	GovnoPacket packet = GovnoPacket {
		.pitch = ahrs.getPitch(),
		.roll = ahrs.getRoll(),
		.yaw = ahrs.getYaw(),

		.temperature = ahrs.getTemperature(),
		.pressure = ahrs.getPressure(),

		.qnh = ahrs.getQnh(),
		.altitude = ahrs.getAltitude()
	};

	Serial.println("[SX1262] Sending packet");
	_radio.transmit((const char*) &packet);
}