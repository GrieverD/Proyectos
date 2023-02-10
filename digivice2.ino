#pragma once

#include "Reloj.h"

bool showAnimation = true;


void setup() {
  setupReloj();

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Wire.begin(5, 4);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
}

void loop() {
  runClock();
  printTime();
}
