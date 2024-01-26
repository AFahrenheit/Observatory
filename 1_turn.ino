//___________________________________________________________________________________________________________________
//                                   ПОВОРОТ КУПОЛА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
int last_position = 0;   # последняя позиция
int new_position = 0;   # Азимут
int turn_deg = 0;                                // шаги в градусах до намеченной цели (шагов мотора на 1 градус)

void setup() {
    Serial.begin(115200);
}

void loop() {

//___________________________________________________________________________________________________________________
//                                   ПОВОРОТ КУПОЛА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  stepper.tick();

  turn_L = last_position + Az;   // вычисление шагов вЛево
  turn_R = last_position - Az;   // вычисление шагов вПраво

  Serial.print("Начальная позиция:"); Serial.println(last_position);
  Serial.print("Новая позиция:"); Serial.println(Az);

  if (turn_R < 0) {                          // если вПраво через точку 0
    turn_R += 361;
  }
  if (turn_L >= 359) {                         // если вЛЕво через точку 0
    turn_L = 360 - Az + last_position;
  }

  pos = min(turn_L, turn_R);               // выбор наименьшего пути право/лево

  if (pos == turn_L && pos <= 180) {        // движение вправо
    Serial.println('Еду леВО');
    while (pos != turn_deg) {

      // КОД ДЛЯ МОТОРА
      static uint32_t tmr2;
      if (millis() - tmr2 > 20) {
        tmr2 = millis();
      }
      stepper.setTargetDeg(turn_deg, ABSOLUTE);     // ставим новую позицию (градусы * кол. шагов гр. ред1/36)

      turn_deg = 0 - pos;                  // отсчёт шагов для цели
      pos = turn_deg;
      if (Az == abs(turn_deg)) {        // если достиг цели
        Serial.println("приехал влеВО");
        stepper.reset();        // обнуление положения 0
        last_position = Az;
        break;
      }
    }
  }
  else {
    Serial.println('Еду ПРАво');                   // движение влево
    while (Az - last_position != turn_deg) {

      // КОД ДЛЯ МОТОРА
      static uint32_t tmr2;
      if (millis() - tmr2 > 20) {
        tmr2 = millis();
      }
      stepper.setTargetDeg(turn_deg, ABSOLUTE);     // ставим новую позицию (градусы * кол. шагов гр. ред1/36)

      turn_deg += 1;                       // отсчёт шагов до цели
      if (pos == turn_deg) {                 // если достиг цели
        Serial.println("приехал ПРАво");
        stepper.reset();        // обнуление положения 0
        last_position = Az;
        break;
      }
    }
  }
}