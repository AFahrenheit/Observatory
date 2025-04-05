Для подключения 4-значного 7-сегментного дисплея удобно использовать TM1637.

Пример кода:

#include<TM1637Display.h>
#define CLK 3 // D3 Arduino
#define DIO 2 // D2 Arduino

TM1637Display display(CLK, DIO);

void setup() {
    display.setBrightness(7);   // Настройка яркости
    display.showNumberDec(1234);    // Что будем отображать
    }

void loop() {

    }