/*
 * This is setup to recieve the data from the beacon using a adafruit feather 32u4 with a RFM96 radio (437MHz)
 */
 
#include <SPI.h>
#include <RH_RF95.h>
/*
// for Feather32u4 RFM9x
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
*/
// for Arduino Pro Mini on the red PCB board
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2
 
// Change to 434.0 or other frequency, must match RX's freq!
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
 
  Serial.begin(9600);
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
 
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(5, false);
  rf95.setCodingRate4(4);
  //rf95.setPayloadCRC(false); //turns the CRC correction off so that it must recieve the packet regardless of errors
  //rf95.CrcAutoClearOff = false; //need to look into this one to set it to off
}

int len = 100;
unsigned long startTime;
unsigned long currentTime;
unsigned long timeBreak=0;
boolean origCheck = false;
int checks = 1;
int totalGood = 0;
int totalBad = 0;
 
void loop()
{
/*
  if (origCheck == false)
  {
    for(int i=0;i<len;i++) //creates the random array to compare the input against for errors
    { origData[i] = random(0,127); }
    origCheck = true;
  }
*/
  int count = 0;
  int origData[len];
  int sentData[len];
  //for(int i=0;i<len;i++) //creates the random array to compare the input against for errors
  //{ origData[i] = random(0,127); }
  
  while (timeBreak < 1500 && count<len){
    if (rf95.available())
    {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
   
      if (rf95.recv(buf, &len))
      {
        //RH_RF95::printBuffer("Received: ", buf, len);
        sentData[count] = buf[0];
        count++;
        startTime = millis();
        //Serial.print("count is "); Serial.println(count);
      }
    }
    if(count>0) 
    {
      timeBreak= millis()-startTime;
    }
  }
  
  Serial.print("check ");
  Serial.print(checks);
  Serial.print(" ->"); 
  Serial.print(count);
  Serial.println(" Bytes of data recieved");

  //Returns the count of the number of good received packets
  Serial.print(rf95.rxGood()-totalGood); Serial.print(" good packets recieved and "); 
  totalGood = totalGood + rf95.rxGood();
  //Returns the count of the number of bad received packets (ie packets with bad lengths, checksum etc) which were rejected and not delivered to the application.
  //Caution: not all drivers can correctly report this count. Some underlying hardware only report good packets
  Serial.print(rf95.rxBad()-totalBad); Serial.println(" bad packets recieved");
  totalBad = totalBad + rf95.rxBad();
  
  
  /*
  Serial.println("data sent is: ");
  for(int j; j<len; j++)
  {
    Serial.print(sentData[j],HEX); Serial.print(" ");
  }
  Serial.println();
    
  Serial.println("Orignal data is: ");
  for(int j; j<len; j++)
  {
    Serial.print(origData[j],HEX); Serial.print(" ");
  }
  Serial.println();
  Serial.println();

  */
  count = 0;
  timeBreak = 0;
  checks++;     
}
