
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "Digimons.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

int counter = 1;
int Alazar = 0;

unsigned long tiempoAnterior = 0;
int semilla = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void Animacion1() {
  randomSeed(analogRead(0));

  static int xPos = 0;
  static int yPos = 0;
  static int xScale = 0;

  // Obtenga el tiempo actual en milisegundos
  unsigned long tiempoActual = millis();

  // Verifique si han pasado 5 minutos (300000 milisegundos) desde la última actualización
  if (tiempoActual - tiempoAnterior > 300000) {
    // Actualiza la semilla y el tiempo anterior
    semilla = random(0, 10);
    randomSeed(semilla);
    tiempoAnterior = tiempoActual;
  }

  long movimiento = (random(0, 6) == 0) ? -6 : 6;

  if (movimiento < 0) {
    display.clearDisplay();
    display.drawBitmap(xPos, 25, Agumon4_bmp, 40, 40, 1);
    display.display();
    delay(400);

    display.clearDisplay();
    display.drawBitmap(xPos, 25, Agumon3_bmp, 40, 40, 1);
    display.display();
    delay(300);
  }
  else {

    display.clearDisplay();
    display.drawBitmap(xPos, 25, Agumon1_bmp, 40, 40, 1);
    display.display();
    delay(300);
    display.clearDisplay();
    display.drawBitmap(xPos, 25, Agumon2_bmp, 40, 40, 1);
    display.display();
    delay(300);

  }
  xPos = xPos + movimiento;
  if (xPos >= 128 - Agumon_WIDTH || xPos < 0) {
    movimiento = -movimiento;
    for (int i = 0; i < 10; i++) {
      xPos = xPos + movimiento;
      if (movimiento == -movimiento) {
        display.clearDisplay();
        display.drawBitmap(xPos, 25, Agumon1_bmp, 40, 40, 1);
        display.display();
        delay(400);
        display.clearDisplay();
        display.drawBitmap(xPos, 25, Agumon2_bmp, 40, 40, 1);
        display.display();
        delay(400);
      }
      else {
        display.clearDisplay();
        display.drawBitmap(xPos, 25, Agumon4_bmp, 40, 40, 1);
        display.display();
        delay(400);
        display.clearDisplay();
        display.drawBitmap(xPos, 25, Agumon3_bmp, 40, 40, 1);
        display.display();
        delay(400);

      }
    }
  }
  if (xPos == 64 - Agumon_WIDTH / 2) {
    delay(2000);
  }
}
