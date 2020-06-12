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
  while (!Serial) { delay(1); }
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
 
  rf95.setTxPower(5, false);
  rf95.setCodingRate4(5);
  //rf95.setPayloadCRC(false); //turns the CRC correction off so that it must recieve the packet regardless of errors
}

int packets = 10;
int bytes = 10;
unsigned long startTime;
unsigned long currentTime;
unsigned long timeBreak=0;
int checks = 1;
int sentData[10][10];
int totalDropped = 0;
 
void loop()
{

  int count = 0;
  
  while (timeBreak < 1000 && count<packets){
    if (rf95.available())
    {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
   
      if (rf95.recv(buf, &len))
      {
        //RH_RF95::printBuffer("Received: ", buf, len);
        for (int i=0;i<len;i++)
        {
        sentData[count][i] = buf[i];
        }
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
  Serial.print(" packets of data received, ");
  
  if (count == packets)
  { BitCheck(sentData); }
  else 
  { BitSortandCheck(sentData); }

  totalDropped = totalDropped + (packets-count);
  Serial.print("Total packets dropped: "); Serial.println(totalDropped);
  
  count = 0;
  timeBreak = 0;
  checks++;     
}

void BitCheck(int sentData[][10])
{

  int origData[10][10];
  for (int i=0;i<packets;i++)
  {
    for(int k=0;k<bytes;k++) //creates the random array to compare the input against for errors
    { origData[i][k] = random(0,127); }
  }
  
  for (int i=0;i<packets;i++)
  {  
    for (int j=0;j<bytes;j++)
    {
      if (origData[i][j] != sentData[i][j])
      {
        Serial.print("Bit error present in packet "); Serial.print(i+1); Serial.print(" , Byte "); Serial.print(j+1);        
        Serial.print(" - data sent: "); Serial.print(sentData[i][j],HEX); Serial.print(" & ");
        Serial.print("Original data: "); Serial.println(origData[i][j],HEX);
      }
    }
  }
 /* 
  for (int i;i<packets;i++)
  {
    for (int j=0;j<bytes;j++)
    {
      Serial.print(origData[i][j],HEX); Serial.print(" ");
    }
    Serial.println();
    for (int k=0;k<bytes;k++)
    {
      Serial.print(sentData[i][k],HEX); Serial.print(" ");
    }
  Serial.println();
  }
  */
  //Serial.println("Bit check complete");
}

void BitSortandCheck(int sentData[][10])
{
  int origData[10][10];
  for (int i=0;i<packets;i++)
  {
    for(int k=0;k<bytes;k++) //creates the random array to compare the input against for errors
    { origData[i][k] = random(0,127); }
  }
/*
  int sft = 0;
  for (int i=0;i<len-sft;i++)
  {
    if (origData[i+sft] != sentData[i])
    {
      if (origData[i+sft+1] == sentData[i])
      {
      Serial.print("Bit error present in Byte number "); Serial.print(i+sft+1);
      Serial.print(" - data sent: "); Serial.print("not rx'ed & ");
      Serial.print("Original data: "); Serial.println(origData[i+sft],HEX);
      sft = sft+1;
      }
      else if (origData[i+sft+2] == sentData[i])
      {
      Serial.print("Bit error present in Byte number "); Serial.print(i+sft+1);
      Serial.print(" - data sent: "); Serial.print("not rx'ed & ");
      Serial.print("Original data: "); Serial.println(origData[i+sft],HEX);
      
      Serial.print("Bit error present in Byte number "); Serial.print(i+sft+2);
      Serial.print(" - data sent: "); Serial.print("not rx'ed & ");
      Serial.print("Original data: "); Serial.println(origData[i+sft+1],HEX);
      sft = sft+2;
      }
      else
      {
      Serial.print("Bit error present in Byte number "); Serial.print(i+sft+1);
      Serial.print(" - data sent: "); Serial.print(sentData[i],HEX); Serial.print(" & ");
      Serial.print("Original data: "); Serial.println(origData[i+sft],HEX);
      }
    }
  }
    for (int j=0;j<len;j++)
  {
    Serial.print(origData[j],HEX); Serial.print(" ");
  }
  Serial.println();
  for (int j=0;j<len;j++)
  {
    Serial.print(sentData[j],HEX); Serial.print(" ");
  }
  Serial.println();
  */
}
