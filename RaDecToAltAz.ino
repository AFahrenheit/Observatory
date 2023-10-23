
//------------------------
//=========ЧАСЫ===========
//------------------------
#include <microDS3231.h>
MicroDS3231 rtc;

//-----------------------
//========ЭНКОДЕР========
//-----------------------
#include <EncButton.h>
EncButton enc1(2, 3);

int Ra = 318;        //п.в.    14h15m(855)    1h09m(69)     17h44m(1064)
int Dec = 2761;      //склон.  19gr03(1143)  35gr37(2137)    4gr33(273)
int8_t  Hours = 3;       //часы       19             23              23
int8_t  Minutes = 39;    //мин.       12             15              15
int8_t pDay = 21;       //день       12             19              5
int8_t pMonth = 4;      //месяц      10             8               6
int pYear = 2022;    //год       2023           2023            2022
float Ra_h;
float Dec_rad;              //склонение в радианах (радианы * 0.0175)

float pUT;       // Всемир. вр. (16:12)       (20:15)         (20:15)
float pGST;      // Зв.вр.Gr    (20:08)       (20.38)         (15.44)
float JD;//Юлиан. календарь     2460230.     2460176,34     2459736.50

float Longit_rad = 37.91111 * 0.0175;  // долгота 37'54'40 (радианы)
float Lat_rad = 54.703 * 0.0175;   // широта 54'42'11  (радианы)

float pLocalTime;
int pDs = 0;         // Переход на летнее время (0 \ 1)
uint8_t pTz = 3;         // Часовой пояс
float pLST;

float hourAngel; float hourAngel_1;

float AzEq; float pAzHor; float pAltHor;

float Az; float Alt;                // азимут

float A; float B;
float lT; float lR0; float lR1; float lT0;

void setup() {
  Serial.begin(115200);
}
void loop() {
  enc1.tick();
  if (enc1.turn()) {

    Ra_h = Ra * 0.016667;
    Dec_rad = abs(Dec/60) * 0.0175;                    // радианы

    julian_date();
    convert_LocalTime_to_LST();
    hour_angle();
    equatorial_to_horizontal();
    Print();
  }
}
void julian_date() {

                // ВЫЧИСЛЯЕМ ДАТУ ПО ЮЛИАНСКОМУ КАЛЕНДАРЮ
  if (pMonth <= 2) {
    pYear -= 1;
    pMonth += 12;}
  A = floor(pYear / 100);
  B = 2 - A + floor(A / 4);
  JD = floor(365.25*(pYear+4716))+ floor(30.6001*(pMonth+1))+pDay+(0.04*Hours)+B-1524.5; // с часами ТОЧНЕЕ
}
void convert_LocalTime_to_LST() {

            // ВЫЧИСЛЯЕМ ВСЕМИРНОЕ ВРЕМЯ ПО ГРИНВИЧУ
  lT = (JD / 36525.0) - 1;
  lR0 = lT * (0.0513366 + lT * (0.00002586222 - lT * 0.000000001722));
  lR1 = 6.697374558 + 2400.0 * (lT - ((pYear - 2000.0) / 100.0));          // ПРОБЛЕМА С ТОЧНОСТЬЮ!!!
  lT0 = lR0+lR1;
  normalize0to24(lT0);

        // ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ
  pLocalTime = Hours + (0.016 * Minutes);     // время в десятичной форме
  pUT = pLocalTime - pDs - pTz;               //ОРИГИНАЛ
  normalize0to24(pUT);

        // ВЫЧИСЛЯЕМ ЗВЕЗДНОЕ ВРЕМЯ ПО ГРИНВИЧУ
  pGST = (pUT * 0.997269625) + lT0 -0.11;
  normalize0to24(pGST);

        // ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ (LST)
  pLST = pGST + (Longit_rad / 15.0);
  normalize0to24(pLST);
}
void hour_angle() {

         // ВЫЧИСЛЯЕМ ЧАСОВОЙ УГОЛ (hourAngel)
  hourAngel_1 = pGST+(Longit_rad / 15.0);
  normalize0to24(hourAngel_1);
  hourAngel = hourAngel_1 - Ra_h;
  normalize0to24(hourAngel);
}
void equatorial_to_horizontal() {

         // ВЫЧИСЛЯЕМ АЗИМУТ И ВЫСОТУ
  AzEq = (hourAngel * 15.0) * 0.0175;               // радианы
  pAltHor = asin((sin(Dec_rad) * sin(Lat_rad)) + (cos(Dec_rad) * cos(Lat_rad) * cos(AzEq)));
  pAzHor = acos((sin(Dec_rad) - (sin(Lat_rad) * sin(pAltHor))) / (cos(Lat_rad) * cos(pAltHor)));
  if (sin(AzEq) > 0) {
    pAzHor = 2 * PI - pAzHor;}
  Az = pAzHor * 57.3;
  Alt = pAltHor * 57.3;
}
void normalize0to24(float(x)) {
  while (x > 24.0){
    x -= 24.0;}
  while (x < 0) {
    x += 24.0;}
  return;
}
void Print() {
  Serial.print("Долгота: "); Serial.println(Longit_rad);
  Serial.print("Широта: "); Serial.println(Lat_rad);
  Serial.print("Прямое. восх: "); Serial.println(Ra_h);
  Serial.print("Склонение гр.:"); Serial.println(Dec_rad);
  Serial.print("Юлиан. кал.: "); Serial.println(JD);
  Serial.print("lT: "); Serial.println(lT);
  Serial.print("lR0: "); Serial.println(lR0);
  Serial.print("lR1: "); Serial.println(lR1);
  Serial.print("lT0 начальное: "); Serial.println(lT0);
  Serial.print("lT0 итоговое: "); Serial.println(lT0);
  Serial.print("pLocalTime: "); Serial.println(pLocalTime);
  Serial.print("pUT: "); Serial.print(pUT);Serial.print("     "); Serial.print(int(pUT));Serial.print(":"); Serial.println(round((pUT - int(pUT))*60));
  Serial.print("pGST: "); Serial.print(pGST); Serial.print("     "); Serial.print(int(pGST));Serial.print(":"); Serial.println(round((pGST - int(pGST))*60));
  Serial.print("pLST: "); Serial.print(pLST); Serial.println(" часов");// Serial.print("         "); Serial.print(pLST_h); Serial.print(":"); Serial.println(pLST_m);
  Serial.print("Часовой угол_1: "); Serial.println(hourAngel_1);
  Serial.print("Часовой угол: "); Serial.println(hourAngel);
  Serial.print("AzEq: "); Serial.print(AzEq); Serial.println("радианы");
  Serial.print("Азимут: "); Serial.print(Az); Serial.println("градусы");
  Serial.print("Высота: "); Serial.print(Alt); Serial.println("градусы");
  Serial.println("  ");
}