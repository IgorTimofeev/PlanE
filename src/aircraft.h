#pragma once

#ifndef PLANE_AIRCRAFT_H
#define PLANE_AIRCRAFT_H
#endif

#include "ahrs.h"

class Aircraft {
	public:
		Aircraft() = default;

		void begin() {
			_ahrs.begin();
		}

		void tick() {
			_ahrs.tick();

			delay(1000);
		}

	private:
		AHRS _ahrs = AHRS();
};