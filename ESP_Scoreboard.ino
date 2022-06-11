/*
 * İzmir / Dr.TRonik Youtube / Mayıs 2022 / scoreboard demo
 * NodeMcu & RC_Switch Library
 * https://youtu.be/OqX2fHuU8xA
 */

/********************************************************************
  GLOBALS___GLOBALS___GLOBALS___GLOBALS___GLOBALS___GLOBALS___
 ********************************************************************/
//P10 ile ilgili
#include <DMDESP.h> //P10 matrixin esp ile çalışabilmesi için...
#include <fonts/angka6x13.h> // DMDESP/fonts klasörü altında angka6x13.h olmalıdır...
#include <fonts/SystemFont5x7.h> // DMDESP/fonts klasörü altında SystemFont5x7.h olmalıdır...
#include <fonts/EMSans8x16.h> // DMDESP/fonts klasörü altında EMSans8x16.h olmalıdır...

//SETUP DMD
#define DISPLAYS_WIDE 4 //Yatayda 4 panel
#define DISPLAYS_HIGH 1 // Dikeyde 1 panel
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH); //Enine 4, boyuna 1 panel,DMDESP.h, bu dizilim ile DISPLAYS_HIGH 1 den fazlasını desteklemiyor...

//433mhz RF Kumanda ile ilgili
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch(); //Modül ismi mySwitch olarak oluşturuldu...

//SimpleTimer kütüphanesi...
#include <SimpleTimer.h> //https://playground.arduino.cc/Code/SimpleTimer/
SimpleTimer timer; // timer nesnesi oluşturuldu...
int timerId ;

//DHT ile ilgili...
#include <DHT.h>
#define DHTPin 3 //GPIO3 yani rx etiketli pin
#define DHTType DHT11
DHT dht(DHTPin, DHTType);
int temp_, hum_ ;

//Mini player mp3 ile ilgili...
#include <SoftwareSerial.h> //Yazılımsal olarak seri iletişim pinlerini ayrlama
#include <DFPlayerMini_Fast.h> // MP3 çalarımızın kütüphanesi...
#define SOUND_SERIAL_RX 5 // Buradaki 5 sayısı D5 değil, GPIO5 !..
#define SOUND_SERIAL_TX 4 // Ve GPIO 4 !..
SoftwareSerial sound_serial(SOUND_SERIAL_RX, SOUND_SERIAL_TX); // RX, TX
DFPlayerMini_Fast sound;

int ho, m, s;
int Brightness = 1;
int team_A = 0; //A takımının skor değişkeni...
int team_B = 0; //B takımının skor değişkeni...
int period = 1; // Kaçıncı devre / period olduğunu tutan değiken...
bool flag = 0; // Durum değişkeni...

/********************************************************************
  SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___
 ********************************************************************/
void setup() {
  Serial.begin(9600);
  sound_serial.begin(9600);
  sound.begin(sound_serial);
  dht.begin();
  Disp.start();

  Disp.setBrightness(1);

  mySwitch.enableReceive(2);  //GPIO2 yani D4 etiketli pine fiziksel olarak RF data çıkışı bağlandı...
  //esp8266 için, D0 ve özel pinler hariç, diğer tüm pinler kesme pini özelliğindedir...

  temp_ = dht.readTemperature();
  hum_ = dht.readHumidity();
  delay(100);

  sound.volume(15); //Ses ayarı 0-30 arası...
  delay(50);

  sound.play(3); //Micro SD içindeki 001.mp3 parçasını çal! Kart içindeki parça ismi bu şekilde olmalı...
  delay(2000);

  sound.play(2);
  delay(50);

  // delay(), setup içinde gerekirse verilmeli, loop içindeki delaylar panelde bozulmalara yol açabiliyor...

  //SimpleTimer kütüphanesi ile ilgili fonksiyon zamanlamaları...
  timerId = timer.setInterval(1000, timerUp); // Her saniyede çalışması istenen timerUp fonksiyonu timerId değişkenine atandı
  timer.setInterval(60000, dht_); // her dakikada dht_ fonksiyonu çalışsın
}

/********************************************************************
  LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__
 ********************************************************************/
void loop() {
  Disp.loop();
  timer.run();

  if (mySwitch.available()) {
    // Okuduğumuz kumanda değerlerini if bloklarına yazarak istediğimiz işlemi yaptırabilmek için:
    // 1.Tuş
    if (mySwitch.getReceivedValue() == 5592512) {
      Brightness = Brightness + 50;
      delay(10);
      if (Brightness > 254) {
        Brightness = 1 ;
      }
      Disp.setBrightness(Brightness);
    }

    // 2.Tuş
    else if (mySwitch.getReceivedValue() == 5592368) {
      period = period + 1;
      if (period > 4) {
        period = 1 ;
      }
    }

    // 3.Tuş
    else if (mySwitch.getReceivedValue() == 5592560) {
      team_A = team_A + 1;
      if (team_A > 20) {
        team_A = 0;
      }
    }

    // 4.Tuş
    else if (mySwitch.getReceivedValue() == 5592332) {
      team_B = team_B + 1;
      if (team_B > 20) {
        team_B = 0;
      }
    }

    // 5.Tuş
    else if (mySwitch.getReceivedValue() == 5592524) {
      timer.disable(timerId);
      Disp.clear();
      Disp.setFont( EMSans8x16 );
      Disp.drawText(11, 0, "GOOOL");
      sound.play(1);
      delay(50);

    }

    // 6.Tuş
    else if (mySwitch.getReceivedValue() == 5592380) {
      Disp.clear();
      timer.enable(timerId);
    }

    mySwitch.resetAvailable();

  }

  switch (period)
  {
    case 1:
      if (flag) // Durum belirteci
        flag = !flag;
      break;


    case 2:
      if (!flag) {
        sound.play(4); //4 2.devre
        delay(10);
        flag = !flag;
      }
      break;

    case 3:
      if (flag) {
        sound.play(5); //5 3.devre
        delay(10);
        flag = !flag;
      }
      break;

    case 4:
      if (!flag) {
        sound.play(6);
        delay(10);
        flag = !flag;
      }
      break;
  }

  Disp.setFont( SystemFont5x7 );
  Disp.drawText(0, 0, String(temp_));
  Disp.drawText(53, 0, String(hum_));
  Disp.drawChar(56, 8, '%');
  Disp.drawText(4, 8, "C");
  Disp.drawCircle( 1,  9,  1 );

  Disp.drawText(64, 0, "A");
  Disp.drawText(64, 9, ">");

  Disp.drawText(93, 0, "D");
  Disp.drawText(93, 9, String(period));

  Disp.drawText(123, 0, "B");
  Disp.drawText(123, 9, "<");

  Disp.setFont( EMSans8x16 );
  char team_A_sf [3];
  char team_B_sf [3];
  sprintf(team_A_sf , "%02d", team_A);
  Disp.drawText(73, 0, team_A_sf);
  sprintf(team_B_sf , "%02d", team_B);
  Disp.drawText(101, 0, team_B_sf);
}

/********************************************************************
  VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs
********************************************************************/
void timerUp () {

  s = (millis() / 1000);
  m = s / 60;
  ho = s / 3600;
  s = s - m * 60;
  m = m - ho * 60;

  char tmr_up_1 [3];
  char tmr_up_2 [3];
  Disp.setFont( angka6x13 );
  sprintf(tmr_up_1, "%02d", m );
  sprintf(tmr_up_2, "%02d", s );
  Disp.drawText(16, 0, tmr_up_1);
  Disp.drawText(35, 0, tmr_up_2);

  Disp.setFont( SystemFont5x7 );
  if (millis() / 1000 % 2 == 0) // her 1 saniye için
  {
    Disp.drawChar(30, 5, ':'); //iki noktayı göster
  }
  else
  {
    Disp.drawChar(30, 5, ' '); // gösterme
  }
}

void dht_ () {
  temp_ = dht.readTemperature();
  hum_ = dht.readHumidity();
}

//Dr.TRonik---> YouTube...
