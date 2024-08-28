#pragma once

#ifndef PLANE_SETTINGS_H
#define PLANE_SETTINGS_H
#endif

#include "cstdint"

namespace settings {
	namespace pinout {
		namespace board {
			const uint8_t onboardLED = 48;
		}

		namespace transceiver {
			const uint8_t chipSelect = 10;
			const uint8_t reset = 6;
			const uint8_t busy = 7;
			const uint8_t dio0 = 5;
		}
	}

	namespace application {
		const uint32_t tickBudget = 1000 / 60;
	}

	namespace transceiver {
		const uint32_t tickInterval = 1000 / 10;

		const uint32_t packetHeader = 0x506C416E;
		const float frequency = 915;
		const float bandwidth = 500;
		const uint8_t spreadingFactor = 7;
		const uint8_t codingRate = 5;
		const uint16_t power = 22;
		const uint16_t preambleLength = 8;
	}
}
