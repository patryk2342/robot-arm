#include <ESP32Servo.h>
  Servo Serwo_0;
const int max_kroki = 20;
int nagrywanie_ruch[max_kroki];

void ruch_lewo_prawo () {
  static int i = 1500;
  if (digitalRead(13) == LOW) {
    Serwo_0.writeMicroseconds(i);
    i = i + 10;
  }
  if (digitalRead(14) == LOW) {
    Serwo_0.writeMicroseconds(i);
    i = i - 10;
  }
} 
// zrobione
int nagrywanie() {
  static bool poprzedni_stan = HIGH;
  bool stan = digitalRead(16);
  static bool przytrzymanie = false;

  static unsigned long czas_uruchomienia_funkcji = 0;
  static int aktualne_kroki = 0;
  // kliknięcie przycisku
  if (poprzedni_stan == HIGH && stan == LOW) {
    czas_uruchomienia_funkcji = millis();
  }  
  // zapis pozycji
  if (poprzedni_stan == LOW && stan == HIGH) {
    if (aktualne_kroki < max_kroki && !przytrzymanie) {
      nagrywanie_ruch[aktualne_kroki] = Serwo_0.readMicroseconds();
      aktualne_kroki++;
    }
    przytrzymanie = false;
  }
  // reset wszystkich kroków
  if (stan == LOW && millis() - czas_uruchomienia_funkcji >= 3000 && !przytrzymanie) {
    przytrzymanie = true;
    Serial.println("Reset kroków");
    aktualne_kroki = 0;
    for (int i = 0; i <max_kroki; i++) {
      nagrywanie_ruch[i] = 0
    }
  }

  poprzedni_stan = stan;
  return aktualne_kroki;
}
// poprawa !!! Cloude AI
int odtwarzanie_2(int ilosc_krokow_do_wykodania) {
  bool stan = digitalRead(21); // przycisk
  static bool poprzedni_stan = HIGH;
  static bool stan_pracy = false;

  int szybkosc_pracy = 10;

  // przyciśnięcie przycisku do odtwarzania
  if (poprzedni_stan == HIGH && stan == LOW) {
    stan_pracy = !stan_pracy;
  }

  poprzedni_stan = stan;
  // odtwarzanie
  if (stan_pracy == true) {
    for (int i = 0; i != ilosc_krokow_do_wykodania; i++) {
      if (nagrywanie_ruch[i + 1] == 0) {
      stan_pracy = false;
      break;
      }
      int start_krok = nagrywanie_ruch[i];
      int koniec_krok = nagrywanie_ruch[i + 1]; 
      int dystans = start_krok - koniec_krok;
      // jeśli dystans jest na minusie serwo w lewo
      if (dystans < 0) {
        for (start_krok; start_krok < koniec_krok; start_krok = start_krok + szybkosc_pracy) {
          Serwo_0.writeMicroseconds(start_krok);
          Serial.println(start_krok);
          Serial.println(koniec_krok);
          Serial.println(stan_pracy);
          Serial.println(dystans);
          Serial.println();
          if (start_krok - koniec_krok >= szybkosc_pracy) {
          Serwo_0.writeMicroseconds(koniec_krok);
          break;
          }  
        }
      }
      // jeśli dystans jest na plusie serwo w prawo
      else if (dystans > 0) {
        for (start_krok; start_krok > koniec_krok; start_krok = start_krok - szybkosc_pracy) {
          Serwo_0.writeMicroseconds(start_krok);
          Serial.println(start_krok);
          Serial.println(koniec_krok);
          Serial.println(stan_pracy);
          Serial.println(dystans);
          Serial.println();
          if (start_krok - koniec_krok <= szybkosc_pracy) {
          Serwo_0.writeMicroseconds(koniec_krok); 
          break;
          }
        }
      }
    }
  }
  return 0;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serwo_0.attach(17, 600, 2300);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  int kat =1500;
  Serwo_0.setTimerWidth(12);
  Serwo_0.writeMicroseconds(kat);

}

void loop() {
  // put your main code here, to run repeatedly:
  int kroki_do_wykonania = nagrywanie();
  // Serial.println(nagrywanie_ruch[kroki_do_wykonania]);
  Serial.println(kroki_do_wykonania);
  // int aktualny_krok = odtwarzanie(kroki_do_wykonania);
  // Serial.println(aktualny_krok);
  Serial.println(odtwarzanie_2(kroki_do_wykonania));
  Serial.println(nagrywanie_ruch[0]);
  Serial.println(nagrywanie_ruch[1]);
  Serial.println(nagrywanie_ruch[2]);
  Serial.println();
  ruch_lewo_prawo();
  delay(20);
}
