#include "avr/sleep.h"
#include "avr/wdt.h"

// Time definitions for Watch-dog-timer
#define WDT_1S B00000110  // 1s
#define WDT_2S B00000111  // 2s
#define WDT_4S B00100000  // 4s
#define WDT_8S B00100001  // 8s

const uint8_t pinPump = 0;
const uint32_t interval = (24 * 60 * 60) / 10;

// Intrpt svc rtn for WDT ISR (vect)
ISR(WDT_vect) {}

void setupWDT(byte sleepT) {
  sleepT += B00010000;  // sleepTime + Enalbe WD-change bit-on
  MCUSR &= B11110111;   // Prepare WDT-reset-flag in MCU-status-Reg
  WDTCR |= B00011000;  // Enable WD-system-reset + WD-change
  WDTCR = sleepT;      // Set sleepTime + Enable WD-change
  WDTCR |= B01000000;  // Finally, enable WDT-interrrupt
}

uint32_t counter;

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  setupWDT(WDT_8S);

  pinMode(pinPump, OUTPUT);
  digitalWrite(pinPump, LOW);

  counter = 0;
}

void deepSleep(void) {
  ADCSRA &= B01111111;  // disable ADC to save power
  sleep_enable();
  sleep_cpu();  // sleep until WDT-interrupt
  sleep_disable();
  ADCSRA |= B10000000;  // enable ADC again
}

void loop() {
  digitalWrite(pinPump, counter == 0 ? HIGH : LOW);
  counter = counter < interval ? counter + 1 : 0;

  deepSleep();
}
