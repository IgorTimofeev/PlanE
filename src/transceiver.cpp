#include "transceiver.h"
#include "aircraft.h"

#define RF_FREQUENCY                                915000000 // Hz  center frequency
#define TX_OUTPUT_POWER                             22        // dBm tx output power
#define LORA_BANDWIDTH                              4         // bandwidth
// 2: 31.25Khz
// 3: 62.5Khz
// 4: 125Khz
// 5: 250KHZ
// 6: 500Khz
#define LORA_SPREADING_FACTOR                       7         // spreading factor [SF5..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
//  2: 4/6,
//  3: 4/7,
//  4: 4/8]

#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_PAYLOADLENGTH                          0         // 0: Variable length packet (explicit header)
// 1..255  Fixed length packet (implicit header)

void Transceiver::begin() {
	Serial.println("[SX1262] Initializing");

	auto state = _radio.begin(RF_FREQUENCY, TX_OUTPUT_POWER);

	if (state == ERR_NONE) {

	}
	else {
		Serial.print("[SX1262] Failure, code: ");
		Serial.println(state);

		while (true)
			delay(100);
	}

	_radio.LoRaConfig(
		LORA_SPREADING_FACTOR,
		LORA_BANDWIDTH,
		LORA_CODINGRATE,
		LORA_PREAMBLE_LENGTH,
		LORA_PAYLOADLENGTH,
		true,               //crcOn
		false);             //invertIrq
}

void Transceiver::tick(Aircraft &aircraft) {
	auto& ahrs = aircraft.getAHRS();

	GovnoPacket packet = GovnoPacket {
		.signature = 0x506C416E,

		.pitch = ahrs.getPitch(),
		.roll = ahrs.getRoll(),
		.yaw = ahrs.getYaw(),

		.temperature = ahrs.getTemperature(),
		.pressure = ahrs.getPressure(),

		.qnh = ahrs.getQnh(),
		.altitude = ahrs.getAltitude()
	};

	Serial.println("[SX1262] Sending packet");

	if (_radio.Send((uint8_t*) &packet, sizeof(GovnoPacket), SX126x_TXMODE_SYNC)) {
		// the packet was successfully transmitted
		Serial.println("[SX1262] success!");
	} else {
		// some other error occurred
		Serial.print("[SX1262] failed, code ");
	}

}