#include <Arduino.h>
#include "gy91.h"

GY91 gy91;

void setup() {
	Serial.begin(115200);

	gy91.begin();

}

void loop() {
	gy91.tick();

	delay(1000);
}