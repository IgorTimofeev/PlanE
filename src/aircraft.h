#pragma once

#include "SPI.h"
#include "ahrs.h"
#include "transceiver.h"

class Aircraft {
	public:
		void begin();
		void tick();

		AHRS& getAHRS();
		Transceiver& getTransceiver();

	private:
		AHRS _ahrs = AHRS();
		Transceiver _transceiver = Transceiver();
};