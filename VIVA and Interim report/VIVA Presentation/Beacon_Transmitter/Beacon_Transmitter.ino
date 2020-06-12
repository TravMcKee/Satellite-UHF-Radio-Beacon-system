/*
   This File is to run the inital three cycles of the initial beacon to check the power consumption



*/
#include <SPI.h>
#include <RH_RF95.h>
#include <LowPower.h>

// for Arduino Pro Mini
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 3


#define RX_TIME 3000

char sat_Ident[] = "TravSat1"; //should it be alpha-numeric char or numbers????
int satBatVoltage = 0;
int solarPanelVoltage = 0;
int photoDiode1 = 0;
int photoDiode2 = 0;
int photoDiode3 = 0;
char radiopacket[50];
boolean launched = false;

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  Serial.begin(9600);
  // the next lines doesn't start the testing until the serial monitor is opened.
  //while (!Serial) {
  //  delay(1);
  //}
  Serial.println("Initalisation Begins");
  pinMode(RFM95_RST, OUTPUT);
  pinMode(2,OUTPUT);
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

  rf95.setTxPower(10, false);
  Serial.println("Initalisation finished");
}


void loop()
{
  if (launched == false)
  {
    Serial.println("satellite launches");
    launch(); //shuts down microcontroller for 4 seconds to represent 45 minutes after launch
    launched = true;
    Serial.begin(9600);
    delay(100);
  }

  Serial.println("Begin collecting data");
  collectData(); //collecting data - battery checks, check light sensing diode, current check
  Serial.println("Finish collecting data");

  Serial.println("begin Transmit");
  transmit();
  Serial.println("Transmission complete");

  // Now wait for a reply
  Serial.println("enter receive cycle");
  delay(10);
  recieve();
  Serial.println("finish receive cycle");
  delay(10);
  Serial.println("enter idle cycle");
  delay(10);
  idleBeacon();
  Serial.println("finish idle cycle");

}

void launch()
{
  rf95.sleep();
  Serial.flush();
  delay(100);
  for (int i = 0; i < 4; i++)
  {
    LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_OFF);
    //LowPower.idle(SLEEP_1S, ADC_OFF,TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF); //for arduino feather 32u4
    //LowPower.idle(SLEEP_1S, ADC_OFF,TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); //for arduino pro mini
    //LowPower.powerStandby(SLEEP_1S, ADC_OFF, BOD_OFF);

  }
  delay(100);
  rf95.setModeIdle();
  //USBDevice.attach();
  delay(100);
}

void transmit()
{
  delay(1000);
  sprintf(radiopacket,"ID: %s,BV: %d,SP: %d,D1, %d,D2: %d,D3: %d",sat_Ident,satBatVoltage,solarPanelVoltage,photoDiode1,photoDiode2,photoDiode3);
  delay(100);
  radiopacket[49] = 0;
  Serial.print("Sending: "); Serial.println(radiopacket);
  rf95.send((uint8_t *)radiopacket, 50);
  delay(10);
  rf95.waitPacketSent();
  delay(1000);
}

void recieve()
{
  unsigned long timeStamp = millis();
  delay(10);
  do
  {
    if (rf95.available())
    {
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

      if (rf95.recv(buf, &len))
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
  } while (millis() < timeStamp + RX_TIME);
}

void idleBeacon()
{
  rf95.sleep();
  delay(100);
  for (int i = 0; i < 5; i++)
  {
    LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_OFF);
    //LowPower.idle(SLEEP_1S, ADC_OFF,TIMER4_OFF, TIMER3_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF); //for arduino feather 32u4
    //LowPower.idle(SLEEP_1S, ADC_OFF,TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); //for arduino pro mini
    //LowPower.powerStandby(SLEEP_1S, ADC_OFF, BOD_OFF);

  }
  delay(100);
  rf95.setModeIdle();
}

void collectData()
{
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
