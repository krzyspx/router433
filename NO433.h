
void gogo(long kod);
long o_kod = 0; // odbebrany kod
int o_impuls = 0; //dł implulsu odbieranych kodów
byte o_bity = 0;  // ilość bitów kodu
byte o_prot = 0; // nr protokolu

#include <RCSwitch.h> // biblioteka obsługi nadawania i odbioru 433 MHz
RCSwitch mySwitch = RCSwitch();

void no433setup()
{ mySwitch.enableReceive(12);  // port odbiornika
  mySwitch.enableTransmit(13); // nadajnika
}

void no433akcja()
{ // procedyra obsługi odbioru kodu przez router
  if (mySwitch.available()) {

    if (printon) { //przełącznik wysyłania info o odebranym kodzie na monitor
      Serial.println("Kod " + String(mySwitch.getReceivedValue()) + " / " + String(mySwitch.getReceivedBitlength()) + " IMP " + String(mySwitch.getReceivedDelay())
                     + " Prot  "  + " / " + String(mySwitch.getReceivedProtocol()) );
    }
    if (terminalon) {// przełącznik wysyłania info na terminal BLYNK
      terminal.println("Kod " + String(mySwitch.getReceivedValue()) + " / " + String(mySwitch.getReceivedBitlength()) + " IMP " + String(mySwitch.getReceivedDelay())
                       + " Prot " + " / " + String(mySwitch.getReceivedProtocol()) );
      terminal.flush();
      String str = String(mySwitch.getReceivedValue()) + " / " + String (mySwitch.getReceivedDelay());
      Blynk.virtualWrite(V3, str);
    }
// ładowanie danych o odebranym kodzie
    o_kod = mySwitch.getReceivedValue(); 
    o_impuls = mySwitch.getReceivedDelay();
    o_bity = mySwitch.getReceivedBitlength();
    o_prot = mySwitch.getReceivedProtocol();

    gogo(mySwitch.getReceivedValue()); // główna procedura pracy routera
    mySwitch.resetAvailable();
  }
}

void sendkod(unsigned long code, int impuls, byte bity, byte prot) { // wysłanie kodu z routera
  mySwitch.setPulseLength(impuls);
  mySwitch.setProtocol(prot);
  mySwitch.setRepeatTransmit(4);
  mySwitch.send(code, bity);
}

void sendkodpilot (unsigned long code) {
  sendkod(code, 320, 24, 1);
}




