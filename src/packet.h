#pragma once

#ifndef PLANE_PACKETCONTAINER_H
#define PLANE_PACKETCONTAINER_H
#endif

#include "cstdint"
#include "settings.h"

enum PacketType : uint8_t {
	AircraftAHRS = 1,
	ControllerCommand = 2
};

enum AltimeterMode : uint8_t  {
	QNH,
	STD
};

#pragma pack(push, 1)
template<typename T>
struct PacketTypeWrapper {
	explicit PacketTypeWrapper(PacketType type, T body) :
		type(type),
		body(body)
	{

	}

	PacketType type;
	T body;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct AircraftAHRSPacket {
	uint8_t throttle;
	uint8_t ailerons;
	uint8_t rudder;
	uint8_t flaps;

	float pitch;
	float roll;
	float yaw;

	float temperature;
	float pressure;

	AltimeterMode altimeterMode;
	float altimeterPressure;

	float altitude;
	float speed;

	bool strobeLights;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ControllerCommandPacket {
	uint8_t throttle;
	uint8_t ailerons;
	uint8_t rudder;
	uint8_t flaps;

	AltimeterMode altimeterMode;
	float altimeterPressure;

	boolean strobeLights;

	void print() const {
		Serial.printf("[AHRSPacket] Throttle, ailerons, rudder, flaps: %d, %d, %d, %d\n", throttle, ailerons, rudder, flaps);
		Serial.printf("[AHRSPacket] Altimeter mode, pressure: %d, %f\n", altimeterMode, altimeterPressure);
		Serial.printf("[AHRSPacket] Strobe lights: %d\n", strobeLights);
	}
};
#pragma pack(pop)