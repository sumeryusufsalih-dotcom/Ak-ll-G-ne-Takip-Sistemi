#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Bileşen Tanımlamaları
Servo gunesServosu;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Pin Tanımlamaları
const int ldrSolPin = A0;
const int ldrSagPin = A1;
const int servoPin = 9;
const int sdChipSelect = 4;

// Değişkenler
int servoAcisi = 90;       // Başlangıç açısı
int tolerans = 15;         // Titremeyi önlemek için tolerans payı
unsigned long sonKayitZamani = 0;
const unsigned long kayitAraligi = 5000;

void setup() {
  Serial.begin(9600);
  
  // Servo Kurulumu
  gunesServosu.attach(servoPin);
  gunesServosu.write(servoAcisi);
  
  // LCD Kurulumu
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Aciliyor");
  delay(1000);
  
  // SD Kart Kurulumu
  lcd.clear();
  if (!SD.begin(sdChipSelect)) {
    lcd.print("SD Kart Hatasi!");
    Serial.println("SD kart baslatilamadi!");
    while (1); 
  }
  lcd.print("SD Kart OK.");
  delay(1000);
  lcd.clear();
}

void loop() {
  // LDR Değerlerini Oku
  int ldrSol = analogRead(ldrSolPin);
  int ldrSag = analogRead(ldrSagPin);
  
  int fark = ldrSol - ldrSag;
  
  // Servo Hareket Mekanizması
  if (fark > tolerans) {
    if (servoAcisi < 180) servoAcisi += 1;
  }
  else if (fark < -tolerans) {
    if (servoAcisi > 0) servoAcisi -= 1;
  }
  
  gunesServosu.write(servoAcisi);
  
  // LCD Ekran Güncelleme
  lcd.setCursor(0, 0);
  lcd.print("Aci: ");
  lcd.print(servoAcisi);
  lcd.print(" derecing  "); 
  
  lcd.setCursor(0, 1);
  lcd.print("L:");
  lcd.print(ldrSol);
  lcd.print(" R:");
  lcd.print(ldrSag);
  lcd.print("   ");

  // Belirli aralıklarla SD Karta Veri Kaydetme
  if (millis() - sonKayitZamani >= kayitAraligi) {
    sonKayitZamani = millis();
    veriKaydet(servoAcisi, ldrSol, ldrSag);
  }
  
  delay(50); 
}

// SD Karta Yazma Fonksiyonu
void veriKaydet(int aci, int solLdr, int sagLdr) {
  File veriDosyasi = SD.open("data.txt", FILE_WRITE);
  
  if (veriDosyasi) {
    // CSV formatında kaydet: Zaman(ms), Açı, Sol_LDR, Sağ_LDR
    veriDosyasi.print(millis());
    veriDosyasi.print(",");
    veriDosyasi.print(aci);
    veriDosyasi.print(",");
    veriDosyasi.print(solLdr);
    veriDosyasi.print(",");
    veriDosyasi.println(sagLdr);
    
    veriDosyasi.close(); 
    Serial.println("Veri kaydedildi.");
  } else {
    Serial.println("Dosya acilamadı!");
    lcd.setCursor(14, 0);
    lcd.print("Err"); 
  }
}