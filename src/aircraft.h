#pragma once

#ifndef PLANE_AIRCRAFT_H
#define PLANE_AIRCRAFT_H
#endif

#include "ahrs.h"
#include <RadioLib.h>
//#include <Ra01S.h>

class Aircraft {
	public:
		Aircraft() = default;

		void begin() {
//			// Resetting CS pins just in case
//			pinMode(26, OUTPUT);
//			digitalWrite(26, HIGH);
//
//			pinMode(27, OUTPUT);
//			digitalWrite(27, HIGH);
//
//			pinMode(5, OUTPUT);
//			digitalWrite(5, HIGH);

			SPI.begin();

			Serial.print("[SX1262] Initializing ...");
			auto state = _radio.begin(866.0, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0);

			if (state == RADIOLIB_ERR_NONE) {
				Serial.println("[SX1262] Success");
			}
			else {
				Serial.print("[SX1262] Failure, code: ");
				Serial.println(state);

				while (true)
					delay(100);
			}

//			_ahrs.begin();
		}

		void tick() {
//			_ahrs.tick();

			delay(1000);
		}

	private:
//		AHRS _ahrs = AHRS();
//
//
//		SX126x  _radio = SX126x (
//			32,               //Port-Pin Output: SPI select
//			33,               //Port-Pin Output: Reset
//			25               //Port-Pin Input:  Busy
//		);


		SX1262 _radio = new Module(
			32,
			RADIOLIB_NC,
			33,
			25
		);
};