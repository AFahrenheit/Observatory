#include <EncButton.h>
EncButton enc2(14, 15); // (4, 5) a0a1 14, 15 (свободные a0 a1 a3 d6 d7 d8)
EncButton enc1(6, 7); // (2, 3) d6d7 6, 7

int Dec = 4800; //(4800тэ в 360°) начальное 90°      (1200тэ = 90°, 13,3 ТЭ = 1°)
int Ra = 0;  // pLST начальное 1440 минут в 24 часах (1200тэ = 90° = 6час, 3,33333 ТЭ = 1 мин)
float h; float m;
int hRa_plus; int hRa_minus;
int Ra_turn;
int Ra_last; int Dec_last;
int pLST;
int x;

void setup() {
  Serial.begin(115200);
}

void loop() {
  encoders();

  if (Ra != Ra_last or Dec != Dec_last) {
    calculation_Ra_Dec();
  }
}

void encoders() {
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

  hRa_plus = (x * 360 + int(abs(Ra)/ 3.33333)) % 1440; //  + pLST %24
  hRa_minus = (x * 360 - int(abs(Ra)/ 3.33333)) % 1440;  //  + pLST %24

    if (Ra > 0) {
      if (Dec < 0) {
        x = 1;
        Ra_turn = hRa_minus;// +LST
      }
      if (Dec > 0) {
        x = 3;
        Ra_turn = hRa_minus;// +LST
      }
    }
    if (Ra < 0) {
      if (Dec > 0) {
        x = 3;
        Ra_turn = hRa_plus;//  +LST
      }
      if (Dec < 0) {
        x = 1;
        Ra_turn = hRa_plus;//  +LST
      }
    }
    if (Ra == 0) {
      Ra_turn = 0;
    }

    Ra_last = Ra;
    Dec_last = Dec;

  // ВЫВОД
  //Serial.print("Dec: "); Serial.println(Dec);
  //Serial.print("Dec мин: ");Serial.println((90.0 - ((4800.0 - abs(Dec)) / 13.3)) *  60);
  Serial.print("Dec гр: ");Serial.println(90.0 - ((4800.0 - abs(Dec)) / 13.3));
  //Serial.println(" ");
  //Serial.print("Ra_turn: "); Serial.println(Ra_turn);
  //Serial.print("Ra: "); Serial.println(Ra);
  //Serial.print("Ra мин: "); Serial.println(int(m)%1440);
  //Serial.print("Ra час: "); Serial.print(int(m/60)%24); Serial.print(":"); Serial.println(int(m)%60);
  //Serial.print("Ra час: "); Serial.print(int(m/60)); Serial.print(":"); Serial.println(int(m)%60);
  Serial.print("Ra(t) час: "); Serial.print(Ra_turn/60);  Serial.print(":"); Serial.print(int(Ra_turn)%60); Serial.println("   +LST %24");
  Serial.println(" ");
}