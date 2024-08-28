#include "aircraft.h"

void Aircraft::begin() {
	SPI.begin();

	_onboardLED.begin();
	_transceiver.begin();
	_ahrs.begin();
}

void Aircraft::tick() {
//	uint32_t startTime = millis();

	_ahrs.tick(*this);
	_transceiver.tick(*this);
	_onboardLED.tick();

//	uint32_t tickCost = millis() - startTime;
//
//	// Svit slip u stenki.........
//	if (tickCost < settings::application::tickBudget)
//		delay(settings::application::tickBudget - tickCost);
}

AHRS &Aircraft::getAHRS() {
	return _ahrs;
}

Transceiver &Aircraft::getTransceiver() {
	return _transceiver;
}

OnboardLED &Aircraft::getOnboardLed() {
	return _onboardLED;
}
