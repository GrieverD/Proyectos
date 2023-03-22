//coded by cefuve.com
#include "Animaciones.h"

#define pin_min 2
#define pin_hour 3

String amPM = "AM";

int ampmActual = 0;
int hour = 12;
int minute = 59;
int second = 50;
int barCount = 0;
int barWhite = 0;

long tiempo_actual = 0;
unsigned long previousTime = 0;




bool modo_edicion = false;
bool wait = false;
bool blink_time = false;
bool barras = false;

int old_min = 1;
int old_hour = 1;
int count = 0;


void setupReloj() {
  Serial.begin(9600);
  pinMode(pin_min, INPUT);
  pinMode(pin_hour, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(100);
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);

}

void printTime() {

  unsigned long currentTime = millis();

  if (ampmActual == 0) {
    amPM = "AM";
  }
  else {
    amPM = "PM";
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(25, 18);

  if (hour < 10) display.print("0");
  display.print(hour);
  if (blink_time) display.print(":");
  if (!blink_time) display.print(" ");
  if (minute < 10) display.print("0");
  display.print(minute);

  display.setCursor(95, 25);
  display.setTextSize(1);
  if (second < 10) display.print("0");
  display.print(second);

  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(amPM);

  if (currentTime - previousTime > 1000) { // 1 segundo en milisegundos
    previousTime = currentTime;
    barCount++;
    
   if (barCount >= 6) {
      barCount = 1;
    }
  }

  for (int i = 0; i <= 6; i++) {
    if (barCount <= i) {
      display.fillRect((25 + (i * 16)), (50), 10, 18, 0);
    }
    else {
      display.fillRect((25 + (i * 16)), (50), 10, 18, 1);
    }
  }
  
  display.display();

  //  if (currentTime - previousTime > 1000) { // 1 segundo en milisegundos
  //    previousTime = currentTime;
  //    barCount++;
  //
  //    if (barCount == 5) {
  //      display.setCursor(20, 18);
  //      for (int j = 0; j < (barWhite); j++) {
  //        display.fillRect((25 + (j * 16)), (50), 10, 18, 0);
  //      }
  //      if (barWhite >= 5 ) {
  //        barCount = 0;
  //        barWhite = 0;
  //      }
  //    }
  //  }
  //  if (barCount <= 5) {
  //    display.setCursor(20, 18);
  //    for (int i = 0; i < (barCount); i++) {
  //      display.fillRect((25 + (i * 16)), (50), 10, 18, 1);
  //    }
  //  }
  //  display.display();

  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
}

void runClock() {
  //Modo edición
  int lectura_min = digitalRead(pin_min);
  int lectura_hour = digitalRead(pin_hour);

  if (lectura_min == 0 && lectura_hour == 0) {
    count++;
    wait = true;
    delay(1);
  } else {
    if (wait == true) {
      wait = false;
      delay(400);
      return;
    }
  }

  if (lectura_min == 1 && lectura_hour == 1) {
    if (count > 1500) {
      modo_edicion = !modo_edicion;
      second = 0;
      old_min = 1;
      old_hour = 1;
    }
    count = 0;
  }

  if (lectura_min == 1 && old_min == 0 && modo_edicion) {
    minute++;
    if (minute == 60) minute = 0;
    printTime();
  }

  if (lectura_hour == 1 && old_hour == 0 && modo_edicion) {
    hour++;
    if (hour == 13) hour = 1;
    printTime();
  }


  old_min = lectura_min;
  old_hour = lectura_hour;
  if (modo_edicion) return;


  //Reloj digital

  //if(millis() >= tiempo_actual + 1000)   Lógica que genera error
  if (millis() - tiempo_actual > 1000) {
    tiempo_actual = millis();
    blink_time = !blink_time;
    second++;

    if (second == 60) {
      second = 0;
      minute++;
    }
    if (minute == 60) {
      minute = 0;
      hour++;
    }
    if (hour == 13) {
      hour = 1;
      ampmActual = !ampmActual;
    }

    printTime();
  }
}
