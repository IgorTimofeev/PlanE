#include <Arduino.h>
#include "aircraft.h"
#include "SPI.h"

Aircraft aircraft;

void setup() {
	Serial.begin(115200);

	aircraft.begin();
}

void loop() {
	aircraft.tick();

	delay(1000);
}