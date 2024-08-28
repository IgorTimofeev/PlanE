#pragma once

#include "SPI.h"
#include "ahrs.h"
#include "transceiver.h"
#include "onboard_led.h"

class Aircraft {
	public:
		void begin();
		void tick();

		AHRS& getAHRS();
		Transceiver& getTransceiver();
		OnboardLED &getOnboardLed();

	private:
		AHRS _ahrs = AHRS();
		Transceiver _transceiver = Transceiver();
		OnboardLED _onboardLED = OnboardLED();
};