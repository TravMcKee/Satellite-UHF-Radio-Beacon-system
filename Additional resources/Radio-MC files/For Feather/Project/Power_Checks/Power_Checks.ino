/*
 * This code is for testing the power consumption of the Satellite beacon during each phase of operation

 * Author: Travis McKee
 * Date: 29 Aug 2019
 * Version: 1.0
 **/
#include <SPI.h>
#include <RH_RF95.h>
#include <LowPower.h>

// for feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
#define LED 13

// set the frequency
//#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(115200);
  (!Serial);
  delay(100);
  Serial.println("starts now");
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(LED, OUTPUT); //For turning on the LED

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Defaults after init are 915.0MHz, modulation GFSK_Rb250Fd250, +23dbM
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  if (!rf95.setFrequency(915.0)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  rf95.setTxPower(23, false);
}

void loop() {
  rf95.sleep();
  delay(3000);
  Serial.println("check");
  USBDevice.detach();
  delay(100);
  //LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  //LowPower.powerSave(SLEEP_8S, ADC_OFF, BOD_OFF, TIMER2_OFF);
  //LowPower.powerStandby(SLEEP_8S, ADC_OFF, BOD_OFF);
  //LowPower.powerExtStandby(SLEEP_8S, ADC_OFF, BOD_OFF, TIMER2_OFF);
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER4_OFF, TIMER3_OFF, TIMER1_OFF,TIMER0_OFF, SPI_OFF, USART1_OFF, TWI_OFF, USB_OFF);
  delay(1000);
  USBDevice.attach();
  Serial.begin(115200);
  (!Serial);
  Serial.println("check 1");
  delay(3000);
}
