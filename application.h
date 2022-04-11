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

#include <FastLED.h>
#include "button.h"

/**
 * Classe Application à implémenter une seule fois (Singleton).
 */
template <int PIN_LED, int PIN_BUTTON>
class Application {
public:
/**
 * Exécutée une seule fois au démarrage.
 */
  inline
  void setup() {
    FastLED.addLeds<WS2812, PIN_LED, RGB>(leds, 1);
    FastLED.clear(true);
    hue = 0;
  }

/**  
 * Exécutée répétitivement, infiniement.
 */
  inline  
  void loop() {

    if (start.shortPress()) {
      while(start.shortPress()) {
        if (start.longPress()) {
          flash(CRGB::White);
          while (start.longPress()) {
            leds[0] = CHSV(hue++, 255, 255);
            FastLED.show();
            delay(50);
          }
//          FastLED.clear(true);
          flash(CRGB::White);
          return;
        }
        delay(250);
      }
      flash(CRGB::Green);
      sequence();
      if (start.longPress()) {
        flash(CRGB::Red); 
        while (start.longPress()) delay(250);
      }
    }
    
  }

/**
 * Méthode devant être appelée toutes les 10ms environ par une interruption.
 */
  inline
  void timer10ms() {
    start.timer();  
  }

protected:
/**
 * Produit une pulsation de la durée indiquée.
 * @param periode length un millis. 
 */
  void pulse(const unsigned periode) {
    static const auto ADJ = 950UL;  // Adjuste le temps de montée/descente
    
    const auto t0 = millis();
    leds[0] = CHSV(hue, 255, 255);
    for (byte i = 0; i < 8*32 - 4; ++i) {
      FastLED.show(pgm_read_byte(CIEL8 + ((i ^ 0x01) + 3) / 8));
      delayMicroseconds(ADJ * periode / (8*32) / 4);
      if (start.longPress()) return;
    }
    for (byte i = 0; i < 8; ++i) {
      FastLED.show(pgm_read_byte(CIEL8 + 31));
      delayMicroseconds(ADJ * periode / (8*32) / 4);
      if (start.longPress()) return;
    }
    for (unsigned i = 16*32 - 8 - 1; i > 0; --i) {
      FastLED.show(pgm_read_byte(CIEL8 + ((i ^ 0x01) + 7) / 16));
      delayMicroseconds(ADJ * periode / (16*32) / 2);
      if (start.longPress()) return;
    }
    FastLED.clear(true);
    while (periode > millis() - t0) {
      delayMicroseconds(ADJ * periode / (16*32) / 2);
      if (start.longPress()) return;
    }
  }

/**
 * Produit une séquence du temps DELAY minutes avec la variation de FAST vers SLOW pulsation/min.
 */
  template<int FAST = 11, int SLOW = 6, int DELAY = 15>
  void sequence() {
    const auto t0 = millis();
    while ((millis() < DELAY * 60000UL + t0) && !start.longPress()) {
      const auto f = (FAST * DELAY) * 60000UL - (FAST - SLOW) * static_cast<unsigned long>(millis() - t0);
      pulse(static_cast<unsigned>(DELAY * 3600000000ULL / f));
    }
    FastLED.clear(true);
  }

/**
 * Flashe la LED un nombre de fois donnée avec la couleur indiquée.
 * @param colour Couleur de flash.
 * @param repeat Nombre de flash.
 */
  void flash(const CRGB& colour, const byte repeat = 1) {
    FastLED.clear(true);
    delay(150);
    leds[0] = colour;
    for (auto i = repeat; i > 0; --i) {
      FastLED.show();
      delay(150);
      FastLED.show(0);
      delay(150);
    }
    FastLED.clear(true);
  }

private:
/// Table de correspondance de luminosité.
  static const uint8_t CIEL8[] PROGMEM;

/// Current Hue.
  unsigned hue;
  
/// FastLED needs.
  CRGB leds[1];

/// Button
  Button<PIN_BUTTON> start;
  
};

/**
 * Table de correspondance de luminosité.
 */
template <int LED, int BUTTON>
const uint8_t Application<LED, BUTTON>::CIEL8[] PROGMEM = {
   0,  1,  2,  3,  4,  5,   7,  9, 12, 15, 18, 22, 27, 32, 38, 44,
  51, 58, 67, 76, 86, 96, 108,120,134,148,163,180,197,216,235,255
};
