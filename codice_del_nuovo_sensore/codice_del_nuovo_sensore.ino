#include "FastIMU.h"
#include <Wire.h>


#define IMU_ADDRESS 0x68
MPU6050 IMU;


calData calib = {0};
AccelData accelData;
GyroData gyroData;
MagData magData;


// -------------------------
// PIN LED + TILT
// -------------------------
const int ledPin = 2;
const int tiltPin = 3;


// -------------------------
// PIN TEMPERATURA
// -------------------------
const int tempPin = A0;


// -------------------------
// INTERUTTORE A 3 POSIZIONI
// -------------------------
const int modeIMU = 6;     // Posizione 1
const int modeLED = 7;     // Posizione 2
const int modeTEMP = 8;    // Posizione 3


unsigned long previousMillis = 0;
const unsigned long interval = 3000;


bool imuInitialized = false;   // Per evitare doppie inizializzazioni


void setup() {
  Serial.begin(115200);


  // MKR1010 usa SDA=11, SCL=12 automaticamente
  Wire.begin();


  // LED + Tilt
  pinMode(ledPin, OUTPUT);
  pinMode(tiltPin, INPUT_PULLUP);


  // Interruttore 3 posizioni
  pinMode(modeIMU, INPUT_PULLUP);
  pinMode(modeLED, INPUT_PULLUP);
  pinMode(modeTEMP, INPUT_PULLUP);
}


void loop() {


  // -------------------------
  // MODALITÀ SELEZIONATA
  // -------------------------
  if (digitalRead(modeIMU) == LOW) {
    runIMU();
  }
  else if (digitalRead(modeLED) == LOW) {
    imuInitialized = false;  // Reset IMU quando si esce dalla modalità
    runLEDandTilt();
  }
  else if (digitalRead(modeTEMP) == LOW) {
    imuInitialized = false;
    runTemperature();
  }
}


// -------------------------
// FUNZIONE 1: LETTURA IMU + CALIBRAZIONE
// -------------------------
void runIMU() {


  // Esegui la calibrazione SOLO la prima volta che entri in modalità IMU
  if (!imuInitialized) {


    Serial.println("Inizializzazione IMU...");
    int err = IMU.init(calib, IMU_ADDRESS);
    if (err != 0) {
      Serial.print("Errore inizializzazione IMU: ");
      Serial.println(err);
      return;
    }


    // --- CALIBRAZIONE COMPLETA ---
    if (IMU.hasMagnetometer()) {
      Serial.println("Muovi l'IMU a 8 per la calibrazione magnetica...");
      delay(3000);
      IMU.calibrateMag(&calib);
      Serial.println("Calibrazione magnetica completata!");
    }


    Serial.println("Mantieni l'IMU ferma e in piano...");
    delay(5000);
    IMU.calibrateAccelGyro(&calib);
    Serial.println("Calibrazione accelerometro/giroscopio completata!");


    // Re-inizializza con i valori calibrati
    IMU.init(calib, IMU_ADDRESS);


    imuInitialized = true;
    Serial.println("IMU pronta!");
  }


  // --- LETTURA DATI IMU ---
  IMU.update();
  IMU.getAccel(&accelData);
  IMU.getGyro(&gyroData);


  Serial.print("ACC: ");
  Serial.print(accelData.accelX); Serial.print("\t");
  Serial.print(accelData.accelY); Serial.print("\t");
  Serial.print(accelData.accelZ); Serial.print("\t");


  Serial.print("GYR: ");
  Serial.print(gyroData.gyroX); Serial.print("\t");
  Serial.print(gyroData.gyroY); Serial.print("\t");
  Serial.print(gyroData.gyroZ);


  if (IMU.hasMagnetometer()) {
    IMU.getMag(&magData);
    Serial.print("\tMAG: ");
    Serial.print(magData.magX); Serial.print("\t");
    Serial.print(magData.magY); Serial.print("\t");
    Serial.print(magData.magZ);
  }


  if (IMU.hasTemperature()) {
    Serial.print("\tTEMP IMU: ");
    Serial.print(IMU.getTemp());
  }


  Serial.println();
  delay(50);
}


// -------------------------
// FUNZIONE 2: LED + TILT
// -------------------------
void runLEDandTilt() {
  if (digitalRead(tiltPin) == LOW) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}


// -------------------------
// FUNZIONE 3: TEMPERATURA LM35
// -------------------------
void runTemperature() {
  unsigned long currentMillis = millis();


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;


    int val = analogRead(tempPin);
    float voltage = val * (3.3 / 1024.0);   // MKR1010 = 3.3V
    float temperature = voltage * 100.0;    // LM35: 10mV per °C


    Serial.print("Temp LM35: ");
    Serial.print(temperature);
    Serial.println(" C");
  }
}




