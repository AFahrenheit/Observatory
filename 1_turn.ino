last_position = 0;   // Текущая позиция
Az = 0;   // Азимут

// Serial.print("Двигаемся с"); Serial.print(last_position); Serial.print("на"); Serial.println(Az);

turn_L = last_position - Az;   // вычисление шагов вЛево
turn_R = Az - last_position;   // вычисление шагов вПраво

pos = min(turn_L, turn_R);              // выбор наименьшего пути право/лево

if (Az - last_position == 0){
    Serial.println("Стоим.");}

else if (Az > last_position and abs(pos) < 180) {
    Serial.print("1 Еду ПРАво"); Serial.print(abs(pos)); Serial.print("углов");}

// if (Az - last_position < -180) {
//     pos = 360 + (Az - last_position);
//     Serial.print("1 Еду ПРАво"); Serial.print(abs(pos)); Serial.print("углов");}

else if (abs(pos) >= 180){
    pos += 360;
    if (pos > 0) {
        pos *= -1;}
    Serial.print("2 Еду levo"); Serial.print(pos); Serial.print("углов");}  // сделать минус

// if (Az - last_position > 180) {
//     pos = Az - last_position;
//     Serial.print("3 Еду ПРАво"); Serial.print(pos); Serial.print("углов");}

else if (abs(turn_R) > 180) {
    if (turn_R < -180){
        pos = 360 + (Az - last_position);
        print('1 Еду ПРАво', abs(pos), 'углов');}
    else if (turn_R > 180){
        pos = turn_R;
        print('3 Еду ПРАво', pos, 'углов');}}

else {
    Serial.print("4 Еду levo"); Serial.print(pos); Serial.print("углов");}