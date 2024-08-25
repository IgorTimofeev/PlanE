#pragma once

#ifndef PLANE_TRANSCEIVER_H
#define PLANE_TRANSCEIVER_H
#endif

#include "Arduino.h"
#include <Ra01S.h>
#include <aes/esp_aes.h>

class Aircraft;

class Transceiver {
	public:
		void begin();
		void tick(Aircraft& aircraft);

	private:
		SX126x _radio = SX126x(32, 33, 25);
};