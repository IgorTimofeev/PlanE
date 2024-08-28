#pragma once

#ifndef PLANE_ONBOARD_LED_H
#define PLANE_ONBOARD_LED_H
#endif

#include "Arduino.h"
#include "settings.h"

class OnboardLED {
	public:
		void begin() {
			updateLED();
		}

		void blink() {
			_deadline = millis() + 10;
			updateLED();
		}

		void tick() {
			if (_deadline == 0 || millis() < _deadline)
				return;

			_deadline = 0;
			updateLED();
		}

	private:
		uint32_t _deadline = 0;

		void updateLED() const {
			if (_deadline > 0) {
				neopixelWrite(settings::pinout::aircraft::onboardLED, 0x99, 0x99, 0x99);
			}
			else {
				neopixelWrite(settings::pinout::aircraft::onboardLED, 0, 0, 0);
			}
		}
};