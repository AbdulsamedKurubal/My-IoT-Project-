#include <Wire.h> // i2c iletişimi için gerekli kütüphane
#include <LiquidCrystal_I2C.h> // lcd ekran için gerekli kütüphane

#include <ArduinoWiFiServer.h> // wifi sunucu işlemleri için gerekli kütüphane
#include <BearSSLHelpers.h> // ssl işlemleri için yardımcı kütüphane
#include <CertStoreBearSSL.h> // ssl sertifika depolama için kütüphane
#include <ESP8266WiFi.h> // esp8266 wifi bağlantısı için temel kütüphane
#include <ESP8266WiFiAP.h> // wifi erişim noktası özellikleri için kütüphane
#include <ESP8266WiFiGeneric.h> // genel wifi işlevleri için kütüphane
#include <ESP8266WiFiGratuitous.h> // wifi bağlantısının sürekliliğini sağlamak için kullanılan kütüphane
#include <ESP8266WiFiMulti.h> // birden fazla wifi ağına bağlanmak için kütüphane
#include <ESP8266WiFiSTA.h> // wifi istemcisi özellikleri için kütüphane
#include <ESP8266WiFiScan.h> // wifi ağlarını taramak için kullanılan kütüphane
#include <ESP8266WiFiType.h> // wifi bağlantı türleri için kütüphane
#include <WiFiClient.h> // temel wifi istemci işlemleri için kütüphane
#include <WiFiClientSecure.h> // güvenli wifi istemcisi için kütüphane
#include <WiFiClientSecureBearSSL.h> // ssl destekli güvenli istemci işlemleri için kütüphane
#include <WiFiServer.h> // wifi sunucu işlemleri için kütüphane
#include <WiFiServerSecure.h> // güvenli wifi sunucusu işlemleri için kütüphane
#include <WiFiServerSecureBearSSL.h> // ssl destekli güvenli sunucu işlemleri için kütüphane
#include <WiFiUdp.h> // udp iletişimi için kütüphane

#define BLYNK_TEMPLATE_ID "TMPL6EytYuUD_" // blynk template id'si
#define BLYNK_TEMPLATE_NAME "Akilli Saksi" // blynk template ismi

// blynk ve diğer kütüphaneleri içe aktar
#define BLYNK_PRINT Serial // blynk ile seri portu yazdırma aktif

#include <BlynkSimpleEsp8266.h> // esp8266 ile blynk entegrasyonu için gerekli kütüphane
#include <ESP8266WiFi.h> // esp8266 wifi bağlantısı için temel kütüphane

// lcd ekranı tanımla
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "BCm7vFSaAk51w75ZDFiFnanbRmMPDk2k"; // blynk auth token
char ssid[] = "Mi10T"; // wifi ağı adı
char pass[] = "samet123"; // wifi ağı şifresi

BlynkTimer timer; // zamanlayıcı oluştur
bool Relay = 0; // röle durumunu takip etmek için değişken

// sensör ve su pompası pinlerini tanımla
#define sensor A0 // toprak nem sensörü pini
#define waterPump D3 // su pompası pini

void setup() {
  Serial.begin(9600); // seri portu başlat
  
  pinMode(waterPump, OUTPUT); // su pompasını çıkış olarak ayarla
  digitalWrite(waterPump, HIGH); // su pompasını başlangıçta kapalı yap
  lcd.begin(); // lcd ekranı başlat
  lcd.backlight(); // lcd arka ışığını aç

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080); // blynk ile bağlantıyı başlat
  
  lcd.setCursor(1, 0); // lcd üzerinde yazı pozisyonunu ayarla
  lcd.print("Sistem Yukleniyor"); // yükleme mesajı göster
  for (int a = 0; a <= 10; a++) {
    lcd.setCursor(a, 1); // ilerleme noktalarını göster
    lcd.print(".....");
    delay(500); // gecikme ekle
  }
  lcd.clear(); // lcd'yi temizle

  // toprak nem sensörünü kontrol etmek için zamanlayıcı ayarla
  timer.setInterval(100000L, soilMoistureSensor);
}

// blynk uygulamasından gelen buton değerini al
BLYNK_WRITE(V1) { 
  Relay = param.asInt(); // butondan gelen değeri oku

  if (Relay == 1) { // buton aktifse
    digitalWrite(waterPump, LOW); // pompayı çalıştır
    lcd.setCursor(0, 1); // lcd pozisyonu ayarla
    lcd.print("Pompa  Aktif ");
  } else { // buton pasifse
    digitalWrite(waterPump, HIGH); // pompayı kapat
    lcd.setCursor(0, 1); // lcd pozisyonu ayarla
    lcd.print("Pompa Kapali");
  }
}

// toprak nem değerlerini al
void soilMoistureSensor() {
  int value = analogRead(sensor); // sensör değerini oku
  value = map(value, 0, 1024, 0, 100); // değeri yüzdeye çevir
  value = (value - 100) * -1; // ters çevirerek nem oranını hesapla

  Blynk.virtualWrite(V3, value); // blynk'e nem değerini gönder
  lcd.setCursor(0, 0); // lcd üzerinde pozisyonu ayarla
  lcd.print("Nem Degeri :"); // nem değeri yazdır
  lcd.print(value);
  lcd.print(" ");

  Serial.print("Nem Değeri: "); // seri porttan nem değerini yazdır
  Serial.println(value);
}

void loop() {
  Blynk.run(); // blynk kütüphanesini çalıştır
  timer.run(); // zamanlayıcıyı çalıştır
}
