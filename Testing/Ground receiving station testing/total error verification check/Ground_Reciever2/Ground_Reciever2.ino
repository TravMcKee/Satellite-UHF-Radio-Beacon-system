/*
 * Ground_Reciever - Ground Station final veificaion - total timing error testing
 * 
 * This program is used for the initial development of the ground receiving station. The program will
 * initialise the radio and GPS units when power is applied and wait for a LoRa signal to be received. 
 * When a valid signal is received (header and packet data CRC checks c/o successfully by the radios
 * in-built software) then a timestamp is taken by counting clock cycles, which is saved with the msg data. 
 * Once the telemtry data is received (timestamp and data saved) then the system will wait for the next
 * GPS PPS signal to arrive in which a GPS timestamp will be taken and the current GPS data is read from
 * the GPS unit. The positional data (latitude/longitude/altitude) and time data (UTC time) is saved. The
 * program will then save the time stamp of the next 3 PPS pulses and use this to determine the average
 * number of clockcycles in one second which will get the instantaneous frequency of the oscillator. The
 * data [lora time stamps, gps time stamps, date, time] are displayed on the serial
 * 
 *  
 *  
 * Author:  Travis McKee 
 * Date:    15 May 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

// for use with the Arduino Uno (ground recieving station)
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2
#define PPS_pin 3 //receive the PPS signal on pin 3

// Set the Transmit/Recieve frequency
#define RF95_FREQ 437.0
 
// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// set up the GPS receiver, RXD(pin 5) & TXD(pin 6)
SoftwareSerial mySerial(5, 6);
TinyGPS gps;

unsigned long loRaTimeStamp[5], oscFreq;
int identData[4][5],telemetryData[50];
long lat, lon, alt;
unsigned long posAge, timeAge, date, time;
volatile unsigned long overflowCount;
boolean telemetryRX = false;
int count = 0;
volatile int count1 = 0;
volatile unsigned long GPSTimeStamp[4];
 
void setup()
{
  //Sets up serial 0 to print on the serial monitor
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);
  Serial.println("** 2 Initialisation begins **");
  
  // set the data rate for the SoftwareSerial port for GPS module
  mySerial.begin(9600);

  //sets up the Timer1 to be used as clock cycle counter will resolution 1/f = 62.5nSec
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1 = 0; //initialise counter value to 0
  OCR1A = 65535; //sets the top value for when the counter resets and overflowCount increments by 1 
  TCCR1B |= (1<< WGM12); //turn on CTC mode 
  TCCR1B |= (1<<CS10); //prescaler set to 1 (62.5nS resolution)
  TIMSK1 |= (1 << OCIE1A); //sets up the interupt for timer1
  interrupts();

  pinMode(RFM95_RST, OUTPUT);
  pinMode(PPS_pin, INPUT);
  digitalWrite(RFM95_RST, HIGH);
 
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
  //Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  //Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(15, false);

  //Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //Changing the default modem settings to one of the preset settings
  while(!rf95.setModemConfig(2))
  {
    Serial.println("LoRa radio modem settings change failed");
  }
  //Setting 0 (Bw125Cr45Sf128) - Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
  //Setting 1 (Bw500Cr45Sf128) - Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
  //Setting 2 (Bw31_25Cr48Sf512) - Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
  //Setting 3 (Bw125Cr48Sf4096) - Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range.

  //Serial.println("** Initialisation complete **");
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
          overflowCount = 0; //reset clock overflow counter
          TCNT1 = 0; //initialise counter value to 0
      }
      loRaTimeStamp[count] = ((overflowCount * 65535) + TCNT1); //Time stamp the MSG that has arrived
        if (len == 5)
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
      

      //look at GPS data after receiving message - will get most current UTC to the second
      if (telemetryRX)
      {
        //Serial.println("ALL MESSAGES NOW RECEIVED");
        noInterrupts();
        attachInterrupt(digitalPinToInterrupt(3), gpsTimeStamp, RISING);
        EIFR |= 3; // clear ext intrupt flag
        interrupts();
          
          while(count1<1) {}; //captures the first GPS pulse, it is doing it way too soon
          
          unsigned long start = millis();
          bool newdata = false;
      
          //this reads in the GPS data - need to account for if the data is not ready (not enough GPS sat locked)
          while (millis() - start < 5000) //look to see if this can be done better
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

        while(count1!=4){};
        
        detachInterrupt(digitalPinToInterrupt(3));

        oscFreq =(GPSTimeStamp[3]-GPSTimeStamp[0])/3;

        //this is used to send the data o MATLAB through the serial connection
        sendData();

        //reset variables
        count = 0;
        count1 = 0;
        telemetryRX= false;
        }
      
    }
  }
}

ISR(TIMER1_COMPA_vect) //if using radiohead library, the RH_ASK.h and RH_ASH.cpp need to be deleted so that this time can be used
{
overflowCount++; //perform interupt when timer1 hits 65535 (top of register)
}

void gpsTimeStamp()
{
  if (count1 < 4)
  {
  GPSTimeStamp[count1] = ((overflowCount * 65535) + TCNT1);
  count1++;
  }
}


void gpsdump(TinyGPS &gps)
{
  gps.get_position(&lat, &lon, &posAge);
  gps.get_datetime(&date, &time, &timeAge);
  alt = gps.altitude(); //this takes a while to obtain correctly after startup - maybe need to look at this
  //gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &crackTimeAge);
}


void sendData() {
  //needs more refinement
  for(int i=0;i<5; i++)
  {
    Serial.print(loRaTimeStamp[i]); Serial.print(',');
  }
  
  for(int j=0;j<4; j++)
  {
  Serial.print(GPSTimeStamp[j]); Serial.print(',');
  }
  Serial.print(oscFreq); Serial.print(',');
  Serial.print(date); Serial.print(',');
  Serial.println(time);
}
