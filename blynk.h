/*
  v0 LED kontrolny
  v1 terminal do wyswietlania co dzieje sie wewnatrz 
  v3 recive kod // wyświetlanie odebranego kodu
  v5 print on/off
  v6 term on/off
  
  v20  v30 v40 tablice kodow we wy i funkcji
  v33  v36     terminal ręczne  wprowadzanie parametrów kodów wejsciowych / wysciowych
  v27  v35     wyświetlanie zapisywanego kodu wejsciowego / wyjsciowego
  v26  v34    przycisk start/stop procedury zapisu kodu do tabel i pamieci wejsciowego  /wyjsciowego
  v22    przycisk zapisu funkcji do tabeli funkcji 
  v23 wybrany indeks tablicy funkcji
  v24 przygotowywny rekord tablicy funkcji
  v25 wybór funkcji 
  
  v29 suwak zadawania czasu
  v31 wysiwtlanie sformatowanego czasu

  v28 send kod wyjściowy
  v12 reset wszystkich tabel
*/

char ssid[] = "00000";
char pass[] = "kkkkkkkkk";
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "af1d652f94a94e9386b34c47c576ebbc";

#define _RED            "#FF0000"
#define _GREEN          "#00FF00"
#define _BLUE           "#0000F0"

#define led_wew 2
#define treconnect 60 // czas miedzy kolejnymi testami łaczności z serwerem BLYNK w sek
int liczreconnect = 10; // licnik czasu testów łącznosci
int printon = 0; // flaga wysyłania danych na monitor
int terminalon = 0; // flaga wysyłania danych na terminal BLYNK

WidgetLED led1(V0); // led w aplikacji wskazujący na połacenie mikroprocesor - aplikacja
WidgetTerminal terminal(V1);


void blynksetup() //nie blokująca procedura połączenia z BLYNK
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Blynk.config(auth, "kpr");
  Blynk.connect();
}

void blynkakcja()
{
  if (Blynk.connected()) Blynk.run();
}

void testconnect() // test łączności z serwerem BLYNK bez blokowania programu głównego GS
{
  if (Blynk.connected()) {
    //    digitalWrite(led_blue, HIGH); //wskaźnik łączności z serwerem stan wysoki- wyłączenie LEDa
    if (printon) {
      Serial.println("connection OK");
    }
    liczreconnect = treconnect;
  } else {
    //    digitalWrite(led_blue, LOW);
    if (printon) {
      Serial.println("connection DOWN");
    }
    liczreconnect--;
    if (liczreconnect == 0) {
      Blynk.connect();
      liczreconnect = treconnect;
    }
  }
}
int wskrunAr = 0;
void miganievLED()//miganie vLED dla kontroli połączenia i miganie ledem na module ESP jako kontrola pracy programu
{
  wskrunAr = !wskrunAr ;
  if (wskrunAr == 0)led1.off(); else led1.on();
  digitalWrite(led_wew, wskrunAr);

}

BLYNK_WRITE(V5) {
  printon = param.asInt();
}

BLYNK_WRITE(V6) {
  terminalon = param.asInt();
}

