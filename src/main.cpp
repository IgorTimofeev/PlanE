#include <Arduino.h>
#include "aircraft.h"
#include "Wire.h"

Aircraft aircraft;

void setup() {
	Serial.begin(115200);

	aircraft.begin();
}

void loop() {
	aircraft.tick();
}