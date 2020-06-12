/*
 * Ground_Reciever - Ground Station timing - Shortened Cycles
 * 
 * This program is used for the initial development of the ground receiving station. The program will
 * initialise the radio and GPS units when power is applied and then wait a LoRa signal to be received. 
 * When a valid signal is received (header and packet data CRC checks c/o successfully by the radios
 * in-built software) then a timestamp is taken using micros() which is saved with the msg data. 
 * Once the telemtry data is received (timestamp and data saved) then the system will wait for the next
 * GPS PPS signal to arrive in which a GPS timestamp will be taken and the current GPS data is read from
 * the GPS unit. The positional data (latitude/longitude/altitude) and time data (UTC time) is sent through
 * serial to be read by MATLAB
 *  
 *  
 * Author:  Travis McKee 
 * Date:    28 Feb 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
//#include <time.h>
 
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
#define PPS_pin 3

// Set the Transmit/Recieve frequency
#define RF95_FREQ 437.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// set up the GPS receiver, RXD(pin 5) & TXD(pin 6)
SoftwareSerial mySerial(5, 6);
TinyGPS gps;

//void gpsdump(TinyGPS &gps);
unsigned long timeStamp = 0;
unsigned long loRaTimeStamp[5];
unsigned long GPSTimeStamp = 0;
int count = 0;
int identData[4][4],telemetryData[50];
boolean telemetryRX = false;
//need to clear up what GPS data i want to record/send
long lat, lon, alt;
unsigned long posAge, timeAge,crackTimeAge, date, time, chars; //chars may not be used
int year;
byte month, day, hour, minute, second, hundredths;
unsigned short sentences, failed; //both these may not be used
 
void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  pinMode(PPS_pin, INPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  Serial.begin(115200);
  //while (!Serial) {
  //  delay(1);
  //}
  delay(100);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
 
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
  rf95.setTxPower(5, false);

  //Changing the default modem settings to one of the preset settings
  //Setting 0 (Bw125Cr45Sf128) - Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
  //Setting 1 (Bw500Cr45Sf128) - Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
  //Setting 2 (Bw31_25Cr48Sf512) - Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
  //Setting 3 (Bw125Cr48Sf4096) - Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range.
  while(!rf95.setModemConfig(3))
  {
    Serial.println("LoRa radio modem settings change failed");
  }

}
 
void loop()
{
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    //need to find a way to stop waiting for a signal if a packet has been dropped and too much time has elapsed (dependant on time between transmits).
    if (rf95.recv(buf, &len))
    {
      timeStamp = micros();
      loRaTimeStamp[count] = timeStamp;
        if (len == 4)
        {
          for (int i=0;i<len;i++)
          {identData[count][i] = buf[i];}
          count++;
        }
        else
        {
          for (int i=0;i<len;i++)
          {telemetryData[i] = buf[i];}
          telemetryRX = true;
        }
      

      //look at GPS data after receiving message
      if (telemetryRX)
      {
        Serial.println("ALL MESSAGES NOW RECEIVED");
        bool newdata = false;
        unsigned long start = millis();
          //this makes the program wait for the next time the PPS signal goes high
          while (digitalRead(PPS_pin)==0){};
          GPSTimeStamp = micros();
            //After the GPS time stamp is marked, read the current GPS data which has the UTC time of the last PPS pulse
            //There nay be a better way to do this
            while (millis() - start < 5000) 
            {
              if (mySerial.available()) 
              {
                char c = mySerial.read();
                if (gps.encode(c)) 
                {
                  newdata = true;
                  break;
                }
              }
            }
          if (newdata) {gpsdump(gps);}

        //Need to included mesuring the timing of several PPS pulses using the arduino MC to calculate the oscillator drift

        //prints all the serial data as char/strings to be collected by MATLAB
        printData();
        //sends all the serial data as the format [data type, message]
        //sendData();
        count = 0;
        telemetryRX= false;
        }
      
    }
  }
}

void gpsdump(TinyGPS &gps)
{
  gps.get_position(&lat, &lon, &posAge);
  gps.get_datetime(&date, &time, &timeAge);
  //gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &crackTimeAge);
  alt = gps.altitude();
}

void printData(){
  for (int i=0; i<4; i++)
  {
    Serial.print("identification msg number "); Serial.print(i+1); Serial.print(" timestamp is: "); Serial.println(loRaTimeStamp[i]);
    Serial.print("We need to take "); Serial.print(GPSTimeStamp-loRaTimeStamp[i]); Serial.println(" microseconds from the UTC time");
    Serial.print("and the data in the msg was: ");
      for(int j=0;j<4; j++)
      { Serial.print(identData[i][j],HEX); Serial.print(" ");
      } Serial.println();
  }
  
  //print the time srtamp and msg data for the telemtry message
  Serial.print("the timestamp for the telemetry msg is: "); Serial.println(loRaTimeStamp[4]);
  Serial.print("We need to take "); Serial.print(GPSTimeStamp-loRaTimeStamp[4]); Serial.println(" microseconds from the UTC time");
  Serial.print("and the data in the msg was: ");
    for(int j=0;j<50; j++)
    { Serial.print(telemetryData[j],HEX); Serial.print(" ");
    } Serial.println(); Serial.println();
  
  Serial.print("the GPS Time stamp: ");
  Serial.println(GPSTimeStamp);

  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
  Serial.print(" Fix age: "); Serial.print(posAge); Serial.println("ms.");

  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");
  Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(timeAge); Serial.println("ms.");
  /*if you want to use date and time all broken up
  Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); 
  Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour));  Serial.print(":");
  Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
  Serial.print("."); Serial.print(static_cast<int>(hundredths)); Serial.print(" UTC +00:00 ");
  Serial.print("  Fix age: ");  Serial.print(crackTimeAge); Serial.println("ms.");
  */
  Serial.print("Alt(cm): "); Serial.println(alt); Serial.println();
}

void sendData() {
  //needs more refinement
  for(int i=0;i<4; i++)
  {
    for(int j=0;j<4; j++)
    {
      Serial.write((int)identData[i][j]);
    }
  }
  for(int j=0;j<50; j++)
  {
  Serial.write((int)telemetryData[j]);
  }
  //Serial.write();
}
