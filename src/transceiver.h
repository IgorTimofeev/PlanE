#pragma once

#ifndef PLANE_TRANSCEIVER_H
#define PLANE_TRANSCEIVER_H
#endif

#include "Arduino.h"
#include "RadioLib.h"

class Aircraft;

struct GovnoPacket {
	uint32_t signature;

	float pitch;
	float roll;
	float yaw;

	float temperature;
	float pressure;

	float qnh;
	float altitude;
};

class Transceiver {
	public:
		void begin();
		void tick(Aircraft& aircraft);

	private:
		SX1262 _radio = new Module(
			32,
			RADIOLIB_NC,
			33,
			25
		);
};