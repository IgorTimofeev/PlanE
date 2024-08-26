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
		Adafruit_BMP280::MODE_NORMAL,
		Adafruit_BMP280::SAMPLING_X2,
		Adafruit_BMP280::SAMPLING_X16,
		Adafruit_BMP280::FILTER_X16,
		Adafruit_BMP280::STANDBY_MS_500
	);
}

void AHRS::tick(Aircraft &aircraft) {
	_imu.readSensor();

	_roll = atan2(_imu.getAccelY_mss(), _imu.getAccelZ_mss());

	_pitch = atan2(
		-_imu.getAccelX_mss(),
		sqrt(_imu.getAccelY_mss() * _imu.getAccelY_mss() + _imu.getAccelZ_mss() * _imu.getAccelZ_mss())
	);

	_yaw = atan2(_imu.getMagY_uT(), _imu.getMagX_uT());

	_temperature = _bmp.readTemperature();
	_pressure = _bmp.readPressure();
	_altitude = pressureToAltitude(_pressure, _qnh);

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

	Serial.print("[MPU9250] Yaw: ");
	Serial.println(degrees(_yaw));

	Serial.print("[MPU9250] Temperature: ");
	Serial.println(_imu.getTemperature_C());

	Serial.print("[BMP280] Temperature: ");
	Serial.print(_temperature);
	Serial.println(" *C");

	Serial.print("[BMP280] Pressure: ");
	Serial.print(_pressure);
	Serial.println(" Pa");

	Serial.print("[BMP280] Altitude: ");
	Serial.print(_altitude);
	Serial.println(" m");
}

float AHRS::pressureToAltitude(float pressureInPa, float seaLevelInPa) {
	return 44330.0f * (1.0f - powf(pressureInPa / seaLevelInPa, 1.0f / 5.255f));
}

float AHRS::getPitch() const {
	return _pitch;
}

float AHRS::getRoll() const {
	return _roll;
}

float AHRS::getYaw() const {
	return _yaw;
}

float AHRS::getTemperature() const {
	return _temperature;
}

float AHRS::getPressure() const {
	return _pressure;
}

float AHRS::getAltitude() const {
	return _altitude;
}

float AHRS::getQnhPa() const {
	return _qnh;
}

void AHRS::setQnhPa(float value) {
	_qnh = value;
}

float AHRS::getQnhMmHg() const {
	return _qnh / 0.0075006157584566f;
}

void AHRS::setQnhMmHg(float value) {
	_qnh = value * 0.0075006157584566f;
}

float AHRS::getQnhInHg() const {
	return _qnh / 0.0002952998057228486f;
}

void AHRS::setQnhInHg(float value) {
	_qnh = value * 0.0002952998057228486f;
}
