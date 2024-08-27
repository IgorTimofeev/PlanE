#pragma once

#ifndef PLANE_SETTINGS_H
#define PLANE_SETTINGS_H
#endif

#include "cstdint"

namespace settings {
	namespace pinout {
		namespace transceiver {
			const uint8_t chipSelect = 32;
			const uint8_t reset = 33;
			const uint8_t busy = 25;
		}
	}

	namespace transceiver {
		const uint32_t packetHeader = 0x506C416E;
	}
}
