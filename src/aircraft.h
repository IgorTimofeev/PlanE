#pragma once

#ifndef PLANE_AIRCRAFT_H
#define PLANE_AIRCRAFT_H
#endif

#include "SPI.h"
#include "ahrs.h"
#include "transceiver.h"
#include <RadioLib.h>

class Aircraft {
	public:
		Aircraft() = default;

		void begin() {
			SPI.begin();

//			// Resetting CS pins just in case
//			pinMode(26, OUTPUT);
//			digitalWrite(26, HIGH);
//
//			pinMode(27, OUTPUT);
//			digitalWrite(27, HIGH);
//
//			pinMode(5, OUTPUT);
//			digitalWrite(5, HIGH);

			_transceiver.begin();
			_ahrs.begin();
		}

		void tick() {
			_ahrs.tick();
			_transceiver.tick();
		}

	private:
		AHRS _ahrs = AHRS();
		Transceiver _transceiver = Transceiver();
};