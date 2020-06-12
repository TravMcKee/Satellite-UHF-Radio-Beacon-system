/*
 * Beacon_Transmitter - Ground Station testing - Full beacon cycle
 * 
 * This program is designed to carry out the full software cycle for the Satellite Beacon for Ground
 * testing purposes. The software cycle comprises of five phase...
 * 
 * 1) Initialisation/Launch - At launch the radio and microcontroller intitialises once power is applied and the 
 * both are put to sleep with the shutdown after launch time being for 30 minutes for launch requirements
 * 2) Data collection - The microcontroller does collect any telemetry data but uses 50 Bytes of random data
 * 3) Data Transmission - 5 radio packets are transmitted. The first 4 packets contain the satellite identification
 * (packet number,23,AF,23,AF) and the fifth packet containing 50 Bytes of random telemetry data 
 * 4) Receive mode - The satellite radio enters receive mode for 60 seconds where it can accept a control command
 * 5) Low power idle mode - The satellite enters a low power mode for between 4-6 minutes where the radio is 
 * turned off and the microcontroller is set to idle
 * 
 * Author:  Travis McKee 
 * Date:    9 June 2020
 * 
 */
 
#include <SPI.h>
#include <RH_RF95.h>
#include <LowPower.h>
#include <EEPROM.h>

#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 2
#define RX_TIME 60000

// Set the Transmit/Receive frequency to 437MHz
#define RF95_FREQ 437.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

char radiopacket[50];
uint8_t satTelemetry[50]; 
uint8_t satAddress[5] = {0,35,175,35,175}; //unique satellite identification - Hexadecimal = 0x23AF
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

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
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
  if (EEPROM.read(0) == 0) //carried out the following after release from launch vehicle
  {
    Serial.println("satellite launches");
    launch(); //shuts down radio and processor for 30 minutes
    EEPROM.update(0,1); //updates EEPROM variable only after launch function is complete
    Serial.begin(115200);
    delay(100);
    satTelemetry[0] = 1;
    for (int i = 1; i< sizeof(satTelemetry); i++) //random data for the telemetry radio packet
    {
      satTelemetry[i] = random(255);
    }
  }
  Serial.begin(115200);
  delay(50);

  Serial.println("Begin collecting data");
  collectData(); 
  Serial.println("Finish collecting data");
  delay(10);

  Serial.println("begin Transmit");
  transmit();
  Serial.println("Transmission complete");
  delay(10);

  
  Serial.println("enter receive cycle");
  receive();
  Serial.println("finish receive cycle");
  delay(10);
  
  Serial.println("enter idle cycle");
  idleBeacon();
  Serial.println("finish idle cycle");
  delay(10);
}

void launch()
{
  rf95.sleep(); // puts radio into sleep mode
  Serial.flush();
  delay(100);
  for (int i = 0; i < 1; i++) // puts processor into power save mode for 30 minutes
  {
    LowPower.powerSave(SLEEP_8S, ADC_OFF, BOD_OFF, TIMER2_OFF);
  }
  delay(100);
  rf95.setModeIdle(); // puts radio into idle mode ready for operation
  delay(100);
}

void transmit()
{  
  for (int i=1; i<5; i++) // send 4 identification radio packets
  {
  Serial.print("Sending Ident "); Serial.println(i);
  satAddress[0]=i; //unique packet identifier
  rf95.send((uint8_t *)satAddress, 5); //send 5 Bytes of data (identifier, address, address)
  delay(10);
  rf95.waitPacketSent(); // wait until transmission is complete
  delay(100);
  }
  
  delay(100);
  Serial.println("Sending Telemetry "); 
  rf95.send((uint8_t *)satTelemetry, 50); //send 50 Bytes of telemetry data
  delay(10);
  rf95.waitPacketSent(); // wait until transmission is complete
  satTelemetry[0] = satTelemetry[0]+1; //increases the first Bytes by 1 to identify each telemetry packet
  delay(100);
}

void receive()
{
  unsigned long timeStamp = millis();
  delay(10);
  do
  {
    if (rf95.available()) //This starts if an external interrupt is RX'ed on the 2 pin (from LoRa module DIO0 pin)
    {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len)) //this performs an operation based on the received data
      {
        Serial.print("Perform control command function"); //this is where the control functions should be placed
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
  } while (millis() < timeStamp + RX_TIME);
}

void idleBeacon()
{
  rf95.sleep(); // Puts the radio in sleep mode
  delay(100);
    for (int i = 0; i < 30; i++) // Puts the processor in idle mode for 4 minutes
  {
    LowPower.idle(SLEEP_8S, ADC_OFF,TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); //for arduino pro mini
  }
  for (int i = 0; i < random(120); i++) // Puts the processor into idle mode for between 1 and 120 seconds
  {
    LowPower.idle(SLEEP_1S, ADC_OFF,TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); //for arduino pro mini
  }
  delay(100);
  rf95.setModeIdle(); // Puts the radio into the idle mode ready for operation
}

void collectData()
{
 Serial.println("*Some telemetry data is collected*");
 // This is where the function should be placed to collect telemetry data from other satellite systems
}
