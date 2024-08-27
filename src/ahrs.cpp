#include "ahrs.h"
#include "aircraft.h"

void AHRS::begin() {
	Serial.println("[MPU9250] Initializing");

	auto& wire = Wire;

	// start communication with IMU
	if (_imu.begin() < 0) {
		Serial.println("[MPU9250] Initialization failed");

		while (1)
			delay(100);
	}

	_imu.setSrd(0);

	Serial.println("[MPU9250] Calibrating");
	// // Already calibrated in begin()?
	// // _imu.calibrateGyro();
	_imu.calibrateAccel();
	_imu.calibrateMag();

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

	_localData.setRoll(atan2(_imu.getAccelY_mss(), -_imu.getAccelZ_mss()));

	_localData.setPitch(atan2(
		_imu.getAccelX_mss(),
		sqrt(_imu.getAccelY_mss() * _imu.getAccelY_mss() + _imu.getAccelZ_mss() * _imu.getAccelZ_mss())
	));

	_localData.setYaw(atan2(_imu.getMagY_uT(), _imu.getMagX_uT()));

	_localData.setTemperature(_bmp.readTemperature());
	_localData.setPressure(_bmp.readPressure());
	updateAltitude();

	_localData.setSpeed(_localData.getSpeed() + 1);
	if (_localData.getSpeed() > 350)
		_localData.setSpeed(0);

//	Serial.print("[MPU9250] Acc: ");
//	Serial.print(_imu.getAccelX_mss());
//	Serial.print(", ");
//	Serial.print(_imu.getAccelY_mss());
//	Serial.print(", ");
//	Serial.println(_imu.getAccelZ_mss());
//
//	Serial.print("[MPU9250] Pitch / roll: ");
//	Serial.print(degrees(_localData.getPitch()));
//	Serial.print(", ");
//	Serial.println(degrees(_localData.getRoll()));
//
//	Serial.print("[MPU9250] Gyro: ");
//	Serial.print(_imu.getGyroX_rads());
//	Serial.print(", ");
//	Serial.print(_imu.getGyroY_rads());
//	Serial.print(", ");
//	Serial.println(_imu.getGyroZ_rads());
//
//	Serial.print("[MPU9250] Mag: ");
//	Serial.print(_imu.getMagX_uT());
//	Serial.print(", ");
//	Serial.print(_imu.getMagY_uT());
//	Serial.print(", ");
//	Serial.println(_imu.getMagZ_uT());
//
//	Serial.print("[MPU9250] Yaw: ");
//	Serial.println(degrees(_localData.getYaw()));
//
//	Serial.print("[MPU9250] Temperature: ");
//	Serial.println(_imu.getTemperature_C());
//
//	Serial.print("[BMP280] Temperature: ");
//	Serial.print(_localData.getTemperature());
//	Serial.println(" *C");
//
//	Serial.print("[BMP280] Pressure: ");
//	Serial.print(_localData.getPressure());
//	Serial.println(" Pa");
//
//	Serial.print("[BMP280] Altitude: ");
//	Serial.print(_localData.getAltitude());
//	Serial.println(" m");
//
//	Serial.print("[BMP280] Speed: ");
//	Serial.print(_localData.getSpeed());
//	Serial.println(" m");
}

LocalData &AHRS::getLocalData() {
	return _localData;
}

RemoteData &AHRS::getRemoteData() {
	return _remoteData;
}

void AHRS::updateAltitude() {
	const float seaLevelPressure =
		_remoteData.getAltimeterMode() == AltimeterMode::QNH
		? _remoteData.getAltimeterPressure()
		: 101300.0f;

	_localData.setAltitude(44330.0f * (1.0f - powf(_localData.getPressure() / seaLevelPressure, 1.0f / 5.255f)));
}

float LocalData::getPitch() const {
	return _pitch;
}

void LocalData::setPitch(float pitch) {
	_pitch = pitch;
}

float LocalData::getRoll() const {
	return _roll;
}

void LocalData::setRoll(float roll) {
	_roll = roll;
}

float LocalData::getYaw() const {
	return _yaw;
}

void LocalData::setYaw(float yaw) {
	_yaw = yaw;
}

float LocalData::getTemperature() const {
	return _temperature;
}

void LocalData::setTemperature(float temperature) {
	_temperature = temperature;
}

float LocalData::getPressure() const {
	return _pressure;
}

void LocalData::setPressure(float pressure) {
	_pressure = pressure;
}

float LocalData::getAltitude() const {
	return _altitude;
}

void LocalData::setAltitude(float altitude) {
	_altitude = altitude;
}

float LocalData::getSpeed() const {
	return _speed;
}

void LocalData::setSpeed(float speed) {
	_speed = speed;
}

uint8_t RemoteData::getThrottle() const {
	return _throttle;
}

void RemoteData::setThrottle(uint8_t throttle) {
	_throttle = throttle;
}

uint8_t RemoteData::getAilerons() const {
	return _ailerons;
}

void RemoteData::setAilerons(uint8_t ailerons) {
	_ailerons = ailerons;
}

uint8_t RemoteData::getRudder() const {
	return _rudder;
}

void RemoteData::setRudder(uint8_t rudder) {
	_rudder = rudder;
}

uint8_t RemoteData::getFlaps() const {
	return _flaps;
}

void RemoteData::setFlaps(uint8_t flaps) {
	_flaps = flaps;
}

AltimeterMode RemoteData::getAltimeterMode() const {
	return _altimeterMode;
}

void RemoteData::setAltimeterMode(AltimeterMode altimeterMode) {
	_altimeterMode = altimeterMode;
}

float RemoteData::getAltimeterPressure() const {
	return _altimeterPressure;
}

void RemoteData::setAltimeterPressure(float altimeterPressure) {
	_altimeterPressure = altimeterPressure;
}

bool RemoteData::getStrobeLights() const {
	return _strobeLights;
}

void RemoteData::setStrobeLights(bool strobeLights) {
	_strobeLights = strobeLights;
}
