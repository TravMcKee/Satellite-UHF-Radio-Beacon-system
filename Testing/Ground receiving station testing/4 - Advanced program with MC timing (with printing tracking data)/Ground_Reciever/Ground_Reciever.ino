/*
 * Ground_Reciever - Ground Station timing - Shortened Cycles
 * 
 * This program is used for the initial development of the ground receiving station. The program will
 * initialise the radio and GPS units when power is applied and then wait a LoRa signal to be received. 
 * When a valid signal is received (header and packet data CRC checks c/o successfully by the radios
 * in-built software) then a timestamp is taken by counting clock cycles which is saved with the msg data. 
 * Once the telemtry data is received (timestamp and data saved) then the system will wait for the next
 * GPS PPS signal to arrive in which a GPS timestamp will be taken and the current GPS data is read from
 * the GPS unit. The positional data (latitude/longitude/altitude) and time data (UTC time) is saved.
 * All the saved data (messages, timestamps and gps data) is then displayed on the serial monitor.
 *  
 *  
 * Author:  Travis McKee 
 * Date:    6 Apr 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>


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
long lat, lon, alt;
unsigned long posAge, timeAge,crackTimeAge, date, time, chars;
int year;
byte month, day, hour, minute, second, hundredths;
unsigned short sentences, failed;
volatile unsigned long pulseCount;
 
void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  pinMode(PPS_pin, INPUT);
  digitalWrite(RFM95_RST, HIGH);

  //Sets up serial 0 to print on the serial monitor
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
  
  // set the data rate for the SoftwareSerial port for GPS module
  mySerial.begin(9600);

  //sets up the Timer1 to be used as clock cycle counter will resolution 1/f = 62.5nSec
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1 = 0; //initialise counter value to 0
  OCR1A = 65535; //sets the top value for when the counter resets and pulseCount increments by 1 
  TCCR1B |= (1<< WGM12); //turn on CTC mode 
  TCCR1B |= (1<<CS10); //prescaler set to 1 (62.5nS resolution)
  TIMSK1 |= (1 << OCIE1A); //sets up the interupt for timer1
  interrupts();
 
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
  //Setting 2 (Bw31_25Cr48Sf512) - Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
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
      if (count == 0) //reset the clock counter variables when the first LoRa message is received
      {
          pulseCount = 0; //reset clock overflow counter
          TCNT1 = 0; //initialise counter value to 0
      }
      loRaTimeStamp[count] = ((pulseCount*65535)+TCNT1); //Time stamp the MSG that has arrived
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
          GPSTimeStamp = ((pulseCount*65535)+TCNT1);

          while (millis() - start < 5000) 
          {
            if (mySerial.available()) 
            {
              char c = mySerial.read();
              //Serial.print(c);  // uncomment to see raw GPS data
              if (gps.encode(c)) 
              {
                newdata = true;
                break;  // uncomment to print new data immediately!
              }
            }
          }
        if (newdata) {gpsdump(gps);}

        //print the time stamp and msg data for each message and the GPS data saved in each variable
        printData();
        count = 0;
        telemetryRX= false;
        }
      
    }
  }
}

ISR(TIMER1_COMPA_vect) //if using radiohead library, the RH_ASK.h and RH_ASH.cpp need to be deleted so that this time can be used
{
pulseCount++; //perform interupt when timer1 hits 65535 (top of register)
}


void gpsdump(TinyGPS &gps)
{
  gps.get_position(&lat, &lon, &posAge);
  gps.get_datetime(&date, &time, &timeAge);
  alt = gps.altitude(); //this takes a while to obtain correctly after startup - maybe need to look at this
  //gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &crackTimeAge);
}


void printData() //need to turn this into more usable form
{
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
