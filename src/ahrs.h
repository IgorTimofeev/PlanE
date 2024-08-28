#pragma once

#ifndef PLANE_AHRS_H
#define PLANE_AHRS_H
#endif

#include "Arduino.h"
#include "SPI.h"
#include "MPU9250.h"
#include "Adafruit_BMP280.h"
#include "quaternion_filter.h"
#include "packet.h"

class Aircraft;

class LocalData {
	public:
		float getPitch() const;
		void setPitch(float pitch);

		float getRoll() const;
		void setRoll(float roll);

		float getYaw() const;
		void setYaw(float yaw);

		float getTemperature() const;
		void setTemperature(float temperature);

		float getPressure() const;
		void setPressure(float pressure);

		float getAltitude() const;
		void setAltitude(float altitude);

		float getSpeed() const;
		void setSpeed(float speed);

	private:
		float _pitch = 0;
		float _roll = 0;
		float _yaw = 0;
		float _temperature = 0;
		float _pressure = 0;
		float _altitude = 0;
		float _speed = 0;
};

class RemoteData {
	public:
		uint8_t getThrottle() const;
		void setThrottle(uint8_t throttle);

		uint8_t getAilerons() const;
		void setAilerons(uint8_t ailerons);

		uint8_t getRudder() const;
		void setRudder(uint8_t rudder);

		uint8_t getFlaps() const;
		void setFlaps(uint8_t flaps);

		AltimeterMode getAltimeterMode() const;
		void setAltimeterMode(AltimeterMode altimeterMode);

		float getAltimeterPressure() const;
		void setAltimeterPressure(float altimeterPressure);

		bool getStrobeLights() const;
		void setStrobeLights(bool strobeLights);

	private:
		uint8_t _throttle;
		uint8_t _ailerons;
		uint8_t _rudder;
		uint8_t _flaps;

		AltimeterMode _altimeterMode = AltimeterMode::QNH;
		float _altimeterPressure = 100792;

		bool _strobeLights;
};

class AHRS {
	public:
		void begin();
		void tick(Aircraft& aircraft);

		LocalData &getLocalData();
		RemoteData &getRemoteData();

	private:
		MPU9250 _imu = MPU9250(Wire, 0x68);
		Adafruit_BMP280 _bmp = Adafruit_BMP280(&Wire);
		uint32_t _tickDeadline = 0;

		LocalData _localData = LocalData();
		RemoteData _remoteData = RemoteData();

		void updateAltitude();
};
