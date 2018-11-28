/*
Copyright 2018 Marc SIBERT

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <Arduino.h>
#include <avr/sleep.h>

class App {
public:
  App() {}

  void setup() {
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);  

/*
// Set Timer 1 : fast PWM
    DDRB  |= _BV(PB1) | _BV(PB2);       // set pins as outputs
    OCR1A = 0;
    TCCR1A = _BV(COM1A1) | _BV(COM1B1)  // non-inverting PWM
            | _BV(WGM11);                 // mode 14: fast PWM, TOP=ICR1
    TCCR1B = _BV(WGM13) | _BV(WGM12)
            | _BV(CS11);                  // prescaler 1
    ICR1 = 256;                         // TOP counter value
*/
    

    const unsigned vBat = getVoltage();
    if (vBat < 2500) flash(1);
    else flash(vBat / 1000);
    delay(1000);

    attachInterrupt(0, App::wakeUpNow, CHANGE);   // INT0 = PD2 = D2
    App::push = 0;
    sleepNow();
  }

  void loop() {

    delay(1000);
    Serial.print("Start ");
    Serial.println(App::push);

// Impaire = pression longue sans relachement
    if (App::push % 2 == 1) {
      while (App::push % 2) { // Attendre relachement et mettre en veille.
        App::push = 0;
        sleepNow();
        return;        
      }
    } else if (App::push > 4) { // plus de 2 pressions avec relachement
      App::push = 0;
      sleepNow();
      return;        
    }

    const unsigned long duree = (App::push == 2 ? 5 * 60 : 10 * 60) * 1000UL; // 5 ou 10 min en ms

// Début d'un cycle    
    App::push = 0;
    const unsigned long start = millis();
    unsigned long now = 0;
    do {
      Serial.print(now - start);
      Serial.print(" - ");
      Serial.println(duree);
      
      for (byte i = 0; i < 64; ++i) {
        analogWrite(BLUE_LED, lum<63,50>(i));
        if (App::push > 0) {
          digitalWrite(BLUE_LED, LOW);
          return;
        }
        now = millis();
        delay((60000ULL * duree) / (11 * duree - 5 * (now - start)) / 128);
      }
      for (byte i = 0; i < 64; ++i) {
        analogWrite(BLUE_LED, lum<63,50>(63 - i));
        if (App::push > 0) {
          digitalWrite(BLUE_LED, LOW);
          return;
        }
        now = millis();
        delay((60000ULL * duree) / (11 * duree - 5 * (now - start)) / 128);
      }
    } while(now - start < duree);
    App::push = 0;
    sleepNow();
  }

protected:
  void flash(const int n) const {
    for (int i = 0 ; i < n; ++i) {
      digitalWrite(BLUE_LED, HIGH);
      delay(50);
      digitalWrite(BLUE_LED, LOW);    
      delay(450);
    }
  }
  
  void eclairer(const unsigned val) {
    DDRB  |= _BV(PB1) | _BV(PB2);       /* set pins as outputs */
    
    OCR1A = val;
    OCR1B = val;
  }
  
  unsigned getVoltage() const {
    ADCSRA |= _BV(ADEN);  // Enable ADC
    ADMUX =  0b01001110;  //  vref = AVcc ; analog input = 1.1V (VBG)
    delayMicroseconds(500); // after Vref change
    ADCSRA |= _BV(ADSC);  // This starts the conversion.
/* This is an idle loop that just wait around until the conversion
 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
 * set above, to see if it is still set.  This bit is automatically
 * reset (zeroed) when the conversion is ready so if we do this in
 * a loop the loop will just go until the conversion is ready. */
    while ( (ADCSRA & _BV(ADSC)) );
    ADCSRA &= !_BV(ADEN);  // Disable ADC
    return (1100UL * 1023 / ADC);     // AVcc = Vbg/ADC*1023 = 1.1V*1023/ADC 
  }

/**
 * Calcule la luminosité sur une echelle de [0..L] à donner à la LED suivant Brillance attendue [0..B].
 */
  template <unsigned B, unsigned L>
  unsigned lum(const unsigned b) const {
    if (b * 25 <= 2 * B) { // b <= 8.B/100
      return 1000UL * L * b / (9033UL * B);
    } else {
      const unsigned long n = 100 * b + (16 * B);
      return (1ULL * L) * n * n * n / (1560896ULL * B * B * B);
    }
  }

  void sleepNow() {
    /* Now is the time to set the sleep mode. In the Atmega8 datasheet
     * http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
     * there is a list of sleep modes which explains which clocks and
     * wake up sources are available in which sleep mode.
     *
     * In the avr/sleep.h file, the call names of these sleep modes are to be found:
     *
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     *
     * For now, we want as much power savings as possible, so we
     * choose the according
     * sleep mode: SLEEP_MODE_PWR_DOWN
     *
     */  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    /* Now it is time to enable an interrupt. We do it here so an
     * accidentally pushed interrupt button doesn't interrupt
     * our running program. if you want to be able to run
     * interrupt code besides the sleep function, place it in
     * setup() for example.
     *
     * In the function call attachInterrupt(A, B, C)
     * A   can be either 0 or 1 for interrupts on pin 2 or 3.  
     *
     * B   Name of a function you want to execute at interrupt for A.
     *
     * C   Trigger mode of the interrupt pin. can be:
     *             LOW        a low level triggers
     *             CHANGE     a change in level triggers
     *             RISING     a rising edge of a level triggers
     *             FALLING    a falling edge of a level triggers
     *
     * In all but the IDLE sleep modes only LOW can be used.
     */
 
//    attachInterrupt(0,wakeUpNow, CHANGE); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
//    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
//    attachInterrupt(0,wakeUpNow, CHANGE); // use interrupt 0 (pin 2) and run function
  }  

  inline static void wakeUpNow() {
    const unsigned long now = micros();
    if (App::when + 1000 < now) ++App::push;  // Prise en compte si plus de 10ms.
    App::when = now;
  }

private:
  static volatile int push;
  static volatile unsigned long when;

  enum {
    BLUE_LED = 10,  // 9 ou 10 = Timer1 16 bits.
    BUTTON = 2      // PD2 = INT0 /!\ Nécessaire pour réveiller le ATMega328. 
  };
 
};

volatile int App::push = 0;
volatile unsigned long App::when = 0;
