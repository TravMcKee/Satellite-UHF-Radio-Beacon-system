// Trav's Sat Beacon

#include <SPI.h>
#include <RH_RF95.h>

// for Arduino Pro Mini
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 3
#define LED 13

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(LED, OUTPUT); //For turning on the LED
  Serial.begin(9600);
  delay(1);

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
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  delay(3000); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("Transmitting..."); // Send a message to rf95_server
  char radiopacket[250] = "aaaaaaaaaabbbbbbbbbbccccccccccddddddddddeeeeeeeeeeffffffffffgggggggggghhhhhhhhhhiiiiiiiiiijjjjjjjjjjkkkkkkkkkkllllllllllmmmmmmmmmmnnnnnnnnnnooooooooooppppppppppqqqqqqqqqqrrrrrrrrrrssssssssssttttttttttuuuuuuuuuuvvvvvvvvvvwwwwwwwwwwxxxxxxxxxxyyyyyyyyyy";
  delay(1000);
  
    for (int j=250;j>0;j=j-5)
    {
    radiopacket[j-1]='\0';
    Serial.print("Sending "); Serial.print(j-1); Serial.println(" Bytes");    
    Serial.println(radiopacket);
    delay(10);
    rf95.send((uint8_t *)radiopacket, j);
    delay(10);
    rf95.waitPacketSent();
    delay(1000);
    }
  delay(3000);

  delay(100000);
}
