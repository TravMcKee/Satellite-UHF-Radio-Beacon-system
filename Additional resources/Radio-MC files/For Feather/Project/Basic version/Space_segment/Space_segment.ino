// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX
 
#include <SPI.h>
#include <RH_RF95.h>
 
// for Feather32u4 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
 
// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0
 
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
  Serial.println("######################################################################");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
 
void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf); 
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      Serial.print("Recieved: ");
      Serial.println((char*)buf);
      uint8_t msg[10];
        if((char*)buf[0]=='1'&&(char*)buf[1]=='1'&&(char*)buf[2]=='1'&&(char*)buf[3]=='1')
          {
            strcpy(msg,"Buzzzzzzzz");
          }
          else if((char*)buf[0]=='2'&&(char*)buf[1]=='2'&&(char*)buf[2]=='2'&&(char*)buf[3]=='2')
          {
            strcpy(msg,"Sat-TRV81");
          }
          else if((char*)buf[0]=='3'&&(char*)buf[1]=='3'&&(char*)buf[2]=='3'&&(char*)buf[3]=='3')
          {
            strcpy(msg,"3.2V");
          }
          else if((char*)buf[0]=='4'&&(char*)buf[1]=='4'&&(char*)buf[2]=='4'&&(char*)buf[3]=='4')
          {
            strcpy(msg,"Flashing");
            flash();
          }
          else
          {
            strcpy(msg,"cmd fail");
          }
          delay(1000);
      rf95.send(msg, sizeof(msg)+1);
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      Serial.println();
      digitalWrite(LED, LOW);
      memset(msg,0,sizeof(msg));
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

void flash()
{
  for (int i=0; i<5; i++)
  {
    digitalWrite(LED, HIGH);
    delay(300);
    digitalWrite(LED,LOW);
    delay(300);
  }
}
