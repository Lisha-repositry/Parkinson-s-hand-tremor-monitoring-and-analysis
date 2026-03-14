#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050_light.h>
#include <SoftwareSerial.h>
SoftwareSerial BT(2,3);
LiquidCrystal_I2C lcd(0x27, 16, 2);
MPU6050 mpu(Wire);

//Dummy flex sensor pin
int flexPin = A0; 

// LED PINS
int ledGreen = 8;
int ledYellow = 9;
int ledRed = 10;

// Buzzer
int buzzerPin = 11;

// NEW tremor thresholds
float mildThr = 2.0;   // Mild tremor start
float highThr = 3.0;   // High tremor start

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  Wire.begin();

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  delay(500);

  // MPU INIT
  byte status = mpu.begin();
  if (status != 0) {
    lcd.clear();
    lcd.print("MPU ERROR");
    while (1);
  }

  mpu.calcOffsets();
  lcd.clear();
  lcd.print("Ready");
  delay(1000);

  pinMode(ledGreen, OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  mpu.update();

  // Calculate acceleration magnitude
  float accValue = abs(mpu.getAccX()) +
                   abs(mpu.getAccY()) +
                   abs(mpu.getAccZ());

  // Dummy flex reading (only for Serial Monitor)
  int flexValue = analogRead(flexPin);

  // ----- SERIAL MONITOR -----
  Serial.print("ACC = ");
  Serial.print(accValue, 2);

  Serial.print("   |   Flex = ");
  Serial.println(flexValue);

  String status;

  // ----- CONDITIONS -----
  if (accValue < mildThr) {
    status = "Normal";
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(buzzerPin, LOW);
  }
  else if (accValue >= mildThr && accValue < highThr) {
    status = "Mild Tremor";
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, HIGH);
    digitalWrite(ledRed, LOW);
    digitalWrite(buzzerPin, LOW);
    BT.println("WARNING: MILD TREMOR DETECTED");

    Serial.println("WARNING: Mild Tremor Detected");
  }
  else {
    status = "High Tremor!";
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYellow, LOW);
    digitalWrite(ledRed, HIGH);
    digitalWrite(buzzerPin, HIGH);
    BT.println("ALERT: HIGH TREMOR DETECTED!");

    Serial.println("ALERT: HIGH TREMOR DETECTED!");
  }

  // ----- LCD DISPLAY -----
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACC:");
  lcd.print(accValue, 1);   // one decimal only

  lcd.setCursor(0, 1);
  lcd.print(status);

  delay(150);
}
