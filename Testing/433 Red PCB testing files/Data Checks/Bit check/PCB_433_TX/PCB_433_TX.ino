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

  Serial.begin(115200);
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
}

int numOfBytes = 100;
int msg[2];

void loop()
{
  delay(500);
    for (int j=1; j<101; j++)
    {
      for (int i=0; i<numOfBytes;i++) 
      {
        msg[0] = random(0,127);
        //Serial.print("Sending "); Serial.println(msg[0],HEX);
        msg[1] = 0;
        rf95.send((uint8_t *)msg, 1);
        rf95.waitPacketSent();
        delay(1000);
      }
     Serial.print("sent batch: ");
     Serial.print(j);
     Serial.print(", number of packets sent: ");
     Serial.println(rf95.txGood()- ((j-1)*100) ); //Returns the count of the number of packets successfully transmitted (though not necessarily received by the destination)

    delay(5000);
    }
  delay(10000000);
}
