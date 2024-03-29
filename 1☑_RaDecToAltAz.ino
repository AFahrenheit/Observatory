//___________________________________________________________________________________________________________________
//                                   ЧАСЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <microDS3231.h>
MicroDS3231 rtc;

//___________________________________________________________________________________________________________________
//                                   БИБЛИОТЕКИ ЭНКОДЕРЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <EncButton.h>
EncButton enc2(14, 15); // (4, 5) a0a1 14, 15 (свободные a0 a1 a3 d6 d7 d8)
EncButton enc1(6, 7); // (2, 3) d6d7 6, 7

//===============================================================================================
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>INPUT<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//===============================================================================================
//                  1                    2                    3                    4
// Имя           Arcturus              Mirach              Кebalrai              Сapella
// RA              855                  69                   1064                 318
// Dec             1143                2135                  273                  2761
// pUT           (16:12)OK           (20:15)-1m           (20:15)1m             (00:39)-2m
// pGST          (20:06)OK           (20.38)-2m           (15.44)-3m            (17.07)OK
// Ч.У.          (5:51)+2m           (19:28)+1m           (21:59)+1m            (11:47)+4m
// Азимут       (279) 280.2          (84) 84.3            (143) 143.8           (357) 358.8
// Высота       (16.5) 16.3         (40.5) 41.3           (33.3) 34.5          (11.52) 11.8
int8_t  hour = now.hour;      //часы       19             23              23
int8_t  minute = now.minute;   //мин.       12             15              15
int8_t date = 21;      //день       12             19              5
int8_t month = 4;      //месяц      10             8               6
int year = 2023;       //год       2023           2023            2022
int Ra = 318;          //п.в.    14h15m(855)    1h09m(69)     17h44m(1064)
int Dec = 2761;        //склон.  19gr03(1143)  35gr37(2137)    4gr33(273)


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
float Az; float Alt;                // азимут

float A; float B;
float lT; float lR0; float lR1; float lT0;

//___________________________________________________________________________________________________________________
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VOID SETUP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void setup() {
  Serial.begin(115200);
//___________________________________________________________________________________________________________________
//                                   ЧАСЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  if (!rtc.begin()) {       // проверка наличия модуля на линии i2c
    Serial.println("DS3231 not found");
    for(;;);
  }

  rtc.setTime(COMPILE_TIME); // установит дату и время, равное времени компиляции программы
}

//___________________________________________________________________________________________________________________
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VOID LOOP<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void loop() {
    ra_dec();
    julian_date();
    convert_LocalTime_to_LST();
    hour_angle();
    equatorial_to_horizontal();
    Print();
    delay(10000);                  // ПЕРИОД РАССЧЕТА НОВЫХ ДАННЫХ ОБСЕРВАТОРИИ 10 СЕКУНД
}

//___________________________________________________________________________________________________________________
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>VOID YIELD<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void yield() {
//___________________________________________________________________________________________________________________
//                                   ЭНКОДЕРЫ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  enc1.tick();
  enc2.tick();

  if (enc1.turn()) {
    if (enc1.right()) {
      Dec -= 10;
      if (Dec < -5399) {
        Dec = 5400;
      }
    }
    if (enc1.left()) {
      Dec += 10;
      if (Dec > 5400) {
        Dec = -5399;
      }
    }
    Serial.print("Dec: "); Serial.println(abs(float(Dec / 60.0)));
  }

  else if (enc2.turn()) {
    if (enc2.right()) {
      Ra += 10;
      if (Ra > 2699) {
        Ra = 0;
      }
    }
    if (enc2.left()) {
      Ra -= 1;
      if (Ra < -2699) {
        Ra += 10;
      }
    }
    Serial.print("Ra: "); Serial.println(Ra / 60);
  }
}


void ra_dec() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕТ ЧАСЫ RA И РАДИАНЫ DEC
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  Ra_h = abs(Ra) * 0.016666667;
  Dec_rad = abs(Dec) * 0.000291666673;
}

void julian_date() {
//___________________________________________________________________________________________________________________
//                                   ВЫЧИСЛЯЕМ ДАТУ ПО ЮЛИАНСКОМУ КАЛЕНДАРЮ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  if (month <= 2) {
    year -= 1;
    month += 12;}
  A = floor(year / 100);
  B = 2 - A + floor(A / 4);
  JD = floor(365.25*(year+4716))+ floor(30.6001*(month+1))+date+(0.04*hour)+B-1524.5; // с часами ТОЧНЕЕ
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
  HA_1 = {normalize0to24(pGST+(Longit_rad / 15.0))};
  HA = {normalize0to24(HA_1 - Ra_h)};
}
void equatorial_to_horizontal() {
  if (Ra == 0 && Dec == 0){
    pAzHor = 0;
    pAltHor = 0;
  }
  else {

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
}
float normalize0to24(float x) {
  while (x > 24.0){
    x -= 24.0;}
  while (x < 0) {
    x += 24.0;}
  return x;
}

//___________________________________________________________________________________________________________________
//                                   ВЫВОД
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void Print() {
  Serial.print("Дата : "); Serial.print(date); Serial.print("."); Serial.print(month); Serial.print("."); Serial.println(year);
  Serial.print("Время: "); Serial.print(hour); Serial.print(":"); Serial.println(minute);
  Serial.print("Ra: "); Serial.println(Ra_h);
  Serial.print("Dec: "); Serial.println(Dec_rad);
//  Serial.print("Долгота: "); Serial.println(Longit_rad);
//  Serial.print("Широта: "); Serial.println(Lat_rad);
//  Serial.print("Юлиан. кал.: "); Serial.println(JD);
//  Serial.print("lT: "); Serial.println(lT);
//  Serial.print("lR0: "); Serial.println(lR0);
//  Serial.print("lR1: "); Serial.println(lR1);
//  Serial.print("lT0: "); Serial.println(lT0);
//  Serial.print("pUT: "); Serial.print(pUT);Serial.print("     "); Serial.print(int(pUT));Serial.print(":"); Serial.println(round((pUT - int(pUT))*60));
//  Serial.print("pGST: "); Serial.print(pGST); Serial.print("     "); Serial.print(int(pGST));Serial.print(":"); Serial.println(round((pGST - int(pGST))*60));
//  Serial.print("pLST (дес): "); Serial.print(pLST); Serial.print("     "); Serial.print(int(pLST)); Serial.print(":"); Serial.println(round((pLST - int(pLST))*60));
//  Serial.print("LocalTime (дес): "); Serial.print(pLocalTime); Serial.print("     "); Serial.print(int(pLocalTime)); Serial.print(":"); Serial.println(round((pLocalTime - int(pLocalTime))*60));
//  Serial.print("Часовой угол: "); Serial.print(HA);  Serial.print("     "); Serial.print(int(HA));Serial.print(":"); Serial.println((HA - int(HA))*60);
//  Serial.print("AzEq: "); Serial.print(AzEq); Serial.println("радианы");
  Serial.print("Азимут: "); Serial.print(Az); Serial.println("градусы");
  Serial.print("Высота: "); Serial.print(Alt); Serial.println("градусы");
  Serial.println("  ");
}
