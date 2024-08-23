#pragma once

#ifndef PLANE_TRANSCEIVER_H
#define PLANE_TRANSCEIVER_H
#endif

#include "Arduino.h"
#include <Ra01S.h>

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
		SX126x _radio = SX126x(
			32,               //Port-Pin Output: SPI select
			33,               //Port-Pin Output: Reset
			25                //Port-Pin Input:  Busy
		);
};