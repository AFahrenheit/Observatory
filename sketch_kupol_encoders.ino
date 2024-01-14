#include <EncButton.h>
EncButton enc2(4, 5);
EncButton enc1(2, 3);

int Dec = 4800; //(4800тэ в 360°) начальное 90° (1200тэ = 90°, 13,3 ТЭ = 1°, 1тэ = 0,075°)
int Ra = 0;  // pLST начальное 1440 минут в 24 часах (1200тэ = 90° = 6час, 3,33 ТЭ = 1 мин, 1тэ = 18сек)
float h; float m;


void setup() {
  Serial.begin(115200);
}

void loop() {
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
      if (Dec > 4800) {
        Dec = -4799;
      }
    }
    //Serial.print("Dec: "); Serial.println(abs(Dec));
    //Serial.print("Dec мин: ");Serial.println((90.0 - ((4800.0 - abs(Dec)) / 13.3)) *  60);
    //Serial.print("Dec гр: ");Serial.println(90.0 - ((4800.0 - abs(Dec)) / 13.3));
  }

  if (enc2.turn()) {
    if (enc2.left()) {
      Ra += 1;
      if (Ra > 1399) {
        Ra = 0;
      }
    }
    if (enc2.right()) {
      Ra -= 1;
      if (Ra < -1399) {
        Ra += 1;
      }
    }
    // Вывод
    m = (1440 - (Ra / 3.33));  //  -(pLST)  (!!!!!!!!!)
    h = m /60;
    //Serial.print("Ra: "); Serial.println(Ra);
    Serial.print("Ra мин: "); Serial.println(int(m)%1440);
    Serial.print("Ra час: "); Serial.print(int(h) % 24); Serial.print(","); Serial.println(int(int(h*100) %100 * 0.6) );
    Serial.println(" ");
  }
}