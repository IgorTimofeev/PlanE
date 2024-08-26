#pragma once

#ifndef PLANE_SETTINGS_H
#define PLANE_SETTINGS_H
#endif

#include "cstdint"

class Settings {
	public:
		class Pinout {
			public:
				class Transceiver {
					public:
						static const uint8_t chipSelect = 32;
						static const uint8_t reset = 33;
						static const uint8_t busy = 25;
				};
		};

		class Transceiver {
			public:
				static const uint32_t packetHeader = 0x506C416E;
		};
};
