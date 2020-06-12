/*
 * Ground_Reciever - Ground Station timing - Shortened Cycles
 * 
 * This program is designed to test if the ground station can receive a LoRa signal and a GPS signal
 * and then immediately printing the results to the serial monitor (no data is saved)
 *  
 *  
 * Author:  Travis McKee 
 * Date:    21 Feb 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
 
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

// set up the GPS receiver, RXD(pin 5) & TXD(pin 6)
SoftwareSerial mySerial(5, 6);
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);
unsigned long timeStamp = 0;
unsigned long loRaTimeStamp[5];
unsigned long GPSTimeStamp = 0;
int count = 0;
 
void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
 
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
 
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
  rf95.setTxPower(5, false);

  //Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //Changing the default modem settings to one of the preset settings
  while(!rf95.setModemConfig(3))
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
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
 
    if (rf95.recv(buf, &len))
    {
      timeStamp = micros();
      loRaTimeStamp[count] = timeStamp;
      count++;
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
            for(int i = 0; i < len; i++)
      {
        Serial.print((uint8_t)buf[i]); 
        Serial.print(", ");
      }
      Serial.println();
      Serial.print("number of Bytes is: ");
      Serial.println(len);

      //look at GPS data after recieving message
      if (count==5)
      {
        bool newdata = false;
        unsigned long start = millis();
        // Every 5 seconds we print an update
        while (millis() - start < 5000) 
        {
          if (mySerial.available()) 
          {
            GPSTimeStamp = micros();
            char c = mySerial.read();
            //Serial.print(c);  // uncomment to see raw GPS data
            if (gps.encode(c)) 
            {
              newdata = true;
              break;  // uncomment to print new data immediately!
            }
          }
        }
        Serial.println();
        if (newdata) 
        {
          Serial.println("Acquired Data");
          Serial.println("-------------");
          gpsdump(gps);
          Serial.println("-------------");
          Serial.println();
        }

        
        //print the time stamp data
        for (int i= 0; i<5; i++)
        {
          Serial.println(loRaTimeStamp[i]);
        }
        Serial.print("GPS Time stamp: ");
        Serial.println(GPSTimeStamp);
        count = 0;
      }
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  gps.get_position(&lat, &lon, &age);
  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");
  
  // On Arduino, GPS characters may be lost during lengthy Serial.print()
  // On Teensy, Serial prints to USB, which has large output buffering and
  //   runs very fast, so it's not necessary to worry about missing 4800
  //   baud GPS characters.

  gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): "); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
    Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");
    Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); 
    Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour));  Serial.print(":");
    Serial.print(static_cast<int>(minute)); Serial.print(":"); Serial.print(static_cast<int>(second));
    Serial.print("."); Serial.print(static_cast<int>(hundredths)); Serial.print(" UTC +00:00 ");
  Serial.print("  Fix age: ");  Serial.print(age); Serial.println("ms.");

  Serial.print("Alt(cm): "); Serial.println(gps.altitude()); 
  
  /*Serial.print(" Course(10^-2 deg): ");
    Serial.print(gps.course()); Serial.print(" Speed(10^-2 knots): "); Serial.println(gps.speed());
  Serial.print("Alt(float): "); printFloat(gps.f_altitude()); Serial.print(" Course(float): ");
    printFloat(gps.f_course()); Serial.println();
  Serial.print("Speed(knots): "); printFloat(gps.f_speed_knots()); Serial.print(" (mph): ");
    printFloat(gps.f_speed_mph());
  Serial.print(" (mps): "); printFloat(gps.f_speed_mps()); Serial.print(" (kmph): ");
    printFloat(gps.f_speed_kmph()); Serial.println();

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: ");
    Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);
    */
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}
