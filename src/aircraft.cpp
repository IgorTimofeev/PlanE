#include "aircraft.h"

void Aircraft::begin() {
	SPI.begin();

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
