#include <Servo.h>
#include <HCSR04.h>

// Çıkış Pinleri
#define servoPin 9
#define buzzerPin 8
#define kirmiziLed 7
#define sariLed 6
#define yesilLed 5

// Ultrasonik Sensör (Trig: 13, Echo: 12)
UltraSonicDistanceSensor distanceSensor(13, 12);
Servo radarServo;

int mesafe;

void setup() {
  Serial.begin(9600);
  radarServo.attach(servoPin);
  
  pinMode(buzzerPin, OUTPUT);
  pinMode(kirmiziLed, OUTPUT);
  pinMode(sariLed, OUTPUT);
  pinMode(yesilLed, OUTPUT);
  
  digitalWrite(buzzerPin, LOW);
  digitalWrite(kirmiziLed, LOW);
}

// Durum ve Uyarı Kontrol Fonksiyonu
void tehlikeKontrol(int d) {
  // 1. KRİTİK TEHLİKE: 12 cm'den yakın
  if (d > 0 && d <= 12) {
    digitalWrite(kirmiziLed, HIGH);
    digitalWrite(sariLed, LOW);
    digitalWrite(yesilLed, LOW);

    // Çok seri ve acil bip uyarısı
    digitalWrite(buzzerPin, HIGH);
    delay(20);
    digitalWrite(buzzerPin, LOW);
  }
  // 2. ORTA TEHLİKE: 13 cm ile 28 cm arası
  else if (d > 12 && d <= 28) {
    digitalWrite(kirmiziLed, LOW);
    digitalWrite(sariLed, HIGH);
    digitalWrite(yesilLed, LOW);

    // Belirli aralıklarla kesikli bip (açı çift sayıyken öter)
    // Bu sayede tarama hızını kilitlemeden aralıklı ses çıkarır
    static int bipSayac = 0;
    if (++bipSayac % 4 == 0) {
      digitalWrite(buzzerPin, HIGH);
      delay(25);
      digitalWrite(buzzerPin, LOW);
    }
  }
  // 3. GÜVENLİ DURUM: 28 cm'den uzak veya cisim yok
  else {
    digitalWrite(kirmiziLed, LOW);
    digitalWrite(sariLed, LOW);
    digitalWrite(yesilLed, HIGH);
    digitalWrite(buzzerPin, LOW);
  }
}

void loop() {
  // 15 dereceden 165 dereceye doğru tarama
  for (int aci = 15; aci <= 165; aci++) {
    radarServo.write(aci);
    delay(30); // Motorun pozisyon alması için bekleme

    mesafe = distanceSensor.measureDistanceCm();
    tehlikeKontrol(mesafe);

    // Bilgisayardaki radar arayüzü için veri formatı: "açı,mesafe."
    Serial.print(aci);
    Serial.print(",");
    Serial.print(mesafe);
    Serial.print(".");
  }

  // 165 dereceden 15 dereceye geri dönüş taraması
  for (int aci = 165; aci >= 15; aci--) {
    radarServo.write(aci);
    delay(30);

    mesafe = distanceSensor.measureDistanceCm();
    tehlikeKontrol(mesafe);

    Serial.print(aci);
    Serial.print(",");
    Serial.print(mesafe);
    Serial.print(".");
  }
}