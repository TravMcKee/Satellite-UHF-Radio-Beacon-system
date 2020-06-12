/*
 * This file is to test the data transmission and reception of the beacon
 * 
 * It randomally sends one BYTE at a time which is a hexadecimal character using the rfm96 radio on 437MHz.
 * THe power level is set to 5dBm (the lowest setting)
 * 
 */

#include <SPI.h>
#include <RH_RF95.h>

// for Arduino Pro Mini on the red PCB board
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2
#define RF95_FREQ 437.00

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(13, OUTPUT); //For turning on the LED

  Serial.begin(9600);
  while (!Serial);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(5, false);
  rf95.setCodingRate4(5);
  //rf95.setPayloadCRC(false); //turns the CRC correction off so that it must rx the packet regardless of errors
}

int numOfPackets = 10;
int BytesPerPackets = 10;
int batchSize = 1000;
uint8_t msg[11];

void loop()
{
  delay(500);
    for (int batch=1; batch<batchSize+1; batch++)
    {
      for (int i=0; i<numOfPackets;i++) 
      {
        //Serial.print("Sending Packet "); Serial.print(i+1); Serial.print(": ");
        for(int k=0; k<BytesPerPackets;k++)
        { 
          msg[k] = random(0,127);
          //Serial.print(msg[k],HEX); Serial.print(" ");
        }
        //Serial.println();

        msg[BytesPerPackets] = 0;
        rf95.send(msg, BytesPerPackets);
        rf95.waitPacketSent();
        delay(100);
      }

     Serial.print("sent batch: ");
     Serial.println(batch);
    delay(2000);
    }
  delay(10000000);
}
