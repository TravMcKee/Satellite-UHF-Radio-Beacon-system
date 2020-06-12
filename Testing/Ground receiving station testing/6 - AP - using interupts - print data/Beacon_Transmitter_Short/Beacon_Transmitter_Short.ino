/*
 * Beacon_Transmitter - Ground Station testing - Shortened Cycles
 * 
 * This program is designed to carry out the Shortened software cycle for the Satellite Beacon for Ground
 * testing purposes. The software cycle comprises of five phases...
 * 
 * 1) Initialisation/Launch - At launch the radio and microcontroller intitialises once power is applied and the both are 
 * put to sleep for 5 seconds (for testing purposes), with the shutdown after launch time being 30-45 minutes for 
 * actual space operations
 * 2) Data collection - The microcontroller does a series of commands such as check voltage, poll light sensing diodes etc.
 * to simulate the collection of satellite telemetry data.
 * 3) Data Transmission - 5 packets of data are transmitted. The first four packets contain the satellite address (packet number,23,AF,23,AF)
 * and the fifth packet containing 50 Bytes of random data representing the satellite telemetry data.
 * 4) Receive mode - The satellite radio enters receive mode for 5 seconds where it could accept a command
 * 5) Low power idle mode - The satellite enters a low power mode for 2 seconds where the radio is turned off and the
 * microcontroller is set to idle
 * 
 * Author:  Travis McKee 
 * Date:    09 Jun 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <LowPower.h>
#include <EEPROM.h>

// for use with the Arduino Pro mini on the red PCB or Breadboard (Space Beacon) and the Arduino Uno (ground receiving station)
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2 //This is the interrupt for the TXDone/RXDone signal from the LoRa module
#define RX_TIME 5000 //This sets how long the receive phase is for the beacon cycle

// Set the Transmit/Receive frequency
#define RF95_FREQ 437.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int satBatVoltage = 0;
int solarPanelVoltage = 0;
int photoDiode1 = 0;
int photoDiode2 = 0;
int photoDiode3 = 0;

char radiopacket[50];
uint8_t satTelemetry[50]; 
uint8_t satAddress[5] = {0,35,175,35,175};
boolean launched = false;

void setup()
{
  Serial.begin(115200);

  Serial.println("Initialisation Begins");
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  EEPROM.write(0,0); //This uses the EEPROM to store if the satellite has just been launched (0) or if it is in orbit (1)

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

    if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(15, false);

  //Defaults after init are 437.0MHz, 15dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  //Changing the default modem settings to one of the RadioHead default settings
  while(!rf95.setModemConfig(2))
  {
    Serial.println("LoRa radio modem settings change failed");
  }
  //Setting 0 (Bw125Cr45Sf128) - Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range
  //Setting 1 (Bw500Cr45Sf128) - Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range.
  //Setting 2 (Bw31_25Cr48Sf512 ) - Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range.
  //Setting 3 (Bw125Cr48Sf4096) - Bw = 125 kHz, Cr = 4/8, Sf = 4096chips/symbol, CRC on. Slow+long range.
  
  Serial.println("Initialisation finished");
}


void loop()
{
  if (EEPROM.read(0) == 0)// This only occurs when the satellite first launches from the satellite
  {
    Serial.println("satellite launches");
    launch(); //shuts down microcontroller for 4 seconds to represent 45 minutes after launch
    EEPROM.update(0,1); //updates that the satellite has been launched only after the launch cycle is complete
    Serial.begin(115200);
    delay(100);
    satTelemetry[0] = 1;
    for (int i = 1; i< sizeof(satTelemetry); i++) //fills the telemetry packet with random data for testing purposes
    {
      satTelemetry[i] = random(255);
    }
  }
  Serial.begin(115200);
  delay(50);

  Serial.println("Begin collecting data");
  delay(10);
  collectData(); //collecting data - battery checks, check light sensing diode, current check
  Serial.println("Finish collecting data");

  Serial.println("begin Transmit");
  delay(10);
  transmit(); //transmit 4 identification packets followed by the telemetry packet
  Serial.println("Transmission complete");


  Serial.println("enter receive cycle");
  delay(10);
  receive(); //places the LoRa into recevve mode so it can receive a control command
  Serial.println("finish receive cycle");
  
  delay(10);
  Serial.println("enter idle cycle");
  delay(10);
  idleBeacon(); //enters a variable lenght low power mode to conserve electrical power
  Serial.println("finish idle cycle");

}

void launch() //This function is for when the satellite 1st launches
{ //if this cycle completes then the EEPROM variable is set to true and it will not be called again if power is disconnected
  rf95.sleep(); //places the radio into sleep mode to prevent radio transmissions
  Serial.flush();
  delay(100);
  for (int i = 0; i < 5; i++)
  {
    LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_OFF);
    // the first part "SLEEP_1S" can also be changed to SLEEP_2S, SLEEP_4S or SLEEP_8S for 2, 4 or 8 seconds
  }
  delay(100);
  rf95.setModeIdle(); // returns the radio to idle mode ready for the next cycle which is data collection and transmit
  delay(100);
}

void transmit()
{  
  for (int i=1; i<5; i++)
  {
  Serial.print("Sending Ident "); Serial.println(i);
  satAddress[0]=i; //this is the unique identifier for each identification radio packet, sequential count from 1 to 4
  rf95.send((uint8_t *)satAddress, 5); //send 5 Bytes of data (8 bit unsigned integer)
  delay(10);
  rf95.waitPacketSent(); //waits for the transmission to complete
  delay(100);
  }
  
  delay(100);
  Serial.println("Sending Telemetry "); 
  rf95.send((uint8_t *)satTelemetry, 50);
  delay(10);
  rf95.waitPacketSent();
  satTelemetry[0] = satTelemetry[0]+1; //Increases the first byte of the telemetry packet by 1 for testing purposes 
  delay(100);
}

void receive()
{
  unsigned long timeStamp = millis();
  delay(10);
  do
  {
    if (rf95.available()) //this acts on the interrupt on pin 2 (DIO0 from the LoRa module)
    {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len)) //this is where the beacon collects data and functions can be written dependant on RX'ed command
      {
        Serial.print("Got: ");
        Serial.println((char*)buf);
        Serial.println("Doing something");
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
  } while (millis() < timeStamp + RX_TIME); //while the elapsed time is less than the RX_TIME
}

void idleBeacon()
{
  rf95.sleep();
  delay(100);
  for (int i = 0; i < 2; i++) //powers down beacon for 2 seconds
  {
    //these are the other options available for the power down phase of the beacon software cycle
    //LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_OFF);
    LowPower.idle(SLEEP_1S, ADC_OFF,TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
    //LowPower.powerStandby(SLEEP_1S, ADC_OFF, BOD_OFF);

  }
  delay(100);
  rf95.setModeIdle();
}

void collectData()
{
  // The following are a series of commands that were used during the VIVA presentation and were
  // kept in the code so that something happens during the collect data phase and the current
  // consumption can be measured and approximated for normal beacon operation. This phase can be
  // filled with functions that collect data from other satellite systems.
  digitalWrite(2,HIGH); //power for light sensing diodes
  delay(1000);
  
  int batRead=0;
  for(int i = 0;i<50;i++)
  {
  batRead = batRead + analogRead(A3);
  }
  satBatVoltage = batRead/50;
  Serial.print("Battery voltage is: "); Serial.println(satBatVoltage);
  
  solarPanelVoltage =  solarPanelVoltage+1;
  if ( solarPanelVoltage == 100)
  {
     solarPanelVoltage = 0;
  }
  Serial.print("Solar Panel Voltage: ");
  Serial.println(solarPanelVoltage);

  int diode1=0;
  int diode2=0;
  int diode3=0;
  for (int j=0;j<50;j++)
  {
  diode1 = diode1 + analogRead(A0);
  diode2 = diode2 + analogRead(A1);
  diode3 = diode3 + analogRead(A2);
  }
  photoDiode1 = diode1/50;
  photoDiode2 = diode2/50;
  photoDiode3 = diode3/50;

    Serial.print("Diode 1: ");
  Serial.print(photoDiode1);
    Serial.print(", Diode 2: ");
  Serial.print(photoDiode2);
    Serial.print(", Diode 3: ");
  Serial.println(photoDiode3);
  digitalWrite(2,LOW); 
  delay(500);
}
