#pragma once

#ifndef PLANE_BMP280_H
#define PLANE_BMP280_H
#endif

#include "SPI.h"

enum class BMP280Sampling : uint8_t {
	NONE = 0x00,
	X1 = 0x01,
	X2 = 0x02,
	X4 = 0x03,
	X8 = 0x04,
	X16 = 0x05
};

enum class BMP280Mode : uint8_t {
	SLEEP = 0x00,
	FORCED = 0x01,
	NORMAL = 0x03,
	SOFT_RESET = 0xB6
};

enum class BMP280Filter : uint8_t {
	OFF = 0x00,
	X2 = 0x01,
	X4 = 0x02,
	X8 = 0x03,
	X16 = 0x04
};

enum class BMP280StandbyDuration : uint8_t {
	MS_1 = 0x00,
	MS_63 = 0x01,
	MS_125 = 0x02,
	MS_250 = 0x03,
	MS_500 = 0x04,
	MS_1000 = 0x05,
	MS_2000 = 0x06,
	MS_4000 = 0x07
};

enum class BMP280Register : uint8_t {
	DIG_T1 = 0x88,
	DIG_T2 = 0x8A,
	DIG_T3 = 0x8C,

	DIG_P1 = 0x8E,
	DIG_P2 = 0x90,
	DIG_P3 = 0x92,
	DIG_P4 = 0x94,
	DIG_P5 = 0x96,
	DIG_P6 = 0x98,
	DIG_P7 = 0x9A,
	DIG_P8 = 0x9C,
	DIG_P9 = 0x9E,

	CHIP_ID = 0xD0,
	VERSION = 0xD1,
	SOFT_RESET = 0xE0,
	CALIBRATION_26 = 0xE1, /** From 0xE1 to 0xF0 */
	STATUS = 0xF3,
	CONTROL = 0xF4,
	CONFIG = 0xF5,
	PRESSURE_DATA = 0xF7,
	TEMPERATURE_DATA = 0xFA,
};

struct CalibrationData {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
};

/** Encapsulates the config register */
struct BMP280Config {
	/** Initialize to power-on-reset state */
	BMP280Config() :
		t_sb((unsigned int) BMP280StandbyDuration::MS_1),
		filter((unsigned int) BMP280Filter::OFF),
		none(0),
		spi3w_en(0)
	{

	}
	/** Inactive duration (standby time) in normal mode */
	unsigned int t_sb: 3;
	/** Filter settings */
	unsigned int filter: 3;
	/** Unused - don't set */
	unsigned int none : 1;
	/** Enables 3-wire SPI */
	unsigned int spi3w_en : 1;
	/** Used to retrieve the assembled config register's byte value. */
	unsigned int get() const { return (t_sb << 5) | (filter << 2) | spi3w_en; }
};

/** Encapsulates trhe ctrl_meas register */
struct BMP280ControlMeasurement {
	/** Initialize to power-on-reset state */
	BMP280ControlMeasurement() :
		osrs_t((unsigned int) BMP280Sampling::NONE),
		osrs_p((unsigned int) BMP280Sampling::NONE),
		mode((unsigned int) BMP280Mode::SLEEP)
	{

	}
	/** Temperature oversampling. */
	unsigned int osrs_t : 3;
	/** Pressure oversampling. */
	unsigned int osrs_p : 3;
	/** Device mode */
	unsigned int mode : 2;
	/** Used to retrieve the assembled ctrl_meas register's byte value. */
	unsigned int get() const { return (osrs_t << 5) | (osrs_p << 2) | mode; }
};

class BMP280 {
	public:
		explicit BMP280(uint8_t csPin) : _csPin(csPin) {

		}

		bool begin() {
			// Resetting CS pin just in case
			pinMode(_csPin, OUTPUT);
			setChipSelect(HIGH);

			// Initializing SPI bus if none
			SPI.begin();

			// check if sensor, i.e. the chip ID is correct
			auto chipID = readUint8(BMP280Register::CHIP_ID);
			if (chipID != 0x58) {
				Serial.print("Wrong chip id: ");
				Serial.println(chipID);
			}

			readCalibrationData();
			// write8(BMP280_REGISTER_CONTROL, 0x3F); /* needed? */
			configure();
			delay(100);

			return true;
		}

		void readGovno() {

		}

		void readCalibrationData() {
			_calibrationData.dig_T1 = readUint16LE(BMP280Register::DIG_T1);
			_calibrationData.dig_T2 = readInt16LE(BMP280Register::DIG_T2);
			_calibrationData.dig_T3 = readInt16LE(BMP280Register::DIG_T3);

			_calibrationData.dig_P1 = readUint16LE(BMP280Register::DIG_P1);
			_calibrationData.dig_P2 = readInt16LE(BMP280Register::DIG_P2);
			_calibrationData.dig_P3 = readInt16LE(BMP280Register::DIG_P3);
			_calibrationData.dig_P4 = readInt16LE(BMP280Register::DIG_P4);
			_calibrationData.dig_P5 = readInt16LE(BMP280Register::DIG_P5);
			_calibrationData.dig_P6 = readInt16LE(BMP280Register::DIG_P6);
			_calibrationData.dig_P7 = readInt16LE(BMP280Register::DIG_P7);
			_calibrationData.dig_P8 = readInt16LE(BMP280Register::DIG_P8);
			_calibrationData.dig_P9 = readInt16LE(BMP280Register::DIG_P9);

			Serial.println("------------ Calibration data ------------------------");
			Serial.println(_calibrationData.dig_T1);
			Serial.println(_calibrationData.dig_T2);
			Serial.println(_calibrationData.dig_T3);
			Serial.println();
			Serial.println(_calibrationData.dig_P1);
			Serial.println(_calibrationData.dig_P2);
			Serial.println(_calibrationData.dig_P3);
			Serial.println(_calibrationData.dig_P4);
			Serial.println(_calibrationData.dig_P5);
			Serial.println(_calibrationData.dig_P6);
			Serial.println(_calibrationData.dig_P7);
			Serial.println(_calibrationData.dig_P8);
			Serial.println(_calibrationData.dig_P9);
		}

		void configure(
			BMP280Mode mode = BMP280Mode::SLEEP,
			BMP280Sampling tempSampling = BMP280Sampling::NONE,
			BMP280Sampling pressSampling = BMP280Sampling::NONE,
			BMP280Filter filter = BMP280Filter::OFF,
			BMP280StandbyDuration duration = BMP280StandbyDuration::MS_1
		) {
			BMP280ControlMeasurement measReg;
			measReg.mode = (unsigned int) mode;
			measReg.osrs_t = (unsigned int) tempSampling;
			measReg.osrs_p = (unsigned int) pressSampling;

			BMP280Config configReg;
			configReg.filter =  (unsigned int) filter;
			configReg.t_sb =  (unsigned int) duration;

			writeUint8(BMP280Register::CONFIG, configReg.get());
			writeUint8(BMP280Register::CONTROL, measReg.get());
		}

		double readTemperature() {
			auto adc_T = readInt24BE(BMP280Register::TEMPERATURE_DATA);

			double var1, var2, T;
			var1 = (((double)adc_T)/16384.0 - ((double) _calibrationData.dig_T1)/1024.0) * ((double) _calibrationData.dig_T2);
			var2 = ((((double)adc_T)/131072.0 - ((double) _calibrationData.dig_T1)/8192.0) *
				(((double)adc_T)/131072.0 - ((double)  _calibrationData.dig_T1)/8192.0)) * ((double) _calibrationData.dig_T3);
			t_fine = (int32_t )(var1 + var2);
			T = (var1 + var2) / 5120.0;
			return T;
		}

		double readPressure() {
			auto adc_P = readInt24BE(BMP280Register::PRESSURE_DATA);

			double var1, var2, p;
			var1 = ((double)t_fine/2.0) - 64000.0;
			var2 = var1 * var1 * ((double) _calibrationData.dig_P6) / 32768.0;
			var2 = var2 + var1 * ((double) _calibrationData.dig_P5) * 2.0;
			var2 = (var2/4.0)+(((double) _calibrationData.dig_P4) * 65536.0);
			var1 = (((double) _calibrationData.dig_P3) * var1 * var1 / 524288.0 + ((double) _calibrationData.dig_P2) * var1) / 524288.0;
			var1 = (1.0 + var1 / 32768.0)*((double) _calibrationData.dig_P1);
			if (var1 == 0.0)
			{
				return 0; // avoid exception caused by division by zero
			}
			p = 1048576.0 - (double) adc_P;
			p = (p - (var2 / 4096.0)) * 6250.0 / var1;
			var1 = ((double) _calibrationData.dig_P9) * p * p / 2147483648.0;
			var2 = p * ((double) _calibrationData.dig_P8) / 32768.0;
			p = p + (var1 + var2 + ((double) _calibrationData.dig_P7)) / 16.0;
			return p;

		}

		float readAltitude(float seaLevelhPa) {
			float pressure = readPressure() / 100.0f;

			return 44330.0f * (1.0f - powf(pressure / seaLevelhPa, 0.1903f));
		}

	private:
		uint8_t _csPin;
		SPISettings _SPISettings = SPISettings(1000000, SPI_MSBFIRST, SPI_MODE0);
		CalibrationData _calibrationData = CalibrationData();
		int32_t t_fine = 0;

		void setChipSelect(uint8_t value) const {
			digitalWrite(_csPin, value);
		}

		void writeBuffer(uint8_t* buffer, int32_t size) {
			SPI.beginTransaction(_SPISettings);
			setChipSelect(LOW);

			SPI.transferBytes(buffer, nullptr, size);

			setChipSelect(HIGH);
			SPI.endTransaction();
		}

		void writeUint8(BMP280Register reg, uint8_t value) {
			// Writing
			uint8_t buffer[2] {
				(uint8_t) (uint8_t(reg) & ~0x80),
				value
			};

			writeBuffer(buffer, 2);
		}

		void writeThenRead(uint8_t* buffer, BMP280Register reg, uint32_t readSize) {
			SPI.beginTransaction(_SPISettings);
			setChipSelect(LOW);

			buffer[0] = (uint8_t) (uint8_t(reg) | 0x80);

			// Writing
			SPI.transferBytes(buffer, nullptr, 1);

			// Reading
			SPI.transfer(buffer, readSize);

			setChipSelect(HIGH);
			SPI.endTransaction();
		}

		uint8_t readUint8(BMP280Register reg) {
			uint8_t buffer[1];
			writeThenRead(buffer, reg, 1);

			return buffer[0];
		}

		// 16 BE
		uint16_t readUint16BE(BMP280Register reg) {
			uint8_t buffer[2];
			writeThenRead(buffer, reg, 2);

			return uint16_t(buffer[0]) << 8 | uint16_t(buffer[1]);
		}

		int16_t readInt16BE(BMP280Register reg) {
			return (int16_t) readUint16BE(reg);
		}

		// 16 LE
		uint16_t readUint16LE(BMP280Register reg) {
			uint8_t buffer[2];
			writeThenRead(buffer, reg, 2);

			return uint16_t(buffer[1]) << 8 | uint16_t(buffer[0]);
		}

		int16_t readInt16LE(BMP280Register reg) {
			return (int16_t) readUint16LE(reg);
		}

		// 32 BE
		int32_t readInt24BE(BMP280Register reg) {
			uint8_t buffer[3];
			writeThenRead(buffer, reg, 3);

			return int32_t(buffer[2] << 16) | int32_t(buffer[1]) << 8 | int32_t(buffer[0]);
		}
};