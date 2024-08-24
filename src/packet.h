#pragma once

#ifndef PLANE_PACKETCONTAINER_H
#define PLANE_PACKETCONTAINER_H
#endif

enum PacketType : uint8_t {
	AHRS = 5
};

#pragma pack(push, 1)
template<typename T>
struct Packet {
	explicit Packet(uint32_t header, PacketType type, T body) :
		header(header),
		type(type),
		body(body)
	{

	}

	uint32_t header;
	PacketType type;
	T body;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct AHRSPacket {
	float pitch;
	float roll;
	float yaw;

	float temperature;
	float pressure;

	float qnh;
	float altitude;

	void print() const {
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

		Serial.print("Altitude: ");
		Serial.println(altitude);
	}
};
#pragma pack(pop)