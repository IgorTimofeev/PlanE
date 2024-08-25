#include "ahrs.h"
#include "aircraft.h"

void AHRS::begin() {
	Serial.println("[MPU9250] Initializing");

	// start communication with IMU
	if (_imu.begin() < 0) {
		Serial.println("[MPU9250] Initialization failed");

		while (1)
			delay(100);
	}

	_imu.setSrd(0);

	Serial.println("[MPU9250] Calibrating");
//	_imu.calibrateAccel();
	// Already calibrated in begin()?
	// _imu.calibrateGyro();
//	_imu.calibrateMag();

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

void AHRS::tick(Aircraft &aircraft) {
	_imu.readSensor();
	_roll = atan2(_imu.getAccelY_mss() , _imu.getAccelZ_mss());
	_pitch = atan2((-_imu.getAccelX_mss()) , sqrt(_imu.getAccelY_mss() * _imu.getAccelY_mss() + _imu.getAccelZ_mss() * _imu.getAccelZ_mss()));

	Serial.print("[MPU9250] Acc: ");
	Serial.print(_imu.getAccelX_mss());
	Serial.print(", ");
	Serial.print(_imu.getAccelY_mss());
	Serial.print(", ");
	Serial.println(_imu.getAccelZ_mss());

	Serial.print("[MPU9250] Pitch / roll: ");
	Serial.print(degrees(_pitch));
	Serial.print(", ");
	Serial.println(degrees(_roll));

	Serial.print("[MPU9250] Gyro: ");
	Serial.print(_imu.getGyroX_rads());
	Serial.print(", ");
	Serial.print(_imu.getGyroY_rads());
	Serial.print(", ");
	Serial.println(_imu.getGyroZ_rads());

	Serial.print("[MPU9250] Mag: ");
	Serial.print(_imu.getMagX_uT());
	Serial.print(", ");
	Serial.print(_imu.getMagY_uT());
	Serial.print(", ");
	Serial.println(_imu.getMagZ_uT());

	_yaw = atan2(_imu.getMagY_uT(), _imu.getMagX_uT());
	Serial.print("[MPU9250] Yaw: ");
	Serial.println(degrees(_yaw));

	Serial.print("[MPU9250] Temperature: ");
	Serial.println(_imu.getTemperature_C());

	_temperature = _bmp.readTemperature();
	Serial.print("[BMP280] Temperature: ");
	Serial.print(_temperature);
	Serial.println(" *C");

	_pressure = _bmp.readPressure();
	Serial.print("[BMP280] Pressure: ");
	Serial.print(_pressure);
	Serial.println(" Pa");

	_altitude = pressureToAltitude(_pressure, _qnh);
	Serial.print("[BMP280] Altitude: ");
	Serial.print(_altitude);
	Serial.println(" m");
}

float AHRS::pressureToAltitude(float pressureInPa, float seaLevelInPa) {
	return 44330.0f * (1.0f - powf(pressureInPa / seaLevelInPa, 1.0f / 5.255f));
}

float AHRS::getPitch() {
	return _pitch;
}

float AHRS::getRoll() {
	return _roll;
}

float AHRS::getYaw() {
	return _yaw;
}

float AHRS::getTemperature() {
	return _temperature;
}

float AHRS::getPressure() {
	return _pressure;
}

float AHRS::getAltitude() {
	return _altitude;
}

float AHRS::getQnh() const {
	return _qnh;
}

void AHRS::setQnh(float qnh) {
	_qnh = qnh;
}
