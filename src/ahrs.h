#pragma once

#ifndef PLANE_GY91_H
#define PLANE_GY91_H
#endif

#include "Arduino.h"
#include "SPI.h"
#include "quaternion_filter.h"
#include "MPU9250.h"
//#include "BMP280.h"
#include "Adafruit_BMP280.h"

class AHRS {
	public:
		AHRS() = default;

		void begin() {
			Serial.println("[MPU9250] Initializing");

			// start communication with IMU
			if (_imu.begin() < 0) {
				Serial.println("[MPU9250] Initialization failed");

				while (1)
					delay(100);
			}

			_imu.setSrd(0);

			Serial.println("Calibrating");
			_imu.calibrateAccel();
			_imu.calibrateGyro();
			_imu.calibrateMag();
			Serial.println("Calibrating finished");

			Serial.println("[BMP280] Initializing");

			if (!_bmp.begin(0x76)) {
				Serial.println("[BMP280] Initialization failed");

				while (1)
					delay(100);
			}

			_bmp.setSampling(
				Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
				Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
				Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
				Adafruit_BMP280::FILTER_X16,      /* Filtering. */
				Adafruit_BMP280::STANDBY_MS_500
			); /* Standby time. */
		}

		void tick() {
			Serial.println("-------------------------- MP9250 --------------------------");

			_imu.readSensor();
			Serial.print("[MPU] Accel: ");
			Serial.print(_imu.getAccelX_mss());
			Serial.print(", ");
			Serial.print(_imu.getAccelY_mss());
			Serial.print(", ");
			Serial.println(_imu.getAccelZ_mss());

			Serial.print("Pitch / roll: ");
			auto roll = atan2(_imu.getAccelY_mss() , _imu.getAccelZ_mss());
			auto pitch = atan2((-_imu.getAccelX_mss()) , sqrt(_imu.getAccelY_mss() * _imu.getAccelY_mss() + _imu.getAccelZ_mss() * _imu.getAccelZ_mss()));
			Serial.print(degrees(pitch));
			Serial.print(", ");
			Serial.println(degrees(roll));

			Serial.print("Mag rotation: ");
			Serial.println(degrees(atan2(_imu.getMagY_uT(), _imu.getMagX_uT())));

			Serial.print("[MPU] Gyro: ");
			Serial.print(_imu.getGyroX_rads());
			Serial.print(", ");
			Serial.print(_imu.getGyroY_rads());
			Serial.print(", ");
			Serial.println(_imu.getGyroZ_rads());

			Serial.print("[MPU] Mag: ");
			Serial.print(_imu.getMagX_uT());
			Serial.print(", ");
			Serial.print(_imu.getMagY_uT());
			Serial.print(", ");
			Serial.println(_imu.getMagZ_uT());
			Serial.print("[MPU] Temperature: ");
			Serial.println(_imu.getTemperature_C());

			Serial.print("[MPU] Mag degrees: ");
			Serial.println(degrees(atan2(_imu.getMagX_uT(), _imu.getMagY_uT())));

			Serial.println("-------------------------- BMP280 --------------------------");

			Serial.print("Temperature: ");
			Serial.print(_bmp.readTemperature());
			Serial.println(" *C");

			auto pressure = _bmp.readPressure();
			Serial.print("Pressure: ");
			Serial.print(pressure);
			Serial.println(" Pa");

			Serial.print("Altitude: ");
			Serial.print(pressureToAltitude(pressure, 102200));
			Serial.println(" m");
		}

		static float pressureToAltitude(float pressureInPa, float seaLevelInPa) {
			return 44330.0f * (1.0f - powf(pressureInPa / seaLevelInPa, 1.0f / 5.255f));
		}

	private:
		MPU9250 _imu = MPU9250(Wire, 0x68);
		Adafruit_BMP280 _bmp = Adafruit_BMP280(&Wire);
//		BMP280 _bmp = BMP280(27);
};
