/*
Program that uses the 32u4 feather as a ground station to contact a feather space station
author: Travis McKee
Date: 24 Aug 2019
*/

#include <SPI.h>
#include <RH_RF95.h>

// for feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

char input = '0';

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(13, OUTPUT); //For turning on the LED

  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }

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
  Serial.println("######################################################################");
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}


void loop()
{

  while (Serial.available() > 0)   
  { 
    input = Serial.parseInt();   
    if (Serial.read() == '\n')
    {
          switch (input) 
          {
            case 1:  //Interograte
            {
              Serial.println("you want to interogratge");
              transmit("1111");
              input = '0';
            }
              break;
            case 2:  //Identify
            {
              Serial.println("you want to identify");
              transmit("2222");
              input = '0';
            }
              break;
              case 3:  //Battery check
            {
              Serial.println("you want to check battery");
              transmit("3333");
              input = '0';
            }
              break;
              case 4:  //Flash light
            {
              Serial.println("you want make sat flash");
              transmit("4444");
              input = '0';
            }
              break;
              case 5:  //Does nothing
            {
              Serial.println("Check command fail");
              transmit("5555");
              input = '0';
            }
              break;
            default:
            {
              Serial.println("Invalid selection");
              input = '0';
            }
              // if nothing else matches, do the default
              break;
          }
          Serial.println();
     }
   }
}

void transmit(char msg[]){
  delay(1000); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("Transmitting..."); // Send a message to rf95_server
  digitalWrite(13, HIGH); //turn on the LED
  uint8_t radiopacket[4];
 
  strcpy(radiopacket,msg);
  Serial.print("Sending "); Serial.println((char*)radiopacket);
  
  rf95.send(radiopacket, sizeof(radiopacket)+1);
  rf95.waitPacketSent();
  
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  digitalWrite(13, LOW);
  
  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(5000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      memset(buf,0,len); 
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
}
