//------------------------
//=========ЧАСЫ===========
//------------------------
#include <microDS3231.h>
MicroDS3231 rtc;

//-----------------------
//========ЭНКОДЕР========
//-----------------------
#include <EncButton.h>
EncButton enc2(4, 5);
EncButton enc1(2, 3);

//=====ПРОБА
int8_t Hours = 19;      //если 19
int8_t Minutes = 12;    //если 12
int8_t Seconds = 30;    //если 30
int Dec = 0; // начальное 90 градусов
int Ra = 0;  // начальное 1440 минут в 24 часах (2700 в 45 градусах)


void setup() {
  Serial.begin(115200);
  rtc.setTime(COMPILE_TIME); // установит дату и время, равное времени компиляции программы
}

void loop() {
  enc1.tick();
  enc2.tick();

  if (enc1.turn()) {
    if (enc1.right()) {
      Dec += 1;
      if (Dec > 5399) {
        Dec -= 1;
      }
    }
    if (enc1.left()) {
      Dec -= 1;
      if (Dec < -5399) {
        Dec += 1;
      }
    }
    Serial.print("Dec: "); Serial.println(Dec);
  }

  if (enc2.turn()) {
    if (enc2.right()) {
      Ra += 1;
      if (Ra > 2699) {
        Ra = 0;
      }
    }
    if (enc2.left()) {
      Ra -= 1;
      if (Ra < -2699) {
        Ra += 1;
      }
    }
    Serial.print("Ra: "); Serial.println(Ra);
  }
}