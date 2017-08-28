
@Mabele Leonard
@RFID and RN2483 chip on LoRa network
//28/08/2017
//importing the necessary libraries for SPI on MFRC522 RFID readTemperature
//import TheThingsNetwork library
//import SoftwareSerial
#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include <TheThingsNetwork.h>
#include <TheThingsMessage.h>
#include <SoftwareSerial.h>


//Using OTA on LoRa
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
devicedata_t data = api_DeviceData_init_default;

void setup() {
	loraSerial.begin(57600);      //lora baud rate
	debugSerial.begin(9600);      //arduino baud rate

	// Wait a maximum of 10s for Serial Monitor
	while (!debugSerial && millis() < 10000);

	debugSerial.println("-- STATUS");
	ttn.showStatus();

	debugSerial.println("-- JOIN");       //joining TheThingsNetwork platform with key
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

	Serial.print("UID size : ");
	Serial.println(mfrc522.uid.size);

 Serial.print("Printing HEX UID : ");
 for (byte i = 0; i < mfrc522.uid.size; i++) {
	Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
	Serial.print(mfrc522.uid.uidByte[i], HEX);
 }
 debugSerial.println("");

	unsigned long UID_unsigned;
	UID_unsigned =  mfrc522.uid.uidByte[0] << 24;
	UID_unsigned += mfrc522.uid.uidByte[1] << 16;
	UID_unsigned += mfrc522.uid.uidByte[2] <<  8;
	UID_unsigned += mfrc522.uid.uidByte[3];


	  debugSerial.println();
	  debugSerial.println("UID Unsigned int");   //printing the unsigned value
	  debugSerial.println(UID_unsigned);


	byte payload[4];
	payload[0] = highByte(UID_unsigned);
  payload[1] = lowByte(UID_unsigned);





	ttn.sendBytes(payload, sizeof(payload));    //sending the bytes to TheThingsNetwork

  delay(2000);

}
