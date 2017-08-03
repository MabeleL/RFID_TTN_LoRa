
#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>

const char *appEui = "70B3D57EF000656C";
const char *appKey = "0B9ACAF122545E01E5242FA0467B9A8C";

//initialize the LoRa modeule - RN2483 on SoftwareSerial
SoftwareSerial loraSerial = SoftwareSerial(8,7);
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup() {
	loraSerial.begin(57600);
	debugSerial.begin(9600);

	// Wait a maximum of 10s for Serial Monitor
	while (!debugSerial && millis() < 10000);

	debugSerial.println("-- STATUS");
	ttn.showStatus();

	debugSerial.println("-- JOIN");
	ttn.join(appEui, appKey);

	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();	// Init MFRC522 card
	Serial.println("Scan PICC to see UID and type...");
}

void loop() {

	debugSerial.println("-- LOOP");
	// Look for new cards
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card. PICC_HaltA() is automatically called.
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

	unsigned long UID_unsigned;
	UID_unsigned =  mfrc522.uid.uidByte[0] << 24;
	UID_unsigned += mfrc522.uid.uidByte[1] << 16;
	UID_unsigned += mfrc522.uid.uidByte[2] <<  8;
	UID_unsigned += mfrc522.uid.uidByte[3];

	long UID_LONG=(long)UID_unsigned;

	Serial.println("UID Long :");
  Serial.println(UID_LONG);

	byte payload[4];
	payload[0] = highByte(UID_LONG);
  payload[1] = lowByte(UID_LONG);

	ttn.sendBytes(payload, sizeof(payload));

  delay(2000);

}
