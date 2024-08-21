#pragma once

#ifndef PLANE_GY91_H
#define PLANE_GY91_H
#endif

#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_BMP280.h"
#include "quaternion_filter.h"
#include "MPU9250.h"

class GY91 {
	public:
		GY91() {

		}

		void begin() {
			SPI.begin();

			Serial.println("Starting BMP280");

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


			Serial.println("Starting MPU9250");

			// start communication with IMU
			if (_imu.begin() < 0) {
				Serial.println("IMU initialization unsuccessful");
				while(1) {}
			}

//			IMU.ConfigAccelRange(bfs::Mpu9250::ACCEL_RANGE_16G);
//			IMU.ConfigGyroRange(bfs::Mpu9250::GYRO_RANGE_2000DPS);
//			IMU.ConfigDlpfBandwidth(bfs::Mpu9250::DLPF_BANDWIDTH_184HZ);
//			IMU.ConfigSrd(0);

			_imu.setSrd(0);

			Serial.println("Calibrating");
			_imu.calibrateAccel();
			_imu.calibrateGyro();
			_imu.calibrateMag();
			Serial.println("Calibrating finished");

//			calibrate(70);
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

			Serial.print("[MPU] Mag degrees: ");
			Serial.println(degrees(atan2(_imu.getMagX_uT(), _imu.getMagY_uT())));

			Serial.println("-------------------------- BMP280 --------------------------");

			Serial.print("[MPU] Temperature: ");
			Serial.println(_imu.getTemperature_C());

			Serial.print("[BMP] Temperature: ");
			Serial.print(_bmp.readTemperature());
			Serial.println(" *C");

			Serial.print("[BMP] Pressure: ");
			Serial.print(_bmp.readPressure());
			Serial.println(" Pa");

			Serial.print("[BMP] Approx altitude: ");
			Serial.print(_bmp.readAltitude(1022));
			Serial.println(" m");

//			float an = -IMU.accel_x_mps2();
//			float ae = +IMU.accel_y_mps2();
//			float ad = +IMU.accel_z_mps2();
//			float gn = +IMU.gyro_x_radps();
//			float ge = -IMU.gyro_y_radps();
//			float gd = -IMU.gyro_z_radps();
//			float mn = +IMU.mag_z_ut();
//			float me = -IMU.mag_y_ut();
//			float md = +IMU.mag_z_ut();
//
//			quat_filter.update(an, ae, ad, gn, ge, gd, mn, me, md, q);
//
//			update_rpy(q[0], q[1], q[2], q[3]);
		}


//		void calibrate(uint8_t iterations) {
//			Serial.println("Calibration started");
//
//			for (float& _offset : _offsets)
//				_offset = 0;
//
//			for (uint8_t i = 0; i < iterations; i++) {
//				IMU.Read();
//
//				_offsets[0] += IMU.getAccelX_mss();
//				_offsets[1] += IMU.accel_y_mps2();
//				_offsets[2] += IMU.accel_z_mps2();
//
//				_offsets[3] += IMU.gyro_x_radps();
//				_offsets[4] += IMU.gyro_y_radps();
//				_offsets[5] += IMU.gyro_z_radps();
//
//				_offsets[6] += IMU.mag_x_ut();
//				_offsets[7] += IMU.mag_y_ut();
//				_offsets[8] += IMU.mag_z_ut();
//			}
//
//			for (float& _offset : _offsets)
//				_offset /= (float) iterations;
//
//			Serial.print("Calibration finished, the offsets are: ");
//
//			for (uint8_t i = 0; i < 9; i++) {
//				if (i > 0)
//					Serial.print(", ");
//
//				Serial.print(_offsets[i]);
//			}
//
//			Serial.println();
//		}
//
//		void update_rpy(float qw, float qx, float qy, float qz) {
//			// Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
//			// In this coordinate system, the positive z-axis is down toward Earth.
//			// Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking down on the sensor positive yaw is counterclockwise.
//			// Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
//			// Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
//			// These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
//			// Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
//			// applied in the correct order which for this configuration is yaw, pitch, and then roll.
//			// For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
//			float a12, a22, a31, a32, a33;  // rotation matrix coefficients for Euler angles and gravity components
//			a12 = 2.0f * (qx * qy + qw * qz);
//			a22 = qw * qw + qx * qx - qy * qy - qz * qz;
//			a31 = 2.0f * (qw * qx + qy * qz);
//			a32 = 2.0f * (qx * qz - qw * qy);
//			a33 = qw * qw - qx * qx - qy * qy + qz * qz;
//			rpy[0] = atan2f(a31, a33);
//			rpy[1] = -asinf(a32);
//			rpy[2] = atan2f(a12, a22);
//			rpy[0] *= 180.0f / PI;
//			rpy[1] *= 180.0f / PI;
//			rpy[2] *= 180.0f / PI;
////			rpy[2] += magnetic_declination;
//			if (rpy[2] >= +180.f)
//				rpy[2] -= 360.f;
//			else if (rpy[2] < -180.f)
//				rpy[2] += 360.f;
//
//			Serial.print("[MPU] Yaw pitch roll: ");
//			Serial.print(rpy[0]);
//			Serial.print(", ");
//			Serial.print(rpy[1]);
//			Serial.print(", ");
//			Serial.println(rpy[2]);
//		}

	private:
		float _offsets[9] {};
//		float rpy[3] {0.f, 0.f, 0.f};
//		float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};

//		QuaternionFilter quat_filter;

		MPU9250 _imu = MPU9250(SPI, 26);
		Adafruit_BMP280 _bmp = Adafruit_BMP280(27, &SPI);
};
