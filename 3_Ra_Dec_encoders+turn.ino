//____________________________________________________________________________________________
//                                     МОТОР
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
//#define GS_FAST_PROFILE количество
#include <GyverStepper2.h>
GStepper2<STEPPER4WIRE> stepper(2048, 11, 9, 10, 8);     // ставим новую позицию (гр.*шаги*купол ред1/36)
//GStepper2<STEPPER2WIRE> stepper(200, 5, 4, 3);                      // драйвер steps, +pul, +dir, +enable

//___________________________________________________________________________________________________________________
//                                   ЧАСЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <microDS3231.h>
MicroDS3231 rtc;

int8_t  hour;      //часы
int8_t  minute;   //минуты
int8_t second;
int8_t date;      //число
int8_t month;      //месяц
int year;       //год
//___________________________________________________________________________________________________________________
//                                   БИБЛИОТЕКИ ЭНКОДЕРЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <EncButton.h>
EncButton enc2(14, 15); // (4, 5) a0a1 14, 15 (свободные a0 a1 a3 d6 d7 d8)
EncButton enc1(6, 7); // (2, 3) d6d7 6, 7


//___________________________________________________________________________________________________________________
//                                   ЭНКОДЕРЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
int Dec = 4800; //(4800тэ в 360°) начальное 90°      (1200тэ = 90°, 13,3 ТЭ = 1°)
int Ra = 0;  // начальное 1440 минут в 24 часах (1200тэ = 90° = 6час, 3,33333 ТЭ = 1 мин)

int mRa; // формулы рассчёта часа от склонения
int x;          // множитель для часа по склонению
int Ra_turn;        // Ra минуты
float degDec;        // Dec градусы
int Ra_last; int Dec_last;  // последние показатели

//___________________________________________________________________________________________________________________
//                                   ПЕРЕМЕННЫЕ РАССЧЁТА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
float Ra_h;
float Dec_rad;              //склонение в радианах (радианы * 0.0175)
float pLST;          // Местное звездное время
float pUT;          // Всемир. вр. (16:12)       (20:15)         (20:15)
float pGST;         // Зв.вр.Gr    (20:08)       (20.38)         (15.44)
float JD;   //Юлиан. календарь     2460230.     2460176,34     2459736.50
float Longit_rad = 37.91111 * 0.0175;  // долгота 37'54'40 (радианы)
float Lat_rad = 54.703 * 0.0175;   // широта 54'42'11  (радианы)
float pLocalTime;
int pDs = 0;         // Переход на летнее время (0 \ 1)
uint8_t pTz = 3;         // Часовой пояс
float HA; float HA_1;
float AzEq; float pAzHor; float pAltHor;

int Az = 0; int Alt;                // азимут высота

float A; float B;
float lT; float lR0; float lR1; float lT0;
//___________________________________________________________________________________________________________________
//                                   ПОВОРОТ КУПОЛА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
int last_position;   // последняя позиция
int32_t turn_deg = 0;                                // шаги в градусах до намеченной цели (шагов мотора на 1 градус)
int turn_R; int turn_L;
int pos;


void setup() {
//___________________________________________________________________________________________________________________
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VOID SETUP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  Serial.begin(115200);
//____________________________________________________________________________________________
//                                     МОТОР
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
stepper.setMaxSpeed(800);         // установка макс. скорости в шагах/сек
stepper.autoPower(true);          // пусть драйвер выключается при достижении позиции
//___________________________________________________________________________________________________________________
//                                   ЧАСЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  if (!rtc.begin()) {       // проверка наличия модуля на линии i2c
    for(;;);

  rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
  }
  delay(5000);
}
void loop() {
//___________________________________________________________________________________________________________________
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VOID LOOP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
    ra_dec();
    julian_date();
    convert_LocalTime_to_LST();
    hour_angle();
    equatorial_to_horizontal();
    Print();
    Turn();
    delay(5000);                  // ПЕРИОД РАССЧЕТА НОВЫХ ДАННЫХ ОБСЕРВАТОРИИ 10 СЕКУНД
}
void yield() {
//___________________________________________________________________________________________________________________
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VOID YIELD<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  encoders();
  calculation_Ra_Dec();
}
void encoders() {
//___________________________________________________________________________________________________________________
//                                   ЭНКОДЕРЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  enc1.tick();
  enc2.tick();
  if (enc1.turn()) {
    if (enc1.right()) {
      Dec -= 1;
      if (Dec < -4799) {
        Dec = 4800;
      }
    }
    if (enc1.left()) {
      Dec += 1;
      if (Dec > 4799) {
        Dec = -4799;
      }
    }
  }
  if (enc2.turn()) {
    if (enc2.left()) {
      Ra += 1;
      if (Ra > 1199) {  // было 1200
        Ra = 0;
      }
    }
    if (enc2.right()) {
      Ra -= 1;
      if (Ra < -1199) {  // было 1200
        Ra += 1;
      }
    }
  }
}
void calculation_Ra_Dec() {
//___________________________________________________________________________________________________________________
//                                   РАСЧЁТ ПОЛОЖЕНИЯ ТЕЛЕСКОПА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

  degDec = 90.0 - ((4800.0 - abs(Dec)) / 13.3);

  if (Dec < 0) {
    x = 1;
    Ra_turn = int(x * 360 - (Ra / 3.33333) + (pLST * 60)) % 1440;
  }
  else if (Dec > 0) {
    x = 3;
    Ra_turn = int(x * 360 - (Ra / 3.33333) + (pLST * 60)) % 1440;
  }

  Ra_last = Ra;
  Dec_last = Dec;
}
void ra_dec() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕТ ЧАСЫ RA И РАДИАНЫ DEC
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  Ra_h = Ra_turn * 0.016666667;
  Dec_rad = degDec * 0.0175;
}
void julian_date() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ ДАТУ ПО ЮЛИАНСКОМУ КАЛЕНДАРЮ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

  hour = rtc.getHours();       //часы
  minute = rtc.getMinutes();   //минуты
  second = rtc.getSeconds();
  date = rtc.getDate();        //число
  month = rtc.getMonth();      //месяц
  year = rtc.getYear();        //год

  if (month <= 2) {
    year -= 1;
    month += 12;}
  A = floor(year * 0.01); // / 100.00);
  B = 2.0 - A + floor(A * 0.25); // / 4.0);
  JD = floor(365.25*(year+4716.0))+ floor(30.6001*(month+1.0))+date+(0.04*hour)+B-1524.5; // с часами ТОЧНЕЕ
}
void convert_LocalTime_to_LST() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ ВСЕМИРНОЕ ВРЕМЯ ПО ГРИНВИЧУ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  lT = (JD / 36525.0) - 1;
  lR0 = lT * (0.0513366 + lT * (0.00002586222 - lT * 0.000000001722));
  lR1 = 6.697374558 + 2400.0 * (lT - ((year - 2000.0) / 100.0));          // ПРОБЛЕМА С ТОЧНОСТЬЮ!!!
  lT0 = {normalize0to24(lR0+lR1)};

//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  pLocalTime = hour + (0.016 * minute);     // время в десятичной форме
  pUT = {normalize0to24(pLocalTime - pDs - pTz)};

//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ ЗВЕЗДНОЕ ВРЕМЯ ПО ГРИНВИЧУ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  pGST = {normalize0to24((pUT * 0.997269625) + lT0 - 0.11)};


//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ (LST)
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  pLST = {normalize0to24(pGST + (Longit_rad / 15.0))};
}
void hour_angle() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ ЧАСОВОЙ УГОЛ (hourAngel)
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  HA_1 = {normalize0to24(pGST + (Longit_rad / 15.0))};
  HA = {normalize0to24(HA_1 - Ra_h)};
}
void equatorial_to_horizontal() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ АЗИМУТ И ВЫСОТУ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  AzEq = (HA * 15.0) * 0.0175;               // радианы

  pAltHor = asin((sin(Dec_rad) * sin(Lat_rad)) + (cos(Dec_rad) * cos(Lat_rad) * cos(AzEq)));
  pAzHor = acos((sin(Dec_rad) - (sin(Lat_rad) * sin(pAltHor))) / (cos(Lat_rad) * cos(pAltHor)));
  if (sin(AzEq) > 0) {
    pAzHor = 2 * PI - pAzHor;}
  Az = pAzHor * 57.3;
  Alt = pAltHor * 57.3;
}
float normalize0to24(float x) {
//___________________________________________________________________________________________________________________
//                                   ПРИВЕДЕНИЕ В ПОРЯДОК
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  while (x > 24.0){
    x -= 24.0;}
  while (x < 0) {
    x += 24.0;}
  return x;
}
void Turn() {
//___________________________________________________________________________________________________________________
//                                   ПОВОРОТ КУПОЛА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  stepper.tick();

  turn_L = last_position + Az;   // вычисление шагов вЛево
  turn_R = Az - last_position;   // вычисление шагов вПраво

  if (turn_R < 0) {                          // если вПраво через точку 0
    turn_R += 361;
  }
  if (turn_L >= 359) {                         // если вЛЕво через точку 0
    turn_L = 360 - Az + last_position;
  }

  pos = min(turn_L, turn_R);               // выбор наименьшего пути право/лево

  static uint32_t tmr2;
  if (millis() - tmr2 > 20) {
    tmr2 = millis();
  }

  if (pos == turn_L && pos <= 180) {        // движение вправо
    //Serial.println("Еду леВО");

    while (pos != turn_deg) {
      // КОД ДЛЯ МОТОРА
      //stepper.setTargetDeg(turn_deg, ABSOLUTE);     // ставим новую позицию (градусы * кол. шагов гр. ред1/36)
      turn_deg = 0 - pos;                  // отсчёт шагов для цели
      pos = turn_deg;

      if (Az == abs(turn_deg)) {        // abs если достиг цели
        //Serial.println("приехал влеВО");
        //stepper.reset();        // обнуление положения 0
        break;
      }
    }
  }
  else {
    //Serial.println("Еду ПРАво");                   // движение влево

    while (Az - last_position != turn_deg) {
      // КОД ДЛЯ МОТОРА
      //stepper.setTargetDeg(turn_deg, ABSOLUTE);     // ставим новую позицию (градусы * кол. шагов гр. ред1/36)
      turn_deg += 1;                       // отсчёт шагов до цели

      if (Az == turn_deg) {                 // если достиг цели
        //Serial.println("приехал ПРАво");
        //stepper.reset();        // обнуление положения 0
        break;
      }
    }
  }
}
void Print() {
//___________________________________________________________________________________________________________________
//                                   ВЫВОД
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
//  Serial.println("Навигация");
//  Serial.print("Дата : "); Serial.print(date); Serial.print("."); Serial.print(month%12); Serial.print("."); Serial.println(year);
  Serial.print("Время: "); Serial.print(hour); Serial.print(":"); Serial.println(minute);
  Serial.print("Юлианский: "); Serial.println(JD);
//  Serial.print("Азимут: "); Serial.print(Az); Serial.println("градусы");
//  Serial.print("Новая позиция:"); Serial.println(Az);
//  Serial.print("Последняя позиция:"); Serial.println(last_position);
  Serial.print("Повернул на "); Serial.print(turn_deg); Serial.println(" углов"); // ОТПРАВЛЯЕМ TURN_DEG
//  Serial.print("Pos: "); Serial.print(pos);
  Serial.println(" ");
}


//___________________________________________________________________________________________________________________
//                                   ВЫВОД
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

//  Serial.print("Dec_rad: "); Serial.println(Dec_rad);
//  Serial.print("sin(Dec_rad): "); Serial.println(sin(Dec_rad));
//  Serial.print("cos(Dec_rad): "); Serial.println(cos(Dec_rad));
//  Serial.print("Lat_rad: "); Serial.println(Lat_rad);
//  Serial.print("sin(Lat_rad): "); Serial.println(sin(Lat_rad));
//  Serial.print("cos(Lat_rad): "); Serial.println(cos(Lat_rad));
//  Serial.print("AzEq: "); Serial.println(AzEq);
//  Serial.print("cos(AzEq): "); Serial.println(cos(AzEq));
//  Serial.print("(sin(Dec_rad) * sin(Lat_rad)): "); Serial.println((sin(Dec_rad) * sin(Lat_rad)));
//  Serial.print("(cos(Dec_rad) * cos(Lat_rad) * cos(AzEq)): "); Serial.println((cos(Dec_rad) * cos(Lat_rad) * cos(AzEq)));
//  Serial.print("pAltHor"); Serial.println(pAltHor);
//
//  Serial.print("sin(pAltHor): "); Serial.println(sin(pAltHor));
//  Serial.print("cos(pAltHor): "); Serial.println(cos(pAltHor));
//  Serial.print("(sin(Dec_rad) - (sin(Lat_rad) * sin(pAltHor)): "); Serial.println((sin(Dec_rad) - (sin(Lat_rad) * sin(pAltHor))));
//  Serial.print("(cos(Lat_rad) * cos(pAltHor)): "); Serial.println((cos(Lat_rad) * cos(pAltHor)));
//  Serial.print("(sin(Dec_rad) - (sin(Lat_rad) * sin(pAltHor))) / (cos(Lat_rad) * cos(pAltHor)): "); Serial.println((sin(Dec_rad) - (sin(Lat_rad) * sin(pAltHor))) / (cos(Lat_rad) * cos(pAltHor)));
//  Serial.print("pAzHor"); Serial.println(pAzHor);

//  Serial.print("Юлиан. кал.: "); Serial.println(JD);
//  Serial.print("lT: "); Serial.println(lT);
//  Serial.print("lR0: "); Serial.println(lR0);
//  Serial.print("lR1: "); Serial.println(lR1);
//  Serial.print("lT0: "); Serial.println(lT0);
//  Serial.print("pUT: "); Serial.print(pUT);Serial.print("     "); Serial.print(int(pUT));Serial.print(":"); Serial.println(round((pUT - int(pUT))*60));
//  Serial.print("pGST: "); Serial.print(pGST); Serial.print("     "); Serial.print(int(pGST));Serial.print(":"); Serial.println(round((pGST - int(pGST))*60));
//  Serial.print("pLST (дес): "); Serial.print(pLST); Serial.print("     "); Serial.print(int(pLST)); Serial.print(":"); Serial.println(round((pLST - int(pLST))*60));
//  Serial.println("  ");
//  Serial.print("LocalTime (дес): "); Serial.print(pLocalTime); Serial.print("     "); Serial.print(int(pLocalTime)); Serial.print(":"); Serial.println(round((pLocalTime - int(pLocalTime))*60));
//  Serial.print("Часовой угол: "); Serial.print(HA);  Serial.print("     "); Serial.print(int(HA));Serial.print(":"); Serial.println((HA - int(HA))*60);
//  Serial.print("AzEq: "); Serial.print(AzEq); Serial.println("радианы");
//  Serial.println("  ");
//  Serial.print("Dec_rad: "); Serial.println(Dec_rad);
//  Serial.print("Dec: "); Serial.println(Dec);
//  Serial.print("Dec мин: ");Serial.println((90.0 - ((4800.0 - abs(Dec)) / 13.3)) *  60);
//  Serial.print("Dec гр: ");Serial.println(degDec);
//  Serial.println(" ");
//  Serial.print("Ra_turn: "); Serial.println(Ra_turn);
//  Serial.print("Ra: "); Serial.println(Ra);

//  Serial.println("Навигация");
//  Serial.print("Дата : "); Serial.print(date); Serial.print("."); Serial.print(month%12); Serial.print("."); Serial.println(year);
//  Serial.print("Время: "); Serial.print(hour); Serial.print(":"); Serial.println(minute);
//  Serial.print("Dec гр: "); Serial.println(degDec);
//  Serial.print("Ra_h: "); Serial.println(Ra_h);
//  Serial.print("Азимут: "); Serial.print(Az); Serial.println("градусы");
//  Serial.print("Высота: "); Serial.print(Alt); Serial.println("градусы");
//  Serial.print("Ra(t) час: "); Serial.print(Ra_turn/60);  Serial.print(":"); Serial.println(int(Ra_turn)%60);
//  Serial.print("Новая позиция:"); Serial.println(Az);
//  Serial.print("Последняя позиция:"); Serial.println(last_position);
//  Serial.print("Повернул на "); Serial.print(turn_deg); Serial.println(" углов");
//  Serial.print("Pos:"); Serial.println(pos);
//  Serial.println(" ");