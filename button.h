/**
 * Copyright 2022 Marc SIBERT
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * 
 */
template<int BUTTON, int SHORT = 10, int LONG = 50>
class Button {
public:

  Button() {
    pinMode(BUTTON, INPUT_PULLUP);
    
#ifdef __AVR_ATmega328P__
    cli();
    TCCR2A = 0;
    TCCR2B = 0;

    TCNT2 = 0;
    OCR2A = 156;
    OCR2B = 0;  // unused
    TIMSK2 = _BV(OCIE2A);
    ASSR = 0;
    
    TCCR2A = _BV(WGM21);
    TCCR2B = _BV(CS22) | _BV(CS21) | _BV(CS20); // clkT2S/1024
    sei();
#elif __ATTINY85__
    cli();
    TCCR1 = 0;  // Stop timer
    TCNT1 = 0;  // Reset timer
    GTCCR = _BV(PSR1);  // Reset prescaler
    OCR1A = 243;  // Compare value
    OCR1C = 243;  // Reset value
    TIMSK = _BV(OCIE1A);  // Interrupt on compare match with OCR1A
    TCCR1 = _BV(CS13) | _BV(CS12) | _BV(CS10);
    TCCR1 |= _BV(CTC1); // Start timer
    sei();
#else
#error NO CODE FOR TIMER INIT
#endif
  }

/**
 * Record the current state "frequently" ~10ms.
 */
  inline
  void timer() {
    for (auto i = LONG-1; i > 0; --i) states[i] = states[i-1];
    states[0] = !digitalRead(BUTTON);
  }

/**
 * Indique l'enregistrement d'une pression courte ou longue en cours (non relachée).
 * @return true si la séquence est plus longue que la taille SHORT, false sinon.
 */
  bool shortPress() const {
    byte l;
    const auto s = state(l);
    return s && (l >= SHORT);
  }

/**
 * Indique l'enregistrement d'une pression longue en cours (non relachée).
 * @return true si la séquence est plus longue que la taille LONG, false sinon.
 */
  bool longPress() const {
    byte l;
    const auto s = state(l);
    return s && (l == LONG);
  }

protected:
/**
 * Return the debounced state.
 * @param lon Nombre d'échantillons identiques
 * @return valeur de la séquence identique.
 */
  bool state(byte &lon) const {
    const auto last = states[0];
    for (auto i = 1; i < LONG; ++i) {
      if (last != states[i]) {
        lon = i;
        return last;
      }
    }
    lon = LONG;
    return last;
  }

private:
/// state pipe.
  volatile bool states[LONG];
};
