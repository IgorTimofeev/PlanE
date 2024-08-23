#include "aircraft.h"

void Aircraft::begin() {
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

void Aircraft::tick() {
	_ahrs.tick(*this);
	_transceiver.tick(*this);
}

AHRS &Aircraft::getAHRS() {
	return _ahrs;
}

Transceiver &Aircraft::getTransceiver() {
	return _transceiver;
}
