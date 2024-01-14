#include <EncButton.h>
EncButton enc2(14, 15); // (4, 5) a0a1 14, 15 (свободные a0 a1 a3 d6 d7 d8)
EncButton enc1(6, 7); // (2, 3) d6d7 6, 7

int Dec = 4800; //(4800тэ в 360°) начальное 90° (1200тэ = 90°, 13,3 ТЭ = 1°, 1тэ = 0,075°)
int Ra = 0;  // pLST начальное 1440 минут в 24 часах (1200тэ = 90° = 6час, 3,33 ТЭ = 1 мин)
float h; float m;


void setup() {
  Serial.begin(115200);
}

void loop() {
  enc1.tick();
  enc2.tick();

  if (enc1.turn()) {
    //  ОДНА СТОРОНА +6, ДРУГАЯ -12
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
    Serial.print("Dec: "); Serial.println(Dec);
    //Serial.print("Dec мин: ");Serial.println((90.0 - ((4800.0 - abs(Dec)) / 13.3)) *  60);
    //Serial.print("Dec гр: ");Serial.println(90.0 - ((4800.0 - abs(Dec)) / 13.3));
  }

  if (enc2.turn()) {
    if (enc2.left()) {
      Ra += 1;
      if (Ra > 1199) {
        Ra = 0;
      }
    }
    if (enc2.right()) {
      Ra -= 1;
      if (Ra < -1199) {
        Ra += 1;
      }
    }
    // Вывод
    m = (1440.0 - (Ra / 3.33333));  //  (-pLST)  (!!!!!!!!!)
    h = m / 60;

    // ЕСЛИ:
    // Ra > 6ч:
    //     Dec < 0, то Ra (18ч-Ra+pLST)
    //     Dec > 0, то Ra ( 6ч-Ra+pLST)
    //
    // Ra < 6ч:
    //     Dec > 0, то Ra ( 18ч+Ra+pLST)
    //     Dec < 0, то Ra ( 6ч+Ra+pLST)

    //Serial.print("Ra: "); Serial.println(Ra);
    Serial.print("Ra мин: "); Serial.println(int(m)%1440);
    Serial.print("Ra час: "); Serial.print(int(h) % 24); Serial.print(","); Serial.println(int(int(h*100) %100 * 0.6) );
    Serial.println(" ");
  }
}