#pragma once

#ifndef BMP280_H
#define BMP280_H
#endif

#include "SPI.h"

enum class BMP280Oversampling : uint8_t {
	None = 0x00,
	X1 = 0x01,
	X2 = 0x02,
	X4 = 0x03,
	X8 = 0x04,
	X16 = 0x05
};

enum class BMP280Mode : uint8_t {
	Sleep = 0x00,
	Forced = 0x01,
	Normal = 0x03,
	SoftReset = 0xB6
};

enum class BMP280Filter : uint8_t {
	None = 0x00,
	X2 = 0x01,
	X4 = 0x02,
	X8 = 0x03,
	X16 = 0x04
};

enum class BMP280StandbyDuration : uint8_t {
	Ms1 = 0x00,
	Ms63 = 0x01,
	Ms125 = 0x02,
	Ms250 = 0x03,
	Ms500 = 0x04,
	Ms1000 = 0x05,
	Ms2000 = 0x06,
	Ms4000 = 0x07
};

enum class BMP280Register : uint8_t {
	DigT1 = 0x88,
	DigT2 = 0x8A,
	DigT3 = 0x8C,

	DigP1 = 0x8E,
	DigP2 = 0x90,
	DigP3 = 0x92,
	DigP4 = 0x94,
	DigP5 = 0x96,
	DigP6 = 0x98,
	DigP7 = 0x9A,
	DigP8 = 0x9C,
	DigP9 = 0x9E,

	ChipID = 0xD0,
	Version = 0xD1,
	SoftReset = 0xE0,
	Calibration = 0xE1,
	Status = 0xF3,
	Control = 0xF4,
	Config = 0xF5,
	PressureData = 0xF7,
	TemperatureData = 0xFA,
};

class BMP280 {
	public:
		explicit BMP280(uint8_t csPin) : _csPin(csPin) {

		}

		void begin(
			BMP280Mode mode = BMP280Mode::Normal,
			BMP280Oversampling temperatureOversampling = BMP280Oversampling::X2,
			BMP280Oversampling pressureOversampling = BMP280Oversampling::X16,
			BMP280Filter filter = BMP280Filter::X16,
			BMP280StandbyDuration standbyDuration = BMP280StandbyDuration::Ms125
		) {
			// Resetting CS pin just in case
			pinMode(_csPin, OUTPUT);
			setChipSelect(HIGH);

			// Initializing SPI bus if none
			SPI.begin();

			// Reading factory-fused calibration offsets
			readCalibrationData();

			// Configuring sensors at least once
			configure(
				mode,
				temperatureOversampling,
				pressureOversampling,
				filter,
				standbyDuration
			);
		}

		void configure(
			BMP280Mode mode,
			BMP280Oversampling temperatureOversampling,
			BMP280Oversampling pressureOversampling,
			BMP280Filter filter,
			BMP280StandbyDuration standbyDuration
		) {
			// t_sb = standbyDuration
			// filter = filter
			// spi3w_en = 0
			writeUint8(BMP280Register::Config, ((uint8_t) standbyDuration << 5) | ((uint8_t) filter << 2) | (uint8_t) 0);

			// osrs_t = temperatureOversampling
			// osrs_p = pressureOversampling
			// mode = mode
			writeUint8(BMP280Register::Control, ((uint8_t) temperatureOversampling << 5) | ((uint8_t) pressureOversampling << 2) | (uint8_t) mode);

			delay(100);
		}

		void readCalibrationData() {
			_calibrationDigT1 = readUint16LE(BMP280Register::DigT1);
			_calibrationDigT2 = readInt16LE(BMP280Register::DigT2);
			_calibrationDigT3 = readInt16LE(BMP280Register::DigT3);

			_calibrationDigP1 = readUint16LE(BMP280Register::DigP1);
			_calibrationDigP2 = readInt16LE(BMP280Register::DigP2);
			_calibrationDigP3 = readInt16LE(BMP280Register::DigP3);
			_calibrationDigP4 = readInt16LE(BMP280Register::DigP4);
			_calibrationDigP5 = readInt16LE(BMP280Register::DigP5);
			_calibrationDigP6 = readInt16LE(BMP280Register::DigP6);
			_calibrationDigP7 = readInt16LE(BMP280Register::DigP7);
			_calibrationDigP8 = readInt16LE(BMP280Register::DigP8);
			_calibrationDigP9 = readInt16LE(BMP280Register::DigP9);
		}

		// These bitchy compensation formulas has been taken from datasheet
		// Don't see any reason to touch them))0
		// See https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf
		float readTemperature() {
			int32_t adc_T = readInt24BE(BMP280Register::TemperatureData);
			// Seems like this shit expects only last 20 bits from 24
			adc_T >>= 4;

			float var1, var2, T;
			var1 = (((float)adc_T)/16384.0f - ((float)_calibrationDigT1)/1024.0f) * ((float)_calibrationDigT2);
			var2 = ((((float)adc_T)/131072.0f - ((float)_calibrationDigT1)/8192.0f) *
				(((float)adc_T)/131072.0f - ((float) _calibrationDigT1)/8192.0f)) * ((float)_calibrationDigT3);
			_tFine = (int32_t)(var1 + var2);
			T = (var1 + var2) / 5120.0f;

			return T;
		}

		float readPressure() {
			// To process raw pressure data, we need previously read temperature
			if (_tFine == -0xFFFF)
				readTemperature();

			int32_t adc_P = readInt24BE(BMP280Register::PressureData);
			adc_P >>= 4;

			float var1, var2, p;
			var1 = ((float)_tFine / 2.0f) - 64000.0f;
			var2 = var1 * var1 * ((float)_calibrationDigP6) / 32768.0f;
			var2 = var2 + var1 * ((float)_calibrationDigP5) * 2.0f;
			var2 = (var2/4.0f)+(((float)_calibrationDigP4) * 65536.0f);
			var1 = (((float)_calibrationDigP3) * var1 * var1 / 524288.0f + ((float)_calibrationDigP2) * var1) / 524288.0f;
			var1 = (1.0f + var1 / 32768.0f)*((float)_calibrationDigP1);
			if (var1 == 0.0f)
			{
				return 0; // avoid exception caused by division by zero
			}
			p = 1048576.0f - (float) adc_P;
			p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
			var1 = ((float)_calibrationDigP9) * p * p / 2147483648.0f;
			var2 = p * ((float)_calibrationDigP8) / 32768.0f;
			p = p + (var1 + var2 + ((float)_calibrationDigP7)) / 16.0f;

			return p;
		}

	private:
		uint8_t _csPin;
		const SPISettings _SPISettings = SPISettings(1000000, SPI_MSBFIRST, SPI_MODE0);

		// Calibration data
		uint16_t _calibrationDigT1 = 0;
		int16_t _calibrationDigT2 = 0;
		int16_t _calibrationDigT3 = 0;

		uint16_t _calibrationDigP1 = 0;
		int16_t _calibrationDigP2 = 0;
		int16_t _calibrationDigP3 = 0;
		int16_t _calibrationDigP4 = 0;
		int16_t _calibrationDigP5 = 0;
		int16_t _calibrationDigP6 = 0;
		int16_t _calibrationDigP7 = 0;
		int16_t _calibrationDigP8 = 0;
		int16_t _calibrationDigP9 = 0;

		// From datasheet:
		// The variable t_fine (signed 32 bit) carries a fine resolution temperature value over to the
		// pressure compensation formula and could be implemented as a global variable
		int32_t _tFine = -0xFFFF;

		void setChipSelect(uint8_t value) const {
			digitalWrite(_csPin, value);
		}

		void writeUint8(BMP280Register reg, uint8_t value) {
			SPI.beginTransaction(_SPISettings);
			setChipSelect(LOW);

			// Writing
			uint8_t buffer[2] {
				(uint8_t) (uint8_t(reg) & ~0x80),
				value
			};

			SPI.transferBytes(buffer, nullptr, 2);

			setChipSelect(HIGH);
			SPI.endTransaction();
		}

		void writeAndRead(uint8_t* buffer, BMP280Register reg, uint32_t readSize) {
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

		uint16_t readUint16LE(BMP280Register reg) {
			uint8_t buffer[2];
			writeAndRead(buffer, reg, 2);

			return ((uint16_t) buffer[1] << 8) | (uint16_t) buffer[0];
		}

		int16_t readInt16LE(BMP280Register reg) {
			return (int16_t) readUint16LE(reg);
		}

		uint32_t readUInt24BE(BMP280Register reg) {
			uint8_t buffer[3];
			writeAndRead(buffer, reg, 3);

			return ((uint32_t) buffer[0] << 16) | ((uint32_t) buffer[1] << 8) | (uint32_t) buffer[2];
		}

		int32_t readInt24BE(BMP280Register reg) {
			return (int32_t) readUInt24BE(reg);
		}
};