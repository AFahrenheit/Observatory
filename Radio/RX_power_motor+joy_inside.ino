//____________________________________________________________________________________________
//                                 БИБЛИОТЕКИ РАДИО
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <SPI.h>                                       // Подключаем библиотеку SPI
#include <nRF24L01.h>                                  // Подключаем библиотеку nRF24L01
#include <RF24.h>                                      // Подключаем библиотеку RF24
//____________________________________________________________________________________________
//                               БИБЛИОТЕКИ ДАТЧИКОВ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"                            // датчик темп и влажности
//____________________________________________________________________________________________
//                          ПЕРЕМЕННЫЕ/ПОДКЛЮЧЕНИЕ ДАТЧИКОВ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
Adafruit_SHT31 sht31 = Adafruit_SHT31();               // датчик темп и влажности
int t;                                               // Целое число для температуры
int h;                                               // Целое число для влажности
int ackData[2];                             // Массив передаваемых данных
//____________________________________________________________________________________________
//                                     МОТОР
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <GyverStepper2.h>
//GStepper2<STEPPER2WIRE> stepper(200, 5, 4, 3);                      // *100 драйвер steps, +pul, +dir, +enable
GStepper2<STEPPER2WIRE> stepper(20000, 5, 4, 3);                      // *100 драйвер steps, +pul, +dir, +enable
//____________________________________________________________________________________________
//                                     РАДИО
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
int32_t data[1];                                           // Массив для приёма данных
RF24 radio(9, 10);                                     // Указываем номера выводов nRF24L01+ (CE, CSN)
//____________________________________________________________________________________________
//                                     ДЖОЙСТИК
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
#include <GyverJoy.h>
GyverJoy jx(2);   // джойстик на пине A2
#include <EncButton.h>
EncButton<EB_TICK, 2> button(INPUT_PULLUP);
bool count_click = 0;                                  // счётчик нажатий кнопки джойстика
uint8_t count_sec = 0;                                 // счётчик секунд бездействия

//____________________________________________________________________________________________
//                                   VOID SETUP
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
bool count = 0;                                       // счётчик включения датчика темп/вл


void setup() {
    Serial.begin(9600);
//    stepper.setAcceleration(300);                        // ускорение (умолчание 300)
    stepper.autoPower(true);                             // автовыключение питания драйвера
//    stepper.disable();
//____________________________________________________________________________________________
//                                   РАБОТА РАДИО
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
    delay(2000);                                               // Ждем 2с
    radio.begin();                                             // Инициируем работу nRF24L01+
    radio.setChannel(120);                                     // Указываем канал передачи (от 0 до 126)
    radio.setRetries(10, 50);                                  // Кол-во попыток и время между попытками
    radio.setDataRate (RF24_1MBPS);                            // Указываем скорость передачи (250KBPS, 1MBPS, 2MBPS)
    radio.setPALevel(RF24_PA_HIGH);                            // Указываем мощность передатчика (MIN=-18dBm, LOW=-12dBm, HIGH=-6dBm, MAX=0dBm)
    radio.openReadingPipe(1, 0xF0F0F0F066);                    // Идентификатор для получения данных
    radio.openWritingPipe(0xF0F0F0F166);                       // Идентификатор для отправки данных
    radio.startListening();                                    // Начать прослушивание эфира
//--------------------------------------------------------------------------------------------
//                        НАСТРОЙКА/КАЛИБРОВКА ДАТЧИКОВ
//--------------------------------------------------------------------------------------------
  while (!Serial)
    delay(10);                                               // Задержка для теста
//  Serial.print("Тест");                           // ПРИНТ ЭТОГО ТРИППЕРА
  if (! sht31.begin(0x44)) {                                 // 0x45 для альтернативного i2c адрес
//    Serial.println("Couldn't find SHT31");        // ПРИНТ ЭТОГО ТРИППЕРА
    while (1) delay(1);
  }
//____________________________________________________________________________________________
//                                     ДЖОЙСТИК
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
  jx.calibrate();   // калибровка нуля при запуске
  jx.deadzone(30);  // мёртвая зона
//  jx.exponent(GJ_SQUARE);  // экспонента для плавности
}
//____________________________________________________________________________________________
//                                   VOID LOOP
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void loop() {
    task_motor();
    task_temp_hum();
    task_joy();
}

//____________________________________________________________________________________________
//                                   КОД МОТОРА + РАДИО
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void task_motor() {
//    Serial.println("Мотор.");
    stepper.tick();                                            // Проверка состояния мотора

    if (radio.available()) {                                   // Если в буфере поступили данные
//        stepper.enable();                       //двигатель включен
//        Serial.println(F("Поехали"));
        radio.read(data, 1);                                     // Считываем данные с массива data и задаем количество считываемых байт

        switch (data[0]) {                                       // Если присланый массив data[0] состоит.....
            case "L": stepper.setSpeed(2000);                      // 9 Поворачиваем двигатель влево
//                delay(20);
                break;                                               // брейк
            case "R": stepper.setSpeed(-2000);                     // 5 Поворачиваем двигатель вправо
//                delay(20);
                break;                                               // брейк
            case "S": stepper.stop();                                // 1 Останавливаем двигатель
                delay(20);
//                stepper.disable();
                count += 1;                                          // счёт включения отправки данных датчика
                break;                                               // брейк

            case int: stepper.setTargetDeg(data[0], ABSOLUTE);      // ПОВОРОТ на turn_deg
                Serial.print("Массив данных: "); Serial.println(data[0]);     // отправляю float*100
                break;                                               // брейк

        }
    }
}

//____________________________________________________________________________________________
//                      КОД СБОРА И ОТПРАВКИ ДАННЫХ С ДАТЧИКОВ
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void task_temp_hum() {
    if (count == 1) {
        t = sht31.readTemperature();                                    // температура в массив
        h = sht31.readHumidity();                                    // влажность в массив
        radio.stopListening();
//        Serial.println(F("Готовлюсь отправлять"));                 // ПРИНТ ЭТОГО ТРИППЕРА
        ackData[0] = t;                                     // температура в массив
        ackData[1] = h;                                    // влажность в массив
//        Serial.print("Температура  : "); Serial.println(ackData[0]);                    // ПРИНТ ЭТОГО ТРИППЕРА
//        Serial.print("Влажность    :   "); Serial.println(ackData[1]);                    // ПРИНТ ЭТОГО ТРИППЕРА
//        Serial.print("Занимает байт:    "); Serial.println(sizeof(ackData));
        radio.write(ackData, 4);            // ответочка с данными массива
//        delay(20);                                         // Задержка в 0.02сек
//        Serial.println(F("Отправлено!"));                 // ПРИНТ ЭТОГО ТРИППЕРА
        count = 0;
        radio.startListening();
    }
}

//____________________________________________________________________________________________
//                                   КОД ДЖОЙСТИКА
//‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
void task_joy() {
    button.tick();                       // опрос кнопки
    if (button.click()){
        radio.stopListening();
//        stepper.enable();
        count_click += 1;
//        Serial.println(F("Джойстик вкл."));
        while (count_click == 1){
            stepper.tick();
            jx.tick();
//            Serial.println(jx.value());
            if (jx.value() > 1) {
                count_sec = 0;
                stepper.setSpeed(2000);
//                Serial.println(F("Влево!"));     // включение движения мотора влево
            }
            else if (jx.value() < -1) {
                count_sec = 0;
                stepper.setSpeed(-2000);
//                Serial.println(F("Вправо!"));    // включение движения мотора вправo
            }
            else {
                stepper.stop();
//                Serial.print("Стоп");
                delay(1000);
                count_sec += 1;                // включение счётчика при бездействии
//                Serial.println(count_sec);

                if (count_sec > 10) {        // выключение пульта
//                    Serial.println(F("Джойстик выкл."));
                    count_click = 0;
                    count_sec = 0;
//                    stepper.disable();
//                    КОД ВКЛЮЧЕНИЯ РАДИО
                    radio.startListening();
                    break;
                }
            }
        }
    }
}