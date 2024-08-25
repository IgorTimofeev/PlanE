#include <Arduino.h>
#include "aircraft.h"

Aircraft aircraft;


// IV => should be a real random value
#define AES_IV      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F

char plaintext[16];
char encrypted[16];

uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
uint8_t iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
esp_aes_context ctx;


void encodetest()
{

	uint8_t copyIv[16];
	memcpy(copyIv, iv, sizeof(iv));

	memset( plaintext, 0, sizeof( plaintext ) );
	strcpy( plaintext, "Hello, ESP32!" );


	esp_aes_crypt_cbc( &ctx, ESP_AES_ENCRYPT, sizeof(plaintext), copyIv, (uint8_t*)plaintext, (uint8_t*)encrypted );

	//See encrypted payload, and wipe out plaintext.
	memset( plaintext, 0, sizeof( plaintext ) );
	int i;
	for( i = 0; i < 16; i++ )
	{
		Serial.printf( "%02x[%c]%c", encrypted[i], (encrypted[i]>31)?encrypted[i]:' ', ((i&0xf)!=0xf)?' ':'\n' );
	}
	Serial.printf( "\n" );

	//Use the ESP32 to decrypt the CBC block.
	memcpy(copyIv, iv, sizeof(iv));
	esp_aes_crypt_cbc( &ctx, ESP_AES_DECRYPT, sizeof(encrypted), copyIv, (uint8_t*)encrypted, (uint8_t*)plaintext );

	//Verify output
	for( i = 0; i < 16; i++ )
	{
		Serial.printf( "%02x[%c]%c", plaintext[i],  (plaintext[i]>31)?plaintext[i]:' ', ((i&0xf)!=0xf)?' ':'\n' );
	}
	Serial.printf( "\n" );
}

void setup() {
	Serial.begin(115200);

	esp_aes_init( &ctx );
	esp_aes_setkey( &ctx, key, 128 );

//	aircraft.begin();
}

void loop() {
//	aircraft.tick();
//
//	delay(1000);

	delay(1000);
	encodetest();
}