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

#define DEBUG

#include "application.h"

/**
 * PINOUT constants
 */
enum {
  PIN_LED =     8,
  PIN_BUTTON = 12
};

/// Singleton Application.
Application<PIN_LED, PIN_BUTTON> app;

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  while (!Serial) ;
  Serial.println(F(__FILE__));
  Serial.println(F("Copyright (c) 2022 par Sibert"));
#endif
  app.setup();
}

void loop() {
  app.loop();
}

ISR(TIMER2_COMPA_vect) {
  app.timer10ms();  
}
