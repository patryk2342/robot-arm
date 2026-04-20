const int joystick_klik = 32;
int osX;
int osY;
const int max_kroki = 30;
int zapis_pozycji[max_kroki][3];
// ustawianie tworzenie serwa
#include <ESP32Servo.h>
  Servo serwo_1;
  Servo serwo_2;
  Servo serwo_3;
// gałka lewo prawo
float joystick_lewo_prawo (int osX, int tryb_dzalania) {
  static int szybko = 10;
  static int wolno = 2;
  int lewo_prawo = osX;
  if (tryb_dzalania==1){
    if (lewo_prawo < 1600)
      return -szybko;
    else if (lewo_prawo < 2100 && lewo_prawo > 1600)
      return 0;
    else if (lewo_prawo > 2100)
      return szybko;
  }
  else{
    if (lewo_prawo < 1600)
      return -wolno;
    else if (lewo_prawo < 2100 && lewo_prawo > 1600)
      return 0;
    else if (lewo_prawo > 2100)
      return wolno;
  }
}
// gałka przód tył
float joystick_przud_tyl (int osY, int tryb_dzalania) {
  static int szybko = 10;
  static int wolno = 2;
  int przod_tyl = osY;
  if (tryb_dzalania==1){ 
    if (przod_tyl < 1600)
      return -szybko;
    else if (przod_tyl < 2100 && przod_tyl > 1600)
      return 0;
    else if (przod_tyl > 2100)
      return szybko;
  }
  else {
        if (przod_tyl < 1600)
      return -wolno;
    else if (przod_tyl < 2100 && przod_tyl > 1600)
      return 0;
    else if (przod_tyl > 2100)
      return wolno;
  }
}
// odczytywanie wartości przycisków
int osZ (int piny_od_dolu, int piny_od_gory) {
  if (digitalRead(piny_od_dolu) == LOW) return -1;
  if (digitalRead(piny_od_gory) == LOW)  return 1;
  else return 0;
}
//zmiana trybu poruszania fast/slow przez kliknięcie
int zmiana_szybkosci (int pin) {
  static int i = 0;
  static bool poprzedni_stan = HIGH;
  bool stan = digitalRead(pin);
  if (poprzedni_stan == HIGH && stan == LOW){
    i = 1 - i;
  }
  poprzedni_stan=stan;
  return i;
}
// poruszanie ramieniem robota przód tył
int serwo_ruch_osY (int osY,int tryb_dzalania) {
  static int poruszanie = 500;
  int ruch = joystick_przud_tyl(osY,tryb_dzalania);
  poruszanie = constrain(poruszanie + ruch, 500, 2450);
  return poruszanie;
}
// poruszanie ramieniem robota lewo prawo
int serwo_ruch_osX (int osX,int tryb_dzalania) {
  static int poruszanie = 500;
  int ruch = joystick_lewo_prawo(osX,tryb_dzalania);
  poruszanie = constrain(poruszanie + ruch, 500, 2450);
  return poruszanie;
}
// poruszenie ramieniem robota dół góra
int serwo_ruch_osZ (int piny_od_dolu ,int piny_od_gory) {
  static int poruszanie = 500;
  int ruch = 10 * osZ(piny_od_dolu ,piny_od_gory);
  poruszanie = constrain(poruszanie + ruch, 500, 2450);
  return poruszanie;
}
// nagrywanie
int nagrywanie_ruchu (int przycisk_nagrywanie_ruch) {
  static bool poprzedni_stan = HIGH;
  bool stan = digitalRead(przycisk_nagrywanie_ruch);
  static bool przytrzymanie = false;

  static unsigned long czas_wcisniecia = 0;
  static int aktualne_kroki = 0;
  //kliknięcie przycisku
  if (poprzedni_stan == HIGH && stan == LOW) {
    czas_wcisniecia = millis();
  }
  // zapis pozycji
  if (poprzedni_stan == LOW && stan == HIGH) {
    if (aktualne_kroki < max_kroki && !przytrzymanie) {
      zapis_pozycji[aktualne_kroki][0] = serwo_1.readMicroseconds();
      zapis_pozycji[aktualne_kroki][1] = serwo_2.readMicroseconds();
      zapis_pozycji[aktualne_kroki][2] = serwo_3.readMicroseconds();
      aktualne_kroki++;
    }
    przytrzymanie = false;
  }
  // reset wszystkich kroków
  if (stan == LOW && millis() - czas_wcisniecia >= 3000 && !przytrzymanie) {
    przytrzymanie = true;
    Serial.println("Reset wszystkich pozycji");
    aktualne_kroki = 0;
    for (int i = 0; i < max_kroki; i++) {
      zapis_pozycji[i][0] = 0;
      zapis_pozycji[i][1] = 0;
      zapis_pozycji[i][2] = 0;
    }
  }

  poprzedni_stan = stan;
  return aktualne_kroki;
}
// odtwarzanie
bool odczytywanie_ruchu (int przycisk_odtwarzanie_ruch, int ilosc_krokow_do_wykodania){
  bool stan = digitalRead(przycisk_odtwarzanie_ruch);
  static bool poprzedni_stan = HIGH;
  static bool stan_pracy = false;
  static int i = 0;
  static int aktualna_poz[3] = {0, 0, 0};

  int szybksc_pracy = 10;

  // toggle start/stop
  if (poprzedni_stan == HIGH && stan == LOW) {
    stan_pracy = !stan_pracy;
    if (stan_pracy) {
      i = 0;
      aktualna_poz[0] = zapis_pozycji[0][0];
      aktualna_poz[1] = zapis_pozycji[0][1];
      aktualna_poz[2] = zapis_pozycji[0][2];
    }
  }
  poprzedni_stan = stan;

  if (stan_pracy) {
    // koniec nagrania
    if (i >= ilosc_krokow_do_wykodania - 1 || (zapis_pozycji[i + 1][0] == 0 && zapis_pozycji[i + 1][1] == 0 && zapis_pozycji[i + 1][2] == 0)) {
      stan_pracy = false;
      i = 0;
      return stan_pracy;
    }

    bool wszystkie_na_miejscu = true;

    for (int s = 0; s < 3; s++) {
      int koniec = zapis_pozycji[i +1][s];

      if (aktualna_poz[s] < koniec) {
      aktualna_poz[s] += szybksc_pracy;
       if (aktualna_poz[s] > koniec) aktualna_poz[s] = koniec;
    }
      else if (aktualna_poz[s] > koniec) {
        aktualna_poz[s] -= szybksc_pracy;
        if (aktualna_poz[s] < koniec) aktualna_poz[s] = koniec;
      }

      if (s == 0) serwo_1.writeMicroseconds(aktualna_poz[s]);
      if (s == 1) serwo_2.writeMicroseconds(aktualna_poz[s]);
      if (s == 2) serwo_3.writeMicroseconds(aktualna_poz[s]);

      if (aktualna_poz[s] != koniec) wszystkie_na_miejscu = false;
    }

    if (wszystkie_na_miejscu) i++;
  }

  return stan_pracy;
}

//przygotowanie esp32
// przypisanie pina do serwa i inne
void setup() {
  Serial.begin(115200);
  pinMode(32, INPUT_PULLUP); // przycisk w joysticku
  pinMode(13, INPUT_PULLUP); // przycisk od dołu
  pinMode(14, INPUT_PULLUP); // przycisk od góry
  pinMode(16, INPUT_PULLUP); // przycisk od nagrywania ruchów kliknięcie to nagranie ruchu
  pinMode(21, INPUT_PULLUP); // przycisk od odtwarzania ruchów 
  serwo_1.attach(17 ,500 ,2450); // serwo 1
  serwo_2.attach(18 ,500 ,2450); // serwo 2
  serwo_3.attach(19, 500 ,2450); // serwo 3
}

void loop() {
  osX = analogRead(34);
  osY = analogRead(35);

  int tryb_dzalania = zmiana_szybkosci(joystick_klik); // szybko lub wolno
  int kroki = nagrywanie_ruchu(16);
  bool odtwarza = odczytywanie_ruchu(21, kroki);

  if (!odtwarza){
    int posX = serwo_ruch_osX(osX, tryb_dzalania);
    int posY = serwo_ruch_osY(osY, tryb_dzalania);
    int posZ = serwo_ruch_osZ(13,14);
    serwo_1.writeMicroseconds(posX);
    serwo_2.writeMicroseconds(posY);
    serwo_3.writeMicroseconds(posZ);
  }
  // Serial.println();
  // Serial.println(joystick_lewo_prawo(osX,tryb_dzalania));
  // Serial.println(joystick_przud_tyl(osY,tryb_dzalania));
  // Serial.println(tryb_dzalania);
  // Serial.println(osZ(13,14));
  // Serial.println(posX);
  // Serial.println(posY);
  // Serial.println(posZ);
  // Serial.println();

  // Serial.println(kroki);
  // Serial.println(zapis_pozycji[kroki - 1][0]);
  
  delay(20);
}