/*
 * Ground_Reciever - Shortened Satellite Beacon Cycle
 * 
 * This program is designed to receive the data sent from the Satellite radio beacon, It provides
 * the serial monitor with the number of Bytes received as well as the Hexadeciamal and equivalent
 * integer representation of the data in each radio packet
 *  
 *  
 * Author:  Travis McKee 
 * Date:    20 Feb 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
 
/*
// for use with the Adafruit Feather32u4 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
*/
// for use with the Arduino Pro mini on the red PCB or Breadboard (Space Beacon) and the Arduino Uno (ground recieving station)
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2

// Set the Transmit/Recieve frequency
#define RF95_FREQ 437.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
 
// Blinky on receipt
#define LED 13
 
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
 
  Serial.println("Feather LoRa RX Test!");
 
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
 

  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);

  //Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //Changing the default modem settings to one of the preset settings
  while(!rf95.setModemConfig(0))
  {
    Serial.println("LoRa radio modem settings change failed");
  }
  //Setting 0 (Bw125Cr45Sf128) - Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
  //Setting 1 (Bw500Cr45Sf128) - Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
  //Setting 2 (Bw31_25Cr48Sf512 ) - Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
  //Setting 3 (Bw125Cr48Sf4096) - Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range.
}
 
void loop()
{

  if (rf95.available())
  {
      Serial.println ("checking");
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
 
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      
      //int RxMSG[20] = (int *)buf;
      for(int i = 0; i < len; i++)
      {
        Serial.print((uint8_t)buf[i]); 
        Serial.print(", ");
      }
      Serial.println();
      Serial.print("number of Bytes is: ");
      Serial.println(len);

    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}
