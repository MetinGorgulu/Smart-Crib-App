#define BLYNK_TEMPLATE_ID "TMPLTKmqq3I0"
#define BLYNK_DEVICE_NAME "Akıllı Beşik"
#define BLYNK_AUTH_TOKEN "M9UPP0hP1Ne8E5lfHKNfAr2QTteSV9BG"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "ThingSpeak.h"


char auth[] = "M9UPP0hP1Ne8E5lfHKNfAr2QTteSV9BG";
char ssid[] = "MetinG";
char pass[] = "12345678";



const long channelID =1996999;             // Thingspeak channel ID 
const int field_no=1; 
const char* writeAPIKey = "169WSPYBJ6T54EGG";   // Thingspeak write API Key 
const char* readAPIKey = "NYSVVMH2PXAMLCG6";   // Thingspeak read API Key 



const char host[] = "api.pushingbox.com";
const char devid[] = "v29B6250252E2CAC";



int sessensor = D4;
int hareketsensor = D2;
int sicakliksensor = A0;
int aktiflamba = D5;
int pasiflamba = D6;


int alarm = 0;
double olculenDeger;
double sicaklikDegeri;
float sicaklikThingSpeak = 0;
int sicaklikiste = 0;
int eskisure = 0;
int yenisure = 0;



WiFiClient client1;
BLYNK_WRITE(V0)
{
  alarm = param.asInt();
  veriGonder();
}
BLYNK_WRITE(V3)
{
  sicaklikiste = param.asInt();
}
void veriGonder()
{
  
  if(alarm == 1)
  {
    for(int i =5; i>0;i-- )
    {
    Blynk.virtualWrite(V1, i);
    delay(1000);
    }
    Blynk.virtualWrite(V1, "AÇIK");
  }
  else
  {
    Blynk.virtualWrite(V1,"KAPALI");
  }
}
void alarmkapat()
{
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, "ÇOCUK UYANDI");
  alarm = 0;
}
void sicaklik()
{
  olculenDeger = analogRead (sicakliksensor);  // A0 analog ucundan değer oku
  sicaklikDegeri = (olculenDeger/1023.0)*5000;     // mv'a dönüşüm işlemi
  sicaklikDegeri = sicaklikDegeri /10.0;         // mV'u sıcaklığa dönüştür
  Blynk.virtualWrite(V2,sicaklikDegeri); 
}
void setup()
{

  Serial.begin(9600);
  Blynk.begin(auth,ssid,pass);
  pinMode(sessensor, INPUT);
  pinMode (sicakliksensor, INPUT);
  pinMode (hareketsensor, INPUT);
  pinMode(aktiflamba, OUTPUT);
  pinMode(pasiflamba, OUTPUT);
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V3,0);
  Blynk.virtualWrite(V1,"HENÜZ KAPALI");
  Blynk.virtualWrite(V2,0);
  ThingSpeak.begin(client1);
  lamba();
}

void loop()
{
  Blynk.run();
  
  int hareketalgi = digitalRead(hareketsensor);
  int sesalgi = digitalRead(sessensor);
  if(alarm == 1)
  {
    if (hareketalgi == 1|| sesalgi == 0)
    {
      Serial.print("Bağalnıyor : ");
      Serial.println(host);
      WiFiClient client;
      const int httpPort = 80;
      if(!client.connect(host,httpPort))
      {
        Serial.println("connection failed");
        return;
      }
      String url = "/pushingbox";
      url += "?devid=";
      url += devid;
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) 
      {
        if (millis() - timeout > 5000) 
        {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
      }
      while(client.available()) 
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }
      Serial.println();
      Serial.println("closing connection");
      alarmkapat();
    }
    else 
    {
      
    }
  }
  if(sicaklikiste==1)
  {
    sicaklik();
    sicaklikiste =2;
    Blynk.virtualWrite(V3,0);
  }
  else
  {
  }
  sicaklikThingSpeakGonder ();
  lamba();
}
void sicaklikThingSpeakGonder ()
{ 
  yenisure = millis();
  if(eskisure + 100000 < yenisure)
  {
    Serial.println("ThingSpeak e Veri Yollandı");
    olculenDeger = analogRead (sicakliksensor);  // A0 analog ucundan değer oku
    olculenDeger = (olculenDeger/1023.0)*5000;     // mv'a dönüşüm işlemi
    sicaklikThingSpeak = olculenDeger /10.0;         // mV'u sıcaklığa dönüştür
    ThingSpeak.writeField (channelID, field_no, sicaklikThingSpeak, writeAPIKey);       // sıcaklık değerini gönder 
    eskisure = yenisure;
  }
} 
void lamba()
{
  if(alarm)
  {
    digitalWrite(aktiflamba, HIGH);
    digitalWrite(pasiflamba, LOW);
  }
  else
  {
    digitalWrite(aktiflamba, LOW);
    digitalWrite(pasiflamba, HIGH);
  }
}