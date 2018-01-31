void sendkod(unsigned long code, int impuls, byte bity, byte prot);
long powtorzkod = 0;  //zmienna do powtrzen wysyałania kodu przez router
int powtorzimpuls = 0; //jw
byte powtorzbity = 0; //jw
byte powtorzprot = 0; //jw
//4797521
byte powtorz = 0; // liczba powtorzen wysylania kodu
unsigned int czas_odliczania = 0; 

byte szukaj_indeks_we(long kod) { // przeszukiwanie tablicy kodow wejsciowych
  byte     indekswy = 0;
  for (int i = 1; i < (rozm_tab_we + 1); i++) if (kod == tablica_adres_we[i].kod) indekswy = i;
  return indekswy;
}

void powtorz_send() { // powtarzanie wysylania kodu
  if (powtorz) {
    sendkod(powtorzkod, powtorzimpuls, powtorzbity, powtorzprot);
    //    Serial.println("powtarzam send  " + String(powtorz));
    powtorz--;
  }
}
void router_timer() {
  powtorz_send();
}void send_indeks_wy(byte ind) { //procedura wysyłania kodu  z tablicy kodów wyjściowych 
  powtorzkod = tablica_adres_wy[ind].kod;
  powtorzimpuls = tablica_adres_wy[ind].impuls;
  powtorzbity = tablica_adres_wy[ind].bity;
  powtorzprot = tablica_adres_wy[ind].prot;
  powtorz = 2;
  sendkod(tablica_adres_wy[ind].kod, tablica_adres_wy[ind].impuls, tablica_adres_wy[ind].bity, tablica_adres_wy[ind].prot);
  if (printon) {
    Serial.println ("index wy  " + String (ind) + "  kod wy  " + String (tablica_adres_wy[ind].kod));
  }
  if (terminalon) {
    terminal.println("index wy  " + String (ind) + "  kod wy  " + String (tablica_adres_wy[ind].kod));
    terminal.flush();
  }
}

BLYNK_WRITE(V28) {//stan klawisza send kod 
  if (param.asInt())send_indeks_wy(wpis_funkcji.kodwy);
  Blynk.virtualWrite (V30, "select", wpis_funkcji.kodwy);

}

void timer10sek() {//funkcja odliczająca czasy ustawione w tabeli funkcji
  for (int i = 1; i < (rozm_tab_fun); i++ )
  {
    if (tablica_adres_fun[i].funkcja == wlsekwyl) {
      if (tablica_adres_fun[i].stan > 0) {
        tablica_adres_fun[i].stan--;
        if (tablica_adres_fun[i].stan == 0) {
          send_indeks_wy(tablica_adres_fun[i].kodwy);
        }
        String s = String ("we " + String (tablica_adres_fun[i].kodwe) + " wy " + String (tablica_adres_fun[i].kodwy) + " F " + String (tablica_adres_fun[i].funkcja) + " t " + String (tablica_adres_fun[i].czas));
        Blynk.virtualWrite(V40, "update", i, s , tablica_adres_fun[i].stan);
      }
    }
  }
}
void gogo(long kod) {// główna procedura obsugi funkcji routera

  byte indeks_kod_we = szukaj_indeks_we(kod);
  if (printon) {
    Serial.println ("kod we " + String (kod) + "  index we  " + String (indeks_kod_we));
  }
  if (terminalon) {
    terminal.println("kod we " + String (kod) + "  index we  " + String (indeks_kod_we));
    terminal.flush();
  }
  if (indeks_kod_we != 0) {

    for (int i = 1; i < (rozm_tab_fun); i++ )
    {
      if (indeks_kod_we == tablica_adres_fun[i].kodwe) {

        if (tablica_adres_fun[i].funkcja == zal)  send_indeks_wy(tablica_adres_fun[i].kodwy);

        if (tablica_adres_fun[i].funkcja == przel_zal) {
          if (tablica_adres_fun[i].stan)send_indeks_wy(tablica_adres_fun[i].kodwy); else send_indeks_wy(tablica_adres_fun[i + 1].kodwy);
          if (tablica_adres_fun[i].stan) tablica_adres_fun[i].stan = 0; else tablica_adres_fun[i].stan = 1;
          String s = String ("we " + String (tablica_adres_fun[i].kodwe) + " wy " + String (tablica_adres_fun[i].kodwy) + " F " + String (tablica_adres_fun[i].funkcja) + " t " + String (tablica_adres_fun[i].czas));
          Blynk.virtualWrite(V40, "update", i, s , tablica_adres_fun[i].stan);
        }
        if (tablica_adres_fun[i].funkcja == wlsekzal) {
          i++;
          tablica_adres_fun[i].stan = tablica_adres_fun[i].czas;
          String s = String (String (tablica_adres_fun[i].kodwe) + " " + String (tablica_adres_fun[i].kodwy) + " " + String (tablica_adres_fun[i].funkcja) + " " + String (tablica_adres_fun[i].czas));
          Blynk.virtualWrite(V40, "update", i, s , tablica_adres_fun[i].stan);
          i--;
          send_indeks_wy(tablica_adres_fun[i].kodwy);
        }
        /*
          }
          if (tablica_adres_fun[i].funkcja == wlh) {
          tablica_adres_fun[i].stan = 60 * 60 * tablica_adres_fun[i].czas;
          send_indeks(tablica_adres_fun[i].kodwy);
          }
        */
      }
    }
  }
}

