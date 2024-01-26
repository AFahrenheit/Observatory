//___________________________________________________________________________________________________________________
//                                   ПОВОРОТ КУПОЛА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
int last_position = 0;   # последняя позиция
int new_position = 0;   # Азимут
int turn = 0;                                // шаги в градусах до намеченной цели (шагов мотора на 1 градус)

void setup() {
    Serial.begin(115200);
}

void loop() {

//___________________________________________________________________________________________________________________
//                                   ПОВОРОТ КУПОЛА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
    turn_L = last_position + new_position;   // вычисление шагов вЛево
    turn_R = last_position - new_position;   // вычисление шагов вПраво

    Serial.print("Начальная позиция:"); Serial.println(last_position);
    Serial.print("Новая позиция:"); Serial.println(new_position);

    if (turn_R < 0) {                          // если вПраво через точку 0
      turn_R += 361;
    }
    if (turn_L >= 359) {                         // если вЛЕво через точку 0
        turn_L = 360 - new_position + last_position;
    }

    pos = min(turn_L, turn_R);               // выбор наименьшего пути право/лево

    if (pos == turn_L && pos <= 180) {        // движение вправо
        Serial.println('Еду леВО');
        while (pos != turn) {
            # КОД ДЛЯ МОТОРА
            turn = 0 - pos;                  // отсчёт шагов для цели
            pos = turn;
            if (new_position == abs(turn)) {        // если достиг цели
                Serial.println("приехал влеВО");
                last_position = new_position;
                break
            }
        }
    }
    else {
        Serial.println('Еду ПРАво');                   // движение влево
        while (new_position - last_position != turn) {
            # КОД ДЛЯ МОТОРА
            turn += 1;                       // отсчёт шагов до цели
            if (pos == turn) {                 // если достиг цели
                Serial.println("приехал ПРАво")
                last_position = new_position
                break
            }
        }
    }


  Serial.print("Повернул на"); Serial.print(turn); Serial.println("углов");
  Serial.print("Последняя позиция: ");Serial.println(last_position);
}