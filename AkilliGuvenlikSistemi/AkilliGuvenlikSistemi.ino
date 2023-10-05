#include <ESP8266WiFi.h> 
#include "DHT.h" 
#include <SPI.h>                          
#include <MFRC522.h>                      
#include "ThingSpeak.h"
#include "DHT.h"

#define WLANSSID "RedmiNote8"
#define WLAN_PASSWORD "yasonun123"
#define DHTPIN 2   
#define DHTTYPE DHT11 
unsigned long myChannelNumber = 1993513;
const char * myWriteAPIKey = "WNCD0M99HPWAHCZH";

int RST_PIN = 0;                          
int SS_PIN = 4;                        
int led = 15;
int dht_pin = 2;
int trigPin = 16;
int echoPin = 5;
long sure;
int mesafe;

MFRC522 rfid(SS_PIN, RST_PIN);     
DHT dht(DHTPIN, DHTTYPE);      
WiFiClient  client;
byte ID[4] = {82, 96, 187, 75};          

void setup() 
{
    Serial.begin(115200);  
    dht.begin();
    WiFi.begin(WLANSSID, WLAN_PASSWORD); 
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);  
  } 
   pinMode(led, OUTPUT);
   pinMode(trigPin, OUTPUT);
   pinMode(echoPin, INPUT);             
   SPI.begin();  
   rfid.PCD_Init();                       
   WiFi.mode(WIFI_STA);
   ThingSpeak.begin(client);
}
void loop()
{
       digitalWrite(trigPin, LOW);
       delayMicroseconds(2);
       digitalWrite(trigPin, HIGH);
       delayMicroseconds(10);
       digitalWrite(trigPin, LOW);
       sure = pulseIn(echoPin, HIGH);
       mesafe = sure * 0.034 / 2;
      int olcum=mesafe;
    if(olcum!=0)
  {
    digitalWrite(led,LOW);
      if(olcum<15)
      {
        digitalWrite(led,HIGH);
      }
      
      else
      {
         digitalWrite(led,LOW);
      } 
     Serial.println("Lütfen kartınızı okutunuz");
     delay(2000);
 
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {
    Serial.println("kart bekleniyor.");
    return;
  }
  if ( ! rfid.PICC_ReadCardSerial())   
  {    
    Serial.println("kart okunamadı.");
    return;
  }
    int i;
    String content= "";
 
    for (byte i = 0; i < rfid.uid.size; i++) 
  {
     Serial.println(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.println(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  if (rfid.uid.uidByte[0] == ID[0] &&    
      rfid.uid.uidByte[1] == ID[1] &&
      rfid.uid.uidByte[2] == ID[2] &&
      rfid.uid.uidByte[3] == ID[3] )
 {
    Serial.println("      Yetkili kisi");
    ekranaYazdir();
       float h = dht.readHumidity();
       float t = dht.readTemperature();
      Serial.print("Sicaklik:"); 
      Serial.println(t);
      Serial.print("Nem:");
      Serial.println(h);
      ThingSpeak.setField(1,t);
      ThingSpeak.setField(2,h);
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if (x == 200)
      {
        Serial.println("Gonderme basarili.");
      }
      else
      {
        Serial.println("Hata HTTP hata kodu: " + String(x));
      }
    }
  }
  
  else {                                
    Serial.println("Yetkisiz Kart");
    digitalWrite(led, 1);
    delay(500);
    digitalWrite(led, 0);
    ekranaYazdir();
  }
  rfid.PICC_HaltA();

}
void ekranaYazdir() {
  Serial.print("ID Numarasi: ");
  for (int sayac = 0; sayac < 4; sayac++) {
    Serial.print(rfid.uid.uidByte[sayac]);
    Serial.print(" ");
  }
  Serial.print("");
}
