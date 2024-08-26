#pragma once

#ifndef PLANE_AHRS_H
#define PLANE_AHRS_H
#endif

#include "Arduino.h"
#include "SPI.h"
#include "MPU9250.h"
#include "Adafruit_BMP280.h"
#include "quaternion_filter.h"

class Aircraft;

class AHRS {
	public:
		void begin();
		void tick(Aircraft& aircraft);

		static float pressureToAltitude(float pressureInPa, float seaLevelInPa);

		float getPitch() const;
		float getRoll() const;
		float getYaw() const;
		float getTemperature() const;
		float getPressure() const;
		float getAltitude() const;

		float getQnhPa() const;
		void setQnhPa(float value);

		float getQnhMmHg() const;
		void setQnhMmHg(float value);

		float getQnhInHg() const;
		void setQnhInHg(float value);

	private:
		MPU9250 _imu = MPU9250(Wire, 0x68);
		Adafruit_BMP280 _bmp = Adafruit_BMP280(&Wire);

		float _pitch = 0;
		float _roll = 0;
		float _yaw = 0;
		float _temperature = 0;
		float _pressure = 0;
		float _altitude = 0;

		float _qnh = 1013;
};
