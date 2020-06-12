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
#define RFM95_CS 10
#define RFM95_RST 4
#define RFM95_INT 3 
#define LED 13

// set the frequency
//#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  Serial.begin(9600);
  (!Serial);
  delay(100);
  Serial.println("starts now");
  delay(100);
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
  delay(100);
  Serial.println("send radio to sleep");
  rf95.sleep();
  delay(2000);
  Serial.println("check powerDown");
  delay(100);
  for (int i=0; i<3;i++) {
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  delay(2000);
  Serial.println("check powerSave");
  delay(100);
  for (int i=0; i<3;i++) {
  LowPower.powerSave(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_OFF);
  }
  delay(2000);
  Serial.println("check powerStandby");
  delay(100);
  for (int i=0; i<3;i++) {
  LowPower.powerStandby(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  delay(2000);
  Serial.println("check powerExtStandby");
  delay(100);
  for (int i=0; i<3;i++) {
  LowPower.powerExtStandby(SLEEP_1S, ADC_OFF, BOD_OFF, TIMER2_OFF);
  }
  delay(3000);
  Serial.println("check idle");
  delay(100);
  for (int i=0; i<3;i++) {
  LowPower.idle(SLEEP_1S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF); 
  }
  delay(5000);
}
