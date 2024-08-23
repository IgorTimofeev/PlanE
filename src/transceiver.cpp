#include "transceiver.h"
#include "aircraft.h"

void Transceiver::begin() {
	Serial.println("[SX1262] Initializing");

	auto state = _radio.begin(
		434.0,
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
		.signature = 0x506C416E,

		.pitch = ahrs.getPitch(),
		.roll = ahrs.getRoll(),
		.yaw = ahrs.getYaw(),

		.temperature = ahrs.getTemperature(),
		.pressure = ahrs.getPressure(),

		.qnh = ahrs.getQnh(),
		.altitude = ahrs.getAltitude()
	};

	Serial.println("[SX1262] Sending packet");
	auto state = _radio.transmit((const char*) &packet);

	if (state == RADIOLIB_ERR_NONE) {
		// the packet was successfully transmitted
		Serial.println("[SX1262] success!");

		// print measured data rate
		Serial.print("[SX1262] Data rate: ");
		Serial.print(_radio.getDataRate());
		Serial.println(" bps");

	}
	else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
		// the supplied packet was longer than 256 bytes
		Serial.println(F("[SX1262] too long!"));
	}
	else if (state == RADIOLIB_ERR_TX_TIMEOUT) {
		// timeout occured while transmitting packet
		Serial.println("[SX1262] timeout!");

	} else {
		// some other error occurred
		Serial.print("[SX1262] failed, code ");
		Serial.println(state);

	}

}