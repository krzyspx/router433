#include <EEPROM.h>
WidgetTerminal terminal_we(V33); //ręczne  wprowadzanie parametrów kodów
WidgetTerminal terminal_wy(V36); //ręczne  wprowadzanie parametrów kodów

#define zal 1  // wartosci pola rodzajów funkcji routera
#define przel_zal 2  // załączanie - kod odebrany zamieniony jest na inny wysłany
#define przel_wyl 3 //przełączanie - ten sam kod wejściowy powoduje wysłanie raz kodu załączania raz wyłączania
#define wlsekzal 4   // załączenia czasowe start - po odebraniu kodu wejściowego wysyłany jest kod załączający
#define wlsekwyl 5   // załączanie czasowe stop - po odliczeniu czasu wysyłany jest kod wyłączajacy

#define max_opis 10  // maksymalna długość opisu kodow
struct stadres_we {   // struktura tabeli dla kodow wejściowych
  char opis[max_opis];
  long kod;
};

struct stadres_wy { // struktura tabeli dla kodów wyjsciowych
  char opis[max_opis];
  long kod;   //kod
  int impuls; // dlugosc impulsu kodu
  byte bity;  // ilosc bitow kodu
  byte prot;  // nr protokolu
};
struct stfunkcja {  // struktura tabeli funkcji routera
  byte kodwe;  // indeks kodu wejsiowego
  byte kodwy;  //indeks kodu wyjściowego
  byte funkcja;  // nr funkcji
  unsigned int czas; // czas w procedurze zalłączenia czasowego
  unsigned int  stan; //zmienna pomocnicza wskazująca na bieżący stan akcji
};

const int rozm_tab_we = 5; //rozmiar tablicy kodów wejściowych
const int rozm_tab_wy = 5; //rozmiar tablicy kodów wyjsciowych
const int rozm_tab_fun = 5; //rozmiar tablicy kodów funkcji

stadres_we tablica_adres_we[rozm_tab_we]; //deklaracja tablicy kodów nadajników
stadres_wy tablica_adres_wy[rozm_tab_wy]; //deklaracja tablicy kodów odb
stfunkcja tablica_adres_fun[rozm_tab_fun]; //deklaracja tablicy kodów funkcji

stfunkcja wpis_funkcji = {0, 0, 0, 0, 0}; //zmienna /danych do zapisu w tablicy funkcji
//--------------- zapis i odczyt eeprom (flash)

#define pamkodwe 100    // początek tablicy kodów nadajnikow we flash
#define pamkodwy 400    // początek tablicy kodów odbiorników we flash
#define pamkodfun 700    // początek tablicy kodów funkcji we flash

int nrpozkod = 0; //wybrany nr pozycji tabeli
int stan = 1; // nr stanu wewnętrznego programu
int p_kod = 0; //stan przycisku aplikacji BLYNK - zapisu kodu do pamięci

int tablica_nr = 0; //nr tablicy z ostatnio naciśniętą pozycją

String kod_etykieta = "";  //zmienna pomocnicza wprowadzania opiu kodu
long KOD = 0; // kod  do zapisu w pamięci
int IMPULS = 0; //impuls  do zapisu w pamięci
byte BITY = 0; //ilosc bitow kodu a do zapisu w pamięci
byte PROT = 0; //nr protokolu urządzenia do zapisu w pamięci

void save_tab () { //zapisz tabeli do flash

  EEPROM.begin(1024);
  EEPROM.put(pamkodwe, tablica_adres_we);
  EEPROM.put(pamkodwy, tablica_adres_wy);
  EEPROM.put(pamkodfun, tablica_adres_fun);
  EEPROM.end(); //  EEPROM.commit();
}

void read_tab() { // odczyt tabeli z flash

  EEPROM.begin(1024);
  EEPROM.get(pamkodwe, tablica_adres_we);
  EEPROM.get(pamkodwy, tablica_adres_wy);
  EEPROM.get(pamkodfun, tablica_adres_fun);
  EEPROM.end(); //  EEPROM.commit();
}

void reset_all() { // resety po zakończeniu procedury
  o_kod = 0; KOD = 0; p_kod = 0; o_impuls = 0; o_bity = 0; o_prot = 0;
}

void rekord_we(String str) { //wyswietlanie rekordu tabeli kodow wejsciowych o nr "wpis_funkcji"
  Blynk.virtualWrite(V20, str, wpis_funkcji.kodwe, String(String(wpis_funkcji.kodwe) + ". " + String(tablica_adres_we[wpis_funkcji.kodwe].opis)  + " > "
                     + String(tablica_adres_we[wpis_funkcji.kodwe].kod)), 0);
  Blynk.virtualWrite (V20, "select", wpis_funkcji.kodwe);
  Blynk.virtualWrite(V27, KOD); //wyświetlanie zapisywanego kodu
}
void rekord_wy(String str) { //wyswietlanie rekordu tabeli kodow wyjsciowych
  Blynk.virtualWrite(V30, str, wpis_funkcji.kodwy, String(String(wpis_funkcji.kodwy) + ". " + String(tablica_adres_wy[wpis_funkcji.kodwy].opis)  + " > "
                     + String(tablica_adres_wy[wpis_funkcji.kodwy].kod) + " / " + String(tablica_adres_wy[wpis_funkcji.kodwy].bity)),
                     String(String(tablica_adres_wy[wpis_funkcji.kodwy].impuls) + " / " + String(tablica_adres_wy[wpis_funkcji.kodwy].prot)));
  Blynk.virtualWrite (V30, "select", wpis_funkcji.kodwy);
  Blynk.virtualWrite(V35, KOD); //wyświetlanie zapisywanego kodu
}
void rekord_fun(String str, int i) { //wyswietlanie rekordu tabeli funkcji
  String s = String ("we " + String (tablica_adres_fun[i].kodwe) + " wy " + String (tablica_adres_fun[i].kodwy)
                     + " F " + String (tablica_adres_fun[i].funkcja) + " t " + String (tablica_adres_fun[i].czas));
  Blynk.virtualWrite(V40, str, i, s , tablica_adres_fun[i].stan);
  Blynk.virtualWrite (V40, "select", i);
}

void zapisz_opis_we(String str) { //zapisanie opisu kodu wejsciowego do tabeli i do pamieci
  int dl_str = str.length() + 1;  //długość str
  if (dl_str > (max_opis + 0))dl_str = max_opis + 0;
  str.toCharArray(tablica_adres_we[wpis_funkcji.kodwe].opis, dl_str); //change string to char[]
  rekord_we("update");
  save_tab();
}
void zapisz_opis_wy(String str) { //zapisanie opisu kodu wyjsciowego do tabeli i do pamieci
  int dl_str = str.length() + 1;  //długość str
  if (dl_str > (max_opis + 0))dl_str = max_opis + 0;
  str.toCharArray(tablica_adres_wy[wpis_funkcji.kodwy].opis, dl_str); //change string to char[]
  rekord_wy("update");
  save_tab();
}
BLYNK_WRITE(V36) {//trminal dla recznego wprowadzania parametrow kodow wyjsciowych
  switch (stan) {
    case 1: {
        zapisz_opis_wy(param.asStr());

      } break;
    case 2: {
        String str = param.asStr();
        char znak[10];
        int dl_str = str.length() + 1;  //długość str
        str.toCharArray(znak, dl_str); //change string to char[]
        KOD  = atol(znak);
        terminal_wy.println("Wprowadzony KOD  " + String(KOD));
        terminal_wy.println("Wprowadz IMPULS  ");
        terminal_wy.flush();
        stan = 10;
      } break;
    case 10: {
        IMPULS = param.asInt();
        terminal_wy.println("Wprowadzony IMPULS  " + String(IMPULS));
        terminal_wy.println("Wprowadz ilosc bitow  ");
        terminal_wy.flush();
        stan = 11;
      } break;
    case 11: {
        BITY = param.asInt();
        terminal_wy.println("Wprowadzono ilosc bitow  " + String(BITY));
        terminal_wy.println("Wprowadz nr protokolu  ");
        terminal_wy.flush();
        stan = 12;
      } break;
    case 12: {
        PROT = param.asInt();
        terminal_wy.println("Wprowadzono nr protokolu " + String(PROT));
        terminal_wy.println("Wprowadz opis  ");
        terminal_wy.flush();
        stan = 13;
      } break;
    case 13: {
        String str = param.asStr();
        kod_etykieta = str;
        terminal_wy.println("Wprowadzo  " + str + " K = " + String(KOD) + " / " + String(BITY) + " I = " + String(IMPULS) + " / " + String(PROT));
        terminal_wy.println("Zapisac t/n ");
        terminal_wy.flush();
        stan = 14;
      } break;
    case 14: {
        String str = param.asStr();
        if (str == "t")terminal_wy.println("zapisano"); else terminal_wy.println("odrzucono");
        terminal_wy.flush();
        if (str == "t") {
          tablica_adres_wy[wpis_funkcji.kodwy].kod = KOD;    tablica_adres_wy[wpis_funkcji.kodwy].impuls = IMPULS;
          tablica_adres_wy[wpis_funkcji.kodwy].bity = BITY; tablica_adres_wy[wpis_funkcji.kodwy].prot = PROT;
          zapisz_opis_wy(kod_etykieta);
          reset_all();
        }
        stan = 1;
        Blynk.virtualWrite(V26, 0);   Blynk.virtualWrite(V34, 0); //przełcznik procedury wyłącz
      } break;
    default: break;
  }
}
BLYNK_WRITE(V33) {//trminal dla recznego wprowadzania parametrow kodow wejsciowych
  switch (stan) {
    case 1: {
        zapisz_opis_we(param.asStr());
      } break;
    case 2:
    case 4:
      {
        String str = param.asStr();
        char znak[10];
        int dl_str = str.length() + 1;  //długość str
        str.toCharArray(znak, dl_str); //change string to char[]
        KOD  = atol(znak);
        terminal_we.println("Wprowadzony KOD  " + String(KOD));
        terminal_we.println("Wprowadz opis  ");
        terminal_we.flush();
        stan = 11;
      } break;
    case 11: {
        String str = param.asStr();
        kod_etykieta = str;
        terminal_we.println("Wprowadzo  " + str + " K = " + String(KOD));
        terminal_we.println("Zapisac t/n ");
        terminal_we.flush();
        stan = 12;
      } break;
    case 12: {
        String str = param.asStr();
        if (str == "t")terminal_we.println("zapisano"); else terminal_wy.println("odrzucono");
        terminal_we.flush();
        if (str == "t") {
          tablica_adres_we[wpis_funkcji.kodwe].kod = KOD;
          zapisz_opis_we(kod_etykieta);
          reset_all();
        }
        stan = 1;
        Blynk.virtualWrite(V26, 0);   Blynk.virtualWrite(V34, 0); //przełcznik procedury wyłącz
      } break;
    default: break;
  }
}

void do_zapisz_kod() { // zapisanie kod do tablic i do pamieci

  if (tablica_nr == 1) {
    tablica_adres_we[nrpozkod].kod = KOD;
    rekord_we("update");
  }
  if (tablica_nr == 2) {
    tablica_adres_wy[nrpozkod].kod = KOD; tablica_adres_wy[nrpozkod].impuls = IMPULS;
    tablica_adres_wy[nrpozkod].bity = BITY; tablica_adres_wy[nrpozkod].prot = PROT;
    rekord_wy("update");
  }
  save_tab();
  Serial.print(" nr  " + String(nrpozkod)); Serial.print("  zapisuje kod  " + String(KOD));  Serial.println(" impuls " + String(IMPULS));
  Blynk.virtualWrite(V26, 0);   Blynk.virtualWrite(V34, 0); //przełcznik procedury wyłącz
  reset_all();
}
BLYNK_WRITE(V26) { // przelacznik start/stop prcedury zapisu kodu wejsciowego do pamieci
  p_kod = param.asInt(); //stan klawisza "start zapisu kodu" 1 = start procedury
  if (param.asInt()) {
    stan = 1;
    terminal_we.println("Wyslij lub wprowadz KOD");
    terminal_we.flush();
  }
}
BLYNK_WRITE(V34) { // przelacznik start/stop prcedury zapisu kodu wyjsciowego do pamieci
  p_kod  = param.asInt(); //stan klawisza "start zapisu kodu" 1 = start procedury
  if (param.asInt()) {
    stan = 1;
    terminal_wy.println("Wyslij lub wprowadz KOD");
    terminal_wy.flush();
  }
}
void KOD_do_eeprom() { //graf czyli automat skończony procedury zapisu kodu 433MHz do pamięci
  switch (stan) {
    case 1: {
        if (p_kod != 0) stan = 2;
        o_kod = 0; KOD = 0;
      } break;
    case 2: {
        if (p_kod == 0) stan = 1;
        if (o_kod != 0) stan = 3;
      } break;
    case 3: {
        KOD = o_kod;
        IMPULS = o_impuls;
        BITY = o_bity;
        PROT = o_prot;
        Blynk.virtualWrite(V27, KOD);
        Blynk.virtualWrite(V35, KOD);
        o_kod = 0;
        stan = 4;
      } break;
    case 4: {
        if (p_kod == 0) stan = 5;
        if (o_kod != 0) stan = 3;
      } break;
    case 5: {
        do_zapisz_kod();
        stan = 1;
      } break;
    default: break;
  }
}

//----------------------------------------- rozne procedury na tablicach

void wyswietl_nowy_rekord() { // wysiwtlanie zmiennej do zapisu do tabelii funkcji
  Blynk.virtualWrite(V24, String ("We " + String (wpis_funkcji.kodwe) + " Wy " + String (wpis_funkcji.kodwy) + " Fun " + String (wpis_funkcji.funkcja)
                                  + " Czas " + String (wpis_funkcji.czas) + " Stan " + String (wpis_funkcji.stan)));
}

void wyswietl_start() {//ustawienia początkowe - zerowania - wyswietalania
  wyswietl_nowy_rekord();
  Blynk.virtualWrite(V23, 0);
  Blynk.virtualWrite(V25, 0);
  Blynk.virtualWrite(V27, 0);
  Blynk.virtualWrite(V35, 0);
}

BLYNK_WRITE(V20) { //obsluga nacisniecia pozycji tabeli kodow wejsciowych
  tablica_nr = 1;
  int k = param[1].asInt();
  if (k == 0)k = 1;
  Blynk.virtualWrite (V20, "select", 0);
  nrpozkod = k;
  wpis_funkcji.kodwe = k;
  Blynk.virtualWrite (V20, "pick", wpis_funkcji.kodwe);
  wyswietl_nowy_rekord();
}
BLYNK_WRITE(V30) {  //obsluga nacisniecia pozycji tabeli kodow wyjsciowych
  tablica_nr = 2;
  int k = param[1].asInt();
  if (k == 0)k = 1;
  Blynk.virtualWrite (V30, "select", 0);
  nrpozkod = k;
  wpis_funkcji.kodwy = k;
  Blynk.virtualWrite (V30, "pick", wpis_funkcji.kodwy);
  wyswietl_nowy_rekord();
}
BLYNK_WRITE(V25) { //wybór nr funkcji z MENU
  wpis_funkcji.funkcja = param.asInt();
  wyswietl_nowy_rekord();
}

void wyswietl_czas() { // zamiana ustawinych sekund na bardziej czytelny format
  unsigned int czas =  wpis_funkcji.czas;
  int h = czas / 360;
  int mn = (czas % 360) / 6;
  int sk = ((czas % 360) % 6) * 10;
  String s = (String(h) + ":" + String(mn) + ":" + String(sk));
  Blynk.virtualWrite(V31, s);
}
BLYNK_WRITE(V29) {// suwak dla zadawanie czasu
  wpis_funkcji.czas = param.asInt();
  wyswietl_czas();
  wyswietl_nowy_rekord();
}

int flaga_wpis_fun = 0;
BLYNK_WRITE(V22) {//przycisk zapisu rekordu do tablicy funkcji
  if (param.asInt()) flaga_wpis_fun = 1;
}
BLYNK_WRITE(V40) {//obsluga nacisniecia pozycji tabeli funkcji
  Blynk.virtualWrite (V40, "pick", param[1].asInt());
  Blynk.virtualWrite(V23, String ("Fun  " + String (param[1].asInt()) ));
  if (flaga_wpis_fun) {
    flaga_wpis_fun = 0;
    Blynk.virtualWrite(V22, 0);
    byte i = param[1].asInt();
    if (i == 0)i = 1;
    Blynk.virtualWrite (V40, "select", 0);
    tablica_adres_fun[i] = wpis_funkcji;
    if (wpis_funkcji.funkcja == przel_wyl) tablica_adres_fun[i].kodwe = 0;
    if (wpis_funkcji.funkcja == wlsekwyl) tablica_adres_fun[i].kodwe = 0;
    rekord_fun("update", i);
    save_tab ();
  }
}

//--------------------------------------setupy testy
void wyswietl_tab() //wyswietlanie początkowe wszystkich pozycji tablic
{
  for ( wpis_funkcji.kodwe = 1; wpis_funkcji.kodwe < (rozm_tab_we); wpis_funkcji.kodwe++)  rekord_we("add");
  for ( wpis_funkcji.kodwy = 1; wpis_funkcji.kodwy < (rozm_tab_wy); wpis_funkcji.kodwy++)  rekord_wy("add");
  for (int i = 1; i < (rozm_tab_fun); i++ ) rekord_fun("add", i);
}

void test_tablica() { // tylko napotrzeby testowe
  //-------- ustawienia testowe
  Blynk.virtualWrite(V20, "clr");
  Blynk.virtualWrite(V30, "clr");
  Blynk.virtualWrite(V40, "clr");

  tablica_adres_we[0] = {{'r'}, 0};
  tablica_adres_we[1] = {{'r'}, 5526804};
  tablica_adres_we[2] = {{'r'}, 5526801};
  tablica_adres_we[3] = {{'r'}, 5526609};
  tablica_adres_we[4] = {{'r'}, 5526612};
  tablica_adres_wy[0] = {{'t'}, 0 , 0};
  tablica_adres_wy[1] = {{'t'}, 5522769 , 320};
  tablica_adres_wy[2] = {{'t'}, 5522772 , 320};
  tablica_adres_wy[3] = {{'t'}, 5525841 , 320};
  tablica_adres_wy[4] = {{'t'}, 5525844 , 320};
  tablica_adres_fun[0] = {0, 0, 0, 0, 0};
  tablica_adres_fun[1] = {1, 1, 1, 0, 0};
  tablica_adres_fun[2] = {2, 2, 1, 0, 0};
  tablica_adres_fun[3] = {3, 4, 1, 0, 0};
  tablica_adres_fun[4] = {4, 1, 1, 0, 0};
  wyswietl_tab();
}
//----------- reset i wyswietlaie tablic

void reset_tabel() { //resetowanie zawartości wszystkich tablic
  Blynk.virtualWrite(V20, "clr");
  Blynk.virtualWrite(V30, "clr");
  Blynk.virtualWrite(V40, "clr");

  Blynk.virtualWrite(V20, "add", 0, "Nr. Opis  > Kod / Ilosc bitow ", 0);
  Blynk.virtualWrite(V30, "add", 0, "Nr. Opis  > Kod / Ilosc bitow ", "Impuls / Prot");
  Blynk.virtualWrite(V40, "add", 0,  "We   Wy  Funkcja Czas ", "Status");

  for (int i = 1; i < (rozm_tab_we); i++) tablica_adres_we[i] = {{'w', 'e', 'j'}, 0, };
  for (int i = 1; i < (rozm_tab_wy); i++) tablica_adres_wy[i] = {{'w', 'y', 'j'}, 0, 0, 0, 0};
  for (int i = 1; i < (rozm_tab_fun); i++) tablica_adres_fun[i] = {0, 0, 0, 0, 0};
  wyswietl_tab();
  save_tab();
}

BLYNK_WRITE(V12) {//reset tabel
  if (param.asInt())reset_tabel();
}
BLYNK_WRITE(V10) {//przelacznik zapis kod/gogo

}
void read_table_from_eeprom()
{
  read_tab(); // odczytuje z eepromu
  //  wyswietl_tab();
}

void   routersetup() {
  wyswietl_start();

  //  test_tablica();
  read_table_from_eeprom();
}
void  router() {
  KOD_do_eeprom();
}

