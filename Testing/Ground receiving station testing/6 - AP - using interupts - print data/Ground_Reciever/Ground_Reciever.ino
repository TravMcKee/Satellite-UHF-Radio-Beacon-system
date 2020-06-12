/*
 * Ground_Receiver station - Original clock counting technique
 * 
 * This program is used for the initial development of the ground receiving station. The program will
 * initialise the radio and GPS units when power is applied and wait for a LoRa signal to be received. 
 * When a valid signal is received (header and packet data CRC checks c/o successfully by the radios
 * in-built software) then a timestamp is taken by counting clock cycles, which is saved with the msg data.
 * The first received LoRa signal resets the clock count so that it begins at 0 for the time measurements
 * Once the telemtry data is received (timestamp and data saved) then the system will wait for the next
 * GPS PPS signal to arrive in which a GPS timestamp will be taken, and the current GPS data is read from
 * the GPS unit. The positional data (latitude/longitude/altitude) and time data (UTC time) is saved. The
 * program will then save the time stamp of the next 3 PPS pulses and use this to determine the average
 * number of clock cycles in one second which will get the instantaneous frequency of the oscillator. All 
 * the saved data (messages, timestamps, freq drift data and GPS data) is then displayed on the serial monitor.
 * The data can also be sent through the serial connection to a computer for processing the data
 *  
 *  
 * Author:  Travis McKee 
 * Date:    9 June 2020
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2 //receive the external interrupt LoRa RXDone/TXDone signal on pin 3
#define PPS_pin 3 //receive the external interrupt PPS signal on pin 3

// Set the Transmit/Receive frequency to 437MHz
#define RF95_FREQ 437.0
 
// Single instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// set up the GPS receiver connections, RXD(pin 5) & TXD(pin 6)
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
  Serial.println("** Initialisation begins **");
  
  // set the data rate for the SoftwareSerial port for the GPS module
  mySerial.begin(9600);

  //sets up the Timer1 to be used as clock cycle counter will resolution 1/f = 62.5nSec
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0 to reset the Timer1 settings
  TCCR1B = 0;// same for TCCR1B
  TCNT1 = 0; //initialise counter value to 0
  OCR1A = 65535; //sets the top value for when the TImer 1 counter resets and the overflowCount increments by 1 
  TCCR1B |= (1 << WGM12); //turn on CTC mode 
  TCCR1B |= (1 << CS10); //prescaler set to 1 (62.5nS resolution)
  TIMSK1 |= (1 << OCIE1A); //sets up the compare ISR for timer1 counter reaches the OCR1A value
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
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(15, false);

  //Defaults after init are 437.0MHz, 15dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //Changing the default modem settings to one of the RadioHead default settings
  while(!rf95.setModemConfig(2))
  {
    Serial.println("LoRa radio modem settings change failed");
  }
  //Setting 0 (Bw125Cr45Sf128) - Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
  //Setting 1 (Bw500Cr45Sf128) - Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
  //Setting 2 (Bw31_25Cr48Sf512) - Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
  //Setting 3 (Bw125Cr48Sf4096) - Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range.

  Serial.println("** Initialisation complete **");
}
 
void loop()
{
  // This waits until an external interrupt is received on the Uno 2 pin from the DIO0 pin on the LoRa module
  // The signal that arrives is the LoRa RXDone signal that tells the processor there is a MSG waiting that
  // has passed the preamble and data payload integrity checks
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      if (count == 0) //reset the clock counter variables if this is the first received data packet
      {
          overflowCount = 0; //reset clock overflow counter
          TCNT1 = 0; //initialise counter value to 0
      }
      loRaTimeStamp[count] = ((overflowCount*65535)+TCNT1); //Time stamp the radio packet that has arrived
        if (len == 5) //if the radio packet is an identification packet
        {
          for (int i=0;i<len;i++)
          {identData[count][i] = buf[i];} // save the RX'ed data into the identification data array
          count++;
        }
        else //if the radio packet is a telemetry packet
        {
          for (int i=0;i<len;i++)
          {telemetryData[i] = buf[i];} // save the RX'ed into the telemetry data array
          telemetryRX = true; //used to tell the processor the radio packets have been RX'ed and to collect GPS data
        }
      

      //look at GPS data after receiving  all the radio packets - will get the current UTC to the second
      if (telemetryRX)
      {
        Serial.println("**** ALL MESSAGES NOW RECEIVED ****");
        Serial.println();

        //The next command turns on the external interrupt on pin 3 which is the PPS signal from the GPS Module
        noInterrupts();
        attachInterrupt(digitalPinToInterrupt(3), gpsTimeStamp, RISING);
        EIFR |= 3; // clear ext intrupt flag
        interrupts();
          
        while(count1<1) {}; //tells the processor to wait for the next PPS signal, may be a better way for this
          
          unsigned long start = millis();
          bool newdata = false;
      
          //this reads in the GPS data - may need to consider if the data is not ready (not enough GPS satellites locked)
          while (millis() - start < 5000) //look to see if this can be done better, i could not bypass it
          {
            if (mySerial.available()) // If there is a connection with the GPS module
            {
              char c = mySerial.read(); // read all the current data from the GPS module. 
                                        // It has the current UTC from the last PPS Pulse
              //Serial.print(c);  // uncomment to see raw GPS data
              if (gps.encode(c)) //feeds all the NEMA data from the GPS module to the gps object in Arduino
              {
                newdata = true; //tells the processor the GPS data is ready to collect
                break;  // uncomment to print new data immediately!
              }
            }
          }
        if (newdata) {gpsdump(gps);} //call a function that collects the required GNSS information from the GPS object

        while(count1!=4){}; //This lets the processor wait until it receives its fourth GNSS PPS timestamp
        detachInterrupt(digitalPinToInterrupt(3));

        oscFreq =(GPSTimeStamp[3]-GPSTimeStamp[0])/3; //calculates the current frequency of the Unos oscillator

        //print the time stamp and msg data for each message and the GPS data saved in each variable
        printData();

        //reset all variables ready for the next lot of RX'ed radio packets
        count = 0;
        count1 = 0;
        telemetryRX= false;
        }
      
    }
  }
}

ISR(TIMER1_COMPA_vect) //if using RadioHead library, the RH_ASK.h and RH_ASH.cpp need to be deleted so that this works
{
overflowCount++; //perform interupt when timer1 hits 65535 (top of register)
}

void gpsTimeStamp()
{
  if (count1 < 4)
  {
  GPSTimeStamp[count1] = ((overflowCount * 65535) + TCNT1); //save the PPS timestamp into an array
  count1++;
  }
}


void gpsdump(TinyGPS &gps) // collects the data required by the ground station from the GPS object with the latest GNSS data
{
  gps.get_position(&lat, &lon, &posAge);
  gps.get_datetime(&date, &time, &timeAge);
  alt = gps.altitude(); //takes a while to obtain after startup, the GPS module must be run for a while for reliable Altitude
  //gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &crackTimeAge);
}


void printData() //this prints all the data on the Arduino serial monitor
{
  for (int i=0; i<4; i++)
  {
    Serial.print("identification msg number "); Serial.print(i+1); Serial.print(" timestamp is: "); Serial.println(loRaTimeStamp[i]);
    Serial.print("and the data in the msg was: ");
      for(int j=0;j<5; j++)
      { Serial.print(identData[i][j],HEX); Serial.print(" ");} 
      Serial.println(); 
  }Serial.println();
  
  //print the time srtamp and msg data for the telemtry message
  Serial.print("the timestamp for the telemetry msg is: "); Serial.println(loRaTimeStamp[4]);
  Serial.print("and the data in the msg was: ");
    for(int j=0;j<50; j++)
    { Serial.print(telemetryData[j],HEX); Serial.print(" ");} 
    Serial.println(); Serial.println();
  
  Serial.print("the GPS Time stamps are: ");
  for(int i=0;i<4; i++)
  {Serial.print(GPSTimeStamp[i]);; Serial.print(" , ");}
  Serial.println(); Serial.println();

  Serial.print("The current frequency of the oscillator is "); Serial.println(oscFreq);
  Serial.println();

  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
  Serial.print(" Fix age: "); Serial.print(posAge); Serial.println("ms.");

  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");
  Serial.print(time);
  Serial.print(" Fix age: "); Serial.print(timeAge); Serial.println("ms.");
  Serial.print("Alt(cm): "); Serial.println(alt); Serial.println();
}
