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
		GY91() = default;

		void begin() {
			mpu.begin();

//			if (!mpu.begin()) {
//				Serial.println("MPU9250 setup failed");
//
//				while (1);
//			}

//			_mpu.ConfigAccelRange(bfs::Mpu9250::ACCEL_RANGE_16G);
//			_mpu.ConfigGyroRange(bfs::Mpu9250::GYRO_RANGE_2000DPS);
//			_mpu.ConfigDlpfBandwidth(bfs::Mpu9250::DLPF_BANDWIDTH_184HZ);
//			_mpu.ConfigSrd(0);



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
		}

		void tick() {
			//Accel
			mpu.set_accel_range(RANGE_4G);
			mpu.get_accel();
			Serial.print("X: ");  Serial.print(mpu.x);
			Serial.print(" Y: "); Serial.print(mpu.y);
			Serial.print(" Z: "); Serial.print(mpu.z);

			mpu.get_accel_g();
			Serial.print(" X_g: "); Serial.print(mpu.x_g,2);
			Serial.print(" Y_g: "); Serial.print(mpu.y_g,2);
			Serial.print(" Z_g: "); Serial.print(mpu.z_g,2);  Serial.println(" G");

			//Gyro
			mpu.set_gyro_range(RANGE_GYRO_250);
			mpu.get_gyro();
			Serial.print("GX: ");  Serial.print(mpu.gx);
			Serial.print(" GY: "); Serial.print(mpu.gy);
			Serial.print(" GZ: "); Serial.print(mpu.gz);

			mpu.get_gyro_d();
			Serial.print(" GX_g: "); Serial.print(mpu.gx_d,2);
			Serial.print(" GY_g: "); Serial.print(mpu.gy_d,2);
			Serial.print(" GZ_g: "); Serial.print(mpu.gz_d,2); Serial.println(" º/s");

			//Mag
			mpu.set_mag_scale(SCALE_14_BITS);
			mpu.set_mag_speed(MAG_8_Hz);
			if(!mpu.get_mag()){
				Serial.print("MX: ");  Serial.print(mpu.mx);
				Serial.print(" MY: "); Serial.print(mpu.my);
				Serial.print(" MZ: "); Serial.print(mpu.mz);

				mpu.get_mag_t();
				Serial.print(" MX_t: "); Serial.print(mpu.mx_t,2);
				Serial.print(" MY_t: "); Serial.print(mpu.my_t,2);
				Serial.print(" MZ_t: "); Serial.print(mpu.mz_t,2); Serial.println(" uT");
			}
			else{
				// |X|+|Y|+|Z| must be < 4912μT to sensor measure correctly
				Serial.println("Overflow no magnetometro.");
			}

			// Temp
			Serial.print("Temperature is ");  Serial.print((((float) mpu.get_temp()) / 333.87 + 21.0), 1);  Serial.println(" degrees C");


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


	private:

		MPU9250 mpu = MPU9250();
		Adafruit_BMP280 _bmp = Adafruit_BMP280();
};
