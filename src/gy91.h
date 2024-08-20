#pragma once

#ifndef PLANE_GY91_H
#define PLANE_GY91_H
#endif

#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_BMP280.h"
#include "mpu9250.h"

class GY91 {
	public:
		GY91() {

		}

		void begin() {
			SPI.begin();

			if (!_mpu.Begin()) {
				Serial.println("MPU9250 setup failed");

				while (1);
			}

			_mpu.ConfigAccelRange(bfs::Mpu9250::ACCEL_RANGE_16G);
			_mpu.ConfigGyroRange(bfs::Mpu9250::GYRO_RANGE_2000DPS);
			_mpu.ConfigDlpfBandwidth(bfs::Mpu9250::DLPF_BANDWIDTH_184HZ);
			_mpu.ConfigSrd(0);

			if (!_bmp.begin()) {
				Serial.println("BMP280 setup failed");

				while (1);
			}

			_bmp.setSampling(
				Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
				Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
				Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
				Adafruit_BMP280::FILTER_X16,      /* Filtering. */
				Adafruit_BMP280::STANDBY_MS_500 /* Standby time. */
			);

			calibrate(70);
		}

		void tick() {
			/* Check if data read */
			if (_mpu.Read()) {
				Serial.println("-------------------------- Govno --------------------------");

				Serial.print("[BMP] New imu data / mag data: ");
				Serial.print(_mpu.new_imu_data());
				Serial.print(", ");
				Serial.println(_mpu.new_mag_data());

				float x, y, z;

				getAccel(x, y, z);
				Serial.print("[BMP] Accel: ");
				Serial.print(x);
				Serial.print(", ");
				Serial.print(y);
				Serial.print(", ");
				Serial.println(z);

				getGyro(x, y, z);
				Serial.print("[BMP] Gyro: ");
				Serial.print(x);
				Serial.print(", ");
				Serial.print(y);
				Serial.print(", ");
				Serial.println(z);

				getMag(x, y, z);
				Serial.print("[BMP] Mag: ");
				Serial.print(x);
				Serial.print(", ");
				Serial.print(y);
				Serial.print(", ");
				Serial.println(z);

				Serial.print("[BMP] Temp: ");
				Serial.println(_mpu.die_temp_c());
			}

			Serial.print("[BMP] Temperature: ");
			Serial.print(_bmp.readTemperature());
			Serial.println(" *C");

			Serial.print("[BMP] Pressure: ");
			Serial.print(_bmp.readPressure());
			Serial.println(" Pa");

			Serial.print("[BMP] Approx altitude: ");
			Serial.print(_bmp.readAltitude(1022));
			Serial.println(" m");
		}

		void getAccel(float& x, float& y, float& z) {
			x = _mpu.accel_x_mps2() - _offsets[0];
			y = _mpu.accel_y_mps2() - _offsets[1];
			z = _mpu.accel_z_mps2() - _offsets[2];
		}

		void getGyro(float& x, float& y, float& z) {
			x = _mpu.gyro_x_radps() - _offsets[3];
			y = _mpu.gyro_y_radps() - _offsets[4];
			z = _mpu.gyro_z_radps() - _offsets[5];
		}

		void getMag(float& x, float& y, float& z) {
			x = _mpu.mag_x_ut() - _offsets[6];
			y = _mpu.mag_y_ut() - _offsets[7];
			z = _mpu.mag_z_ut() - _offsets[8];
		}

		void calibrate(uint8_t iterations) {
			Serial.println("Calibration started");

			for (float& _offset : _offsets)
				_offset = 0;

			for (uint8_t i = 0; i < iterations; i++) {
				_mpu.Read();

				_offsets[0] += _mpu.accel_x_mps2();
				_offsets[1] += _mpu.accel_y_mps2();
				_offsets[2] += _mpu.accel_z_mps2();

				_offsets[3] += _mpu.gyro_x_radps();
				_offsets[4] += _mpu.gyro_y_radps();
				_offsets[5] += _mpu.gyro_z_radps();

				_offsets[6] += _mpu.mag_x_ut();
				_offsets[7] += _mpu.mag_y_ut();
				_offsets[8] += _mpu.mag_z_ut();
			}

			for (float& _offset : _offsets)
				_offset /= (float) iterations;

			Serial.print("Calibration finished, the offsets are: ");

			for (uint8_t i = 0; i < 9; i++) {
				if (i > 0)
					Serial.print(", ");

				Serial.print(_offsets[i]);
			}

			Serial.println();
		}

	private:
		float _offsets[9] {};

		bfs::Mpu9250 _mpu = bfs::Mpu9250(&SPI, 26);
		Adafruit_BMP280 _bmp = Adafruit_BMP280(27, &SPI);
};
