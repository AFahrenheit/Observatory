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
  if (count_LP == 1) {
  last_position = Az;
  //turn_deg = 0;
  }

  turn_L = last_position + Az;   // вычисление шагов вЛево
  turn_R = last_position - Az;   // вычисление шагов вПраво

//  turn_L = (last_position + Az) % 359;   // вычисление шагов вЛево
//  turn_R = (Az - last_position) % 359;   // вычисление шагов вПраво

//  Serial.print("Начальная позиция:"); Serial.println(last_position);
//  Serial.print("Новая позиция:"); Serial.println(Az);

  if (turn_R < 0) {                          // если вПраво через точку 0
    turn_R += 361;
  }
  if (turn_L >= 359) {                         // если вЛЕво через точку 0
    turn_L = 360 - Az + last_position;
  }

  pos = min(turn_L, turn_R);               // выбор наименьшего пути право/лево

//  if (last_position == Az) {
//    Serial.println("Стою");
//  }

  if (pos == turn_L && pos <= 180) {        // движение вправо
    Serial.println('Еду ПРАво');

    while (pos != turn_deg) {
    //while (abs(Az - last_position) != turn_deg) {
      count_LP = 0;
      turn_deg += 1;                       // отсчёт шагов до цели

      if (Az == turn_deg) {        // если достиг цели
        Serial.println("приехал ПРАво");
        // ОТПРАВЛЯЕМ ДАННЫЕ
        //last_position = Az;
        break;
      }
    }
  }

  else {
    Serial.println('Еду ЛЕво');                   // движение влево

    while (Az - last_position != turn_deg) {
      count_LP = 0;

      turn_deg = 0 - pos;                  // отсчёт шагов для цели
      pos = turn_deg;

      if (pos == turn_deg) {                 // если достиг цели
      // if ((Az == (abs(turn_deg) + last_position) % 359)) { //  {                // если достиг цели
        Serial.println("приехал ЛЕво");
        // ОТПРАВЛЯЕМ ДАННЫЕ
        break;
      }
    }
  }
}