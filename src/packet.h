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
	bool strobeLights;

	float pitch;
	float roll;
	float yaw;

	float temperature;
	float pressure;

	void print() const {
		Serial.print("throttle: ");
		Serial.println(throttle);

		Serial.print("ailerons: ");
		Serial.println(ailerons);

		Serial.print("rudder: ");
		Serial.println(rudder);

		Serial.print("strobeLights: ");
		Serial.println(strobeLights);

		Serial.print("Pitch: ");
		Serial.println(pitch);

		Serial.print("Roll: ");
		Serial.println(roll);

		Serial.print("Yaw: ");
		Serial.println(yaw);

		Serial.print("Temp: ");
		Serial.println(temperature);

		Serial.print("Pressure: ");
		Serial.println(pressure);
	}
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ControllerCommandPacket {
	uint8_t throttle;
	uint8_t ailerons;
	uint8_t rudder;
	boolean strobeLights;

	void print() const {
		Serial.print("throttle: ");
		Serial.println(throttle);

		Serial.print("ailerons: ");
		Serial.println(ailerons);

		Serial.print("rudder: ");
		Serial.println(rudder);

		Serial.print("strobeLights: ");
		Serial.println(strobeLights);
	}
};
#pragma pack(pop)