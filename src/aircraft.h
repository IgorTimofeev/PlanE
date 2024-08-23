#pragma once

#include "SPI.h"
#include "ahrs.h"
#include "transceiver.h"
#include <RadioLib.h>

class Aircraft {
	public:
		void begin();
		void tick();

		AHRS& getAHRS();
		Transceiver& getTransceiver();
		SX1262& getRadio();

	private:
		AHRS _ahrs = AHRS();
		Transceiver _transceiver = Transceiver();

		SX1262 _radio = new Module(
			32,
			RADIOLIB_NC,
			33,
			25
		);
};