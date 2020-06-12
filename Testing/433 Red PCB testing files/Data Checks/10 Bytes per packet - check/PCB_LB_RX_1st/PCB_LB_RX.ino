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
  rf95.setCodingRate4(5);
  //rf95.setPayloadCRC(false); //turns the CRC correction off so that it must recieve the packet regardless of errors
}

int packets = 100;
int Bytes = 10;
unsigned long startTime;
unsigned long currentTime;
unsigned long timeBreak=0;
int checks = 1;
 
void loop()
{
  int count = 0;
  int origData[packets][10];
  int sentData[packets][10];
  
  //random array generator removed from here

  while (timeBreak < 1500 && count<100){
    if (rf95.available())
    {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
      
      if (rf95.recv(buf, &len))
      {
        //This prints the butter directly using the LoRa RFM95 library
        //RH_RF95::printBuffer("Received: ", buf, len);
        
        //transfers the data from the radio buffer to the sentData array
        for (int l=0;l<len;l++)
        { 
          sentData[count][l] = buf[l];      
        }
        /*
        //Prints the data that has been transferred from the Buffer to the sentData
        for (int i=0;i<len;i++)
        { 
          Serial.print(sentData[count][i],HEX);
          Serial.print(" ");   
        }
        Serial.println();
        /*
        //Prints the original data that was sent
        for (int j=0;j<len;j++)
        {
          Serial.print(origData[count][j],HEX);
          Serial.print(" ");
        }
        Serial.println();
        */
        count++;
        startTime = millis();
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
  Serial.println(" packets of data received");
  /*
//Prints the data that has been transferred from the Buffer to the sentData
for (int j=0;j<count;j++)
{
  for (int i=0;i<Bytes;i++)
  { 
    Serial.print(sentData[j][i],HEX);
    Serial.print(" ");   
  }
  Serial.println();
}
*/
/*
  if (count == 100) {}
  //{ BitCheck(origData,sentData); }
  else 
  { Serial.print("The radio has dropped one or more packets"); }
  //{ BitSortandCheck(origData,sentData); }
*/
  count = 0;
  timeBreak = 0;
  checks++;     
}
/*
void BitCheck(int origData[][10], int sentData[][10])
{

  for(int i=0;i<packets;i++) //creates the random array to compare the input against for errors
  {   
    for(int j=0;j<Bytes;j++)
      { origData[i][j] = random(0,127); }
  }    
  
  for (int x=0;x<packets;x++)
  {
    for (int y=0;y<Bytes;y++)
    {
      
      if (origData[x][y] != sentData[x][y])
      {
        Serial.print("Bit error present in Batch "); Serial.print(x+1);
        Serial.print(", Byte "); Serial.print(y+1);
        Serial.print(" - data sent: "); Serial.print(sentData[x][y],HEX); Serial.print(" & ");
        Serial.print("Original data: "); Serial.println(origData[x][y],HEX);
      }
    }
  }
  /*
  for (int i=0;i<packets;i++)
  {
    for (int j=0;j<Bytes;j++)
    {
      Serial.print(origData[i][j],HEX); 
      Serial.print(" ");
    }
    Serial.println();
    for (int k=0;k<Bytes;k++)
    {
      Serial.print(sentData[i][k],HEX); 
      Serial.print(" ");
    }
    Serial.println(); Serial.println();
  }
  
}

*/
/*
void BitSortandCheck(int origData[], int sentData[])
{
  int sft = 0;
  for (int i=0;i<leng-sft;i++)
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
    for (int j=0;j<leng;j++)
  {
    Serial.print(origData[j],HEX); Serial.print(" ");
  }
  Serial.println();
  for (int j=0;j<leng;j++)
  {
    Serial.print(sentData[j],HEX); Serial.print(" ");
  }
  Serial.println();
}
*/
