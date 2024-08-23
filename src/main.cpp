#include <Arduino.h>
#include "aircraft.h"
#include "SPI.h"

Aircraft aircraft;

void setup() {
	Serial.begin(115200);
	SPI.begin();

	aircraft.begin();
}

void loop() {
	aircraft.tick();
}