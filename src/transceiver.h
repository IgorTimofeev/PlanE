#pragma once

#ifndef PLANE_TRANSCEIVER_H
#define PLANE_TRANSCEIVER_H
#endif

#include "Arduino.h"
#include <RadioLib.h>
#include <aes/esp_aes.h>
#include "settings.h"
#include "packet.h"

class Aircraft;

enum TransceiverMode : uint8_t {
	Idle,
	Transmit,
	Receive
};

class Transceiver {
	public:
		void begin();
		void tick(Aircraft& aircraft);

	private:
		SX1262 _sx1262 = new Module(
			settings::pinout::transceiver::chipSelect,
			settings::pinout::transceiver::dio0,
			settings::pinout::transceiver::reset,
			settings::pinout::transceiver::busy
		);

		uint8_t _sx1262Buffer[256] {};

		TransceiverMode _mode = TransceiverMode::Idle;
		static volatile bool _canOperate;
		ICACHE_RAM_ATTR static void setFlag(void);
		uint32_t _tickDeadline = 0;

		const uint8_t _AESKey[16] = { 0x02, 0xEB, 0x46, 0x45, 0x96, 0xB0, 0xD6, 0xB9, 0x7C, 0x34, 0xBE, 0x77, 0x75, 0xF2, 0xBE, 0x1B };
		const uint8_t _AESIV[16] = { 0x17, 0x5D, 0x25, 0x2A, 0xFD, 0x72, 0x1E, 0x01, 0x02, 0x60, 0x88, 0x92, 0x9A, 0x9B, 0x2A, 0xA9 };
		uint8_t _AESIVCopy[sizeof(_AESIV)] {};
		uint8_t _AESBuffer[sizeof(_sx1262Buffer)] {};

		template<typename T>
		void transmit(PacketType packetType, const T& packet);
		void receive(Aircraft &aircraft);
		void parsePacket(Aircraft &aircraft, uint8_t* bufferPtr);
};