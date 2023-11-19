#include "avr/sleep.h"

// Time definitions for Watch-dog-timer.
// 1sec at 128kHz clock corresponds to 1.125sec in actual mesured time.
#define WDT_1S B00000110  // 1sec (= 1.125sec)
#define WDT_2S B00000111  // 2sec (= 2.25sec)
#define WDT_4S B00100000  // 4sec (= 4.5sec)
#define WDT_8S B00100001  // 8sec (= 9sec)

const uint8_t pinPump = 0;
const uint16_t pumpDuration = 4000; // Run pump 4sec (4.5sec at 128kHz clock).
const byte sleepDuration = WDT_8S; // Sleep 8sec but the actual duration of sleep is 9sec at 128KHz clock.
const uint16_t interval = 9600; // = ((24 * 60 * 60) / 1) / 9; Run pump once a day.

// Intrpt svc rtn for WDT ISR (vect)
ISR(WDT_vect) {}

void setupWDT(byte sleepT) {
  sleepT += B00010000; // sleepTime + Enalbe WD-change bit-on
  MCUSR &= B11110111;  // Prepare WDT-reset-flag in MCU-status-Reg
  WDTCR |= B00011000;  // Enable WD-system-reset + WD-change
  WDTCR = sleepT;      // Set sleepTime + Enable WD-change
  WDTCR |= B01000000;  // Finally, enable WDT-interrrupt
}

uint16_t counter;

void setup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  setupWDT(sleepDuration);

  pinMode(pinPump, OUTPUT);
  digitalWrite(pinPump, LOW);

  counter = 0;
}

void deepSleep(void) {
  ADCSRA &= B01111111;  // disable ADC to save power
  sleep_enable();
  sleep_cpu();          // sleep until WDT-interrupt
  sleep_disable();
  ADCSRA |= B10000000;  // enable ADC again
}

void loop() {
  if (counter == 0) {
    digitalWrite(pinPump, HIGH);
    delay(pumpDuration);
  }
  digitalWrite(pinPump, LOW);

  counter++;
  if (counter >= interval) {
    counter = 0;
  }

  deepSleep();
}
