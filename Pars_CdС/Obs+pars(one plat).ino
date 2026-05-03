
#define EB_DEB_TIME 50      // таймаут гашения дребезга кнопки (кнопка)

//                                     МОТОР
#include <GyverStepper2.h>
GStepper2<STEPPER2WIRE> stepper(200, 5, 4, 3);                      // 200 драйвер steps, +pul, +dir, +enable

// Если звездочка 60мм диаметра (дл.ок. 0,1884м), а упорное кольцо 2,42м (дл.ок 8,8м), то
// выходит звездочка делает 46,7 оборотов за 1 полный круг купола,
// редуктор 36 к 1, 200 шагов на оборот мотора.

int32_t incoming = 0;

//                                     ДЖОЙСТИК
#include <GyverJoy.h>
GyverJoy jx(2);   // джойстик на пине A2 (указываются пины АЦП)
#include <EncButton.h>
Button button(2);
bool count_click = 0;                                  // счётчик нажатий кнопки джойстика
uint8_t count_sec = 0;                                 // счётчик секунд бездействия

bool count = 0;                                       // счётчик включения датчика темп/вл
int16_t joy_speed_value = 2000; // скорость вращения мотора с джойтика                (ТРОГАЛ)

//                                     Wi-fi

#include <ESP8266WiFi.h>
#include <time.h>
#include <math.h>
#include <SPI.h>                                                     // Подключаем библиотеку SPI
//#include <nRF24L01.h>                                                // Подключаем библиотеку nRF24L01
#include <RF24.h>                                                    // Подключаем библиотеку RF24
// Данные сети Wi-Fi для подключения
const char* WIFI_SSID = "Mordor";
const char* WIFI_PASS = "19734682spg";
// Указываем тут IP IPv4 (win+R -> cmd -> ipconfig) ПК, к которому соединяемся
// Те же настройки должны быть в CdC (Настройка -> Главное -> Сервер)
const char* CDC_IP = "192.168.0.102"; // стационарный
//const char* CDC_IP = "192.168.0.105"; // ноут (ipconfig IPv4)
const uint16_t CDC_PORT = 4030;
WiFiClient client;
// Координаты обсерватории
const double OBS_LAT = 54.703164;  // +N
const double OBS_LON = 37.911196;  // +E

//                                   ПОВОРОТ КУПОЛА
float last_position;   // последняя позиция
float turn_R; float turn_L;
float pos;
int8_t count;
double az;

void connectToCDC() {
  while (!client.connected()) {
    //Serial.println("Connecting to CdC...");
    client.connect(CDC_IP, CDC_PORT);
    delay(2000);
  }
  //Serial.println("✅ Connected to CdC");
}

String sendLX200(const char* cmd) {
  client.print(cmd);
  delay(200);
  String r = "";
  while (client.available()) r += (char)client.read();
  r.trim();
  r.replace("#","");
  return r;
}

String extractRA(String raw) {
  int lastLine = raw.lastIndexOf(">"); // ищем последнюю строку
  if (lastLine < 0) return "";
  String line = raw.substring(lastLine);
  int hIdx = line.indexOf("h");
  int mIdx = line.indexOf("m", hIdx);
  int sIdx = line.indexOf("s", mIdx);
  if (hIdx<0 || mIdx<0 || sIdx<0) return "";
  return line.substring(hIdx-2, sIdx+1); // hh:mm:ss.s
}

String extractDEC(String raw) {
  int lastLine = raw.lastIndexOf(">");
  if (lastLine < 0) return "";
  String line = raw.substring(lastLine);
  int plus = line.indexOf("+");
  int minus = line.indexOf("-");
  int idx = (plus>0)? plus : minus;
  if (idx<0) return "";
  int degIdx = line.indexOf("°", idx);
  int minIdx = line.indexOf("'", degIdx);
  int secIdx = line.indexOf('"', minIdx);
  if (degIdx<0 || minIdx<0 || secIdx<0) return "";
  return line.substring(idx, secIdx+1); // +dd°mm'ss.s
}

double raToDeg(String ra) {
  int h = ra.substring(0,2).toInt();
  int m = ra.substring(3,5).toInt();
  double sec = ra.substring(6).toFloat();
  return (h + m/60.0 + sec/3600.0) * 15.0;
}

double decToDeg(String dec) {
  int sign = (dec[0]=='-'? -1 : 1);
  int degEnd = dec.indexOf('°');
  int minEnd = dec.indexOf('\'');
  int secEnd = dec.indexOf('"');
  int d = dec.substring(1, degEnd).toInt();
  int m = dec.substring(degEnd+1, minEnd).toInt();
  double s = dec.substring(minEnd+1, secEnd).toFloat();
  return sign * (d + m/60.0 + s/3600.0);
}

double julianDate() {
  time_t t = time(nullptr);
  return t/86400.0 + 2440587.5;
}

double getLST() {
  double JD = julianDate();
  double T  = (JD - 2451545.0)/36525.0;

  double GMST = 280.46061837 + 360.98564736629*(JD - 2451545.0) + 0.000387933*T*T - T*T*T/38710000.0;

  GMST = fmod(GMST, 360.0);
  if (GMST < 0) GMST += 360.0;

  double LST = GMST + OBS_LON;
  if (LST >= 360.0) LST -= 360.0;
  return LST;
}

double calcAz(double raDeg, double decDeg) {
  double H = getLST() - raDeg;
  if (H < 0) H += 360;
  if (H >= 360) H -= 360;

  double Hrad = radians(H);
  double decRad = radians(decDeg);
  double latRad = radians(OBS_LAT);

  double sinAlt = sin(decRad)*sin(latRad) + cos(decRad)*cos(latRad)*cos(Hrad);
  double altRad = asin(sinAlt);

  double sinAz = -cos(decRad)*sin(Hrad)/cos(altRad);
  double cosAz = (sin(decRad) - sin(latRad)*sinAlt)/(cos(latRad)*cos(altRad));

  double az = degrees(atan2(sinAz, cosAz));
  if (az < 0) az += 360;
  return az;
}

void getAZ() {
  if (!client.connected()) connectToCDC();

  String raw = sendLX200(":GR#");
  String raS = extractRA(raw);
  String deS = extractDEC(raw);

  if (raS.length()==0 || deS.length()==0) {
    //Serial.println("No data");
    delay(1000);
    return;
  }

  double ra = raToDeg(raS);
  double dec= decToDeg(deS);
  double az = calcAz(ra, dec);

  // Вывод отладочный
  //Serial.print("RA  = "); Serial.println(raS);
  //Serial.print("DEC = "); Serial.println(deS);
  //Serial.print("AZ  = "); Serial.println(az,2);
  //Serial.println("--------------------");
  delay(500);

  //Serial.print("az посчитан: "); Serial.println(az, 2);

//                                   ПОВОРОТ КУПОЛА
  turn_L = last_position - az;   // вычисление шагов вЛево
  turn_R = az - last_position;   // вычисление шагов вПраво
  pos = min(turn_L, turn_R);     // выбор наименьшего пути право/лево

  if (pos == 0.0) {
  }

  else if (az > last_position and abs(pos) < 180.0) {
    data[0] = fabs(pos) * 3096;//  *2 для счёта и *(43)обороты редуктора на купол    передаём шаги с редукцией
    if (data[0] != 0) {
      //Serial.print("1 Еду ПРАво"); Serial.print(abs(pos)); Serial.println("шагов");
      mot_work();
    }
  }

  else if (turn_R < -180.0) {
    pos = 360.0 + (turn_R);
    data[0] = fabs(pos) * 3096;//  *2 для счёта и *(43)обороты редуктора на купол    передаём шаги с редукцией
    if (data[0] != 0.0) {
      //Serial.print("2 Еду ПРАво"); Serial.print(abs(pos)); Serial.println("шагов");
      mot_work();
    }
  }

  else if (abs(pos) >= 180.0){
    pos += 359.0;
    if (pos > 1) {
      pos *= -1;
    }
    data[0] = pos * 3096;//  *2 для счёта и *(43)обороты редуктора на купол    передаём шаги с редукцией
    if (data[0] != 0.0) {
      //Serial.print("3 Еду levo"); Serial.print(pos); Serial.println("шагов");
      mot_work();
    }
  }

  else if (abs(turn_R) > 180.0) {
    if (turn_R < -180.0){
      pos = 360.0 + turn_R;
    }
    data[0] = fabs(pos) * 3096;//  *2 для счёта и *(43)обороты редуктора на купол    передаём шаги с редукцией
    if (data[0] != 0.0) {
      //Serial.print("4 Еду ПРАво"); Serial.print(abs(pos)); Serial.println("шагов");
      mot_work();
    }
  }

  else if (turn_R > 180.0){
    pos = turn_R;
    data[0] = pos * 3096;//  *2 для счёта и *(43)обороты редуктора на купол    передаём шаги с редукцией
    if (data[0] != 0.0) {
      //Serial.print("5 Еду ПРАво"); Serial.print(pos); Serial.println("шагов");
      mot_work();
    }
  }

  else {
    data[0] = pos * 3096;//  *36 шагов/градус *2 для счёта и *(43)обороты редуктора на купол    передаём шаги с редукцией
    if (data[0] != 0.0) {
      //Serial.print("6 Еду levo"); Serial.print(pos); Serial.println("шагов");
      mot_work();
    }
  }
  delay(1000);
}

void mot_work(); {
    stepper.tick();
    if (stepper.ready()) {
        incoming = - data[0];                              //   шаги
        stepper.setTargetDeg(incoming, RELATIVE);          // шаги
        last_position = az;
    }
}

void setup() {

//                                   VOID SETUP
  Serial.begin(115200);
  //stepper.setAcceleration(300);                        // ускорение (умолчание 300) (ТРОГАЛ)
  stepper.autoPower(true);                             // автовыключение питания драйвера
  //stepper.setMaxSpeed(2500);         // установка макс. скорости в шагах/сек        (ТРОГАЛ)
  //  stepper.disable();

//                                     Wi-fi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while(WiFi.status()!=WL_CONNECTED) delay(500);

  configTime(0,0,"pool.ntp.org","time.nist.gov");
  while(time(nullptr)<1700000000) delay(500);

// ---------------- SPI (ВАЖНО для ESP8266) ----------------
  SPI.begin();  // SCK=14, MISO=12, MOSI=13 по умолчанию


//                                     ДЖОЙСТИК
  jx.calibrate();   // калибровка нуля при запуске
  jx.deadzone(30);  // мёртвая зона
  //Serial.println("3 Калибровка джойстика");
  //jx.exponent(GJ_SQUARE);  // экспонента для плавности

  connectToCDC();
}

void task_joy() {
//                                   КОД ДЖОЙСТИКА
  button.tick();                       // опрос кнопки
  //Serial.println("6 Проверка джойстика");

  if (button.click()){
    radio.stopListening();
    count_click += 1;
    //Serial.println(F("Джойстик вкл."));
    while (count_click == 1){
      stepper.tick();
      jx.tick();
      //Serial.println(jx.value());
      if (jx.value() > 1) {
        count_sec = 0;
        stepper.setSpeed(joy_speed_value);
        //Serial.println(F("Влево!"));     // включение движения мотора влево
      }
      else if (jx.value() < -1) {
        count_sec = 0;
        stepper.setSpeed(- joy_speed_value);
        //Serial.println(F("Вправо!"));    // включение движения мотора вправo
      }
      else {
        stepper.stop();
        //Serial.print("Стоп");
        delay(1000);
        count_sec += 1;                // включение счётчика при бездействии
        //Serial.println(count_sec);

        if (count_sec > 1) {        // выключение пульта
          //Serial.println(F("Джойстик выкл."));
          count_click = 0;
          count_sec = 0;
//          stepper.disable();
          radio.startListening();
          break;
        }
      }
    }
  }
}

void loop() {
  getAZ();
  task_joy();
}


