// rf69_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_client
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration.
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

// Craig Benson 2019-07-16
// Teensy and Adafruit RFM69HCW Radio Breakout board
// Note that RESET (active HIGH) is tied HIGH by breakout board, need to set LOW
/* Teensy 3.2 Pin    Adafruit RFM69HCW Pin
 *  GND                GND
 *  2                  G0                 
 *  10                 CS
 *  11                 MOSI
 *  12                 MISO
 *  13                 SCK
 *  16                 RST
 *  3V3                Vin  
 *  */

#include <SPI.h>
#include <RH_RF69.h>

#define IRQIN 2
#define RFRESET 16 
#define MAXREG 48

// Singleton instance of the radio driver
//RH_RF69 rf69;
//RH_RF69 rf69(15, 16); // For RF69 on PJRC breakout board with Teensy 3.1
//RH_RF69 rf69(4, 2); // For MoteinoMEGA https://lowpowerlab.com/shop/moteinomega
RH_RF69 rf69(10, IRQIN); // For RF69 on ADAFRUIT breakout board with Teensy 3.1

void setup() 
{
  Serial.begin(9600);
  Serial.println("Hello, about to init");
  delay(100);
  pinMode(RFRESET,OUTPUT);
  delay(100);
  digitalWrite(RFRESET,LOW);
  Serial.println("set RFRESET LOW");
  delay(100);

  if (!rf69.init()) {
    Serial.println("init failed");
  } else {
    Serial.println("init success");
  }
  delay(100);
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  // No encryption
  // No encryption
  if (!rf69.setFrequency(433.0)) {
    Serial.println("setFrequency failed");
  } else {
    Serial.println("setFrequency passed");
  }
  // If you are using a high power RF69, you *must* set a Tx power in the
  // range 14 to 20 like this:
  // rf69.setTxPower(14);

for (int i=0;i<=MAXREG;i++) {
  Serial.print("Register 0x");
  Serial.print(i,HEX);
  Serial.print(" = 0x");
  Serial.println(rf69.spiRead(i));
  delay(50);
}

 // The encryption key has to be the same as the one in the client
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
#if 0
  // For compat with RFM69 Struct_send
  rf69.setModemConfig(RH_RF69::GFSK_Rb250Fd250);
  rf69.setPreambleLength(3);
  uint8_t syncwords[] = { 0x2d, 0x64 };
  rf69.setSyncWords(syncwords, sizeof(syncwords));
  rf69.setEncryptionKey((uint8_t*)"thisIsEncryptKey");
#endif
}

void loop()
{
 // Serial.print("Current RSSI = ");
 // Serial.println(rf69.rssiRead());
  
  if (rf69.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len))
    {
//      RH_RF69::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rf69.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      rf69.send(data, sizeof(data));
      rf69.waitPacketSent();
      Serial.println("Sent a reply");
      Serial.print("Last RSSI was ");
      Serial.println(rf69.lastRssi());
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  //delay(100);
}

