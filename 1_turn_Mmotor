#include <GyverStepper2.h>
GStepper2<STEPPER4WIRE> stepper(2048, 11, 9, 10, 8);     // ставим новую позицию (гр.*шаги*купол ред1/36)

//int32_t incoming; // количество углов, на который поворачивается двигатель
int32_t incoming = 90;  // -вправо, +лево

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(800);         // установка макс. скорости в шагах/сек
  stepper.autoPower(true);
}

void loop() {
    stepper.tick();
    static uint32_t tmr2;
    if (millis() - tmr2 > 20) {
      tmr2 = millis();
    }
    stepper.setTargetDeg(incoming, ABSOLUTE);

    if (stepper.ready()) {
      Serial.println("Приехал");
  }
}