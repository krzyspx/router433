
#include "blynk.h" //obsługa BLYNKa
#include "NO433.h" // obsługa transmisji 433 MHz
#include "funkcje.h" // obsluga setup routera
#include "router_gogo.h" // obsługa pracy routera


#include <Timers.h> //  my favorite timer 
Timers <3> akcja; //

#define led_wew 2      //program OK miganie 1 sek

void setup()
{
  pinMode(led_wew, OUTPUT); //LED na module
  digitalWrite(led_wew, HIGH);

  Serial.begin(115200);
  delay(100);
  Serial.println();  Serial.println(F(__FILE__));  //BLYNK .4.10 Arduino IDE.cc 1.8.3

  akcja.attach(0, 2000, timer1sek); //  2 sek
  akcja.attach(1, 10000, timer10sek); //  10 sek
  //  akcja.attach(1, 2000, sendEvent);

  blynksetup(); // setup biblioteki BLYNK
  no433setup();  //setup 433 MHz
  routersetup();  // setup routera

}
void loop()
{
  akcja.process(); // timer
  blynkakcja();    //BLYNK
  no433akcja();   // n/o 4333MHz
  router();       // obsługa procedur router
}

//-------------- procedury wewnętrzne

void timer1sek() { //różne rzeczy wykonywane cyklicznie
  miganievLED(); //wskaźnik vLED działania komunikacji
  testconnect(); // test łączności z serwerem
  router_timer(); // odliczanie czasu w routerze
}





