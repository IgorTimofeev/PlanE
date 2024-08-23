#pragma once

#ifndef PLANE_TRANSCEIVER_H
#define PLANE_TRANSCEIVER_H
#endif

#include "Arduino.h"
#include "RadioLib.h"

class Transceiver {
	public:
		Transceiver() {

		}

		void begin() {
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

		void tick() {

		}

	private:
		uint8_t _csPin;

		SX1262 _radio = new Module(
			32,
			RADIOLIB_NC,
			33,
			25
		);
};