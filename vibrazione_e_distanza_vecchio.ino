const int pinDistanza = A1;
const int pinVibrazione = 4;    
const int pinInterruttoreDistanza = 2;
const int pinInterruttoreVibrazione = 3;

volatile int state = 0;
bool letturaDistanzaAttiva = false;
bool letturaVibrazioneAttiva = false;

unsigned long ultimoAggiornamento = 0;
const unsigned long intervallo = 3000; // 3 secondi

void setup() {
  Serial.begin(9600);

  pinMode(pinDistanza, INPUT);
  pinMode(pinVibrazione, INPUT);

  // Interruttori collegati a GND → serve PULLUP
  pinMode(pinInterruttoreDistanza, INPUT_PULLUP);
  pinMode(pinInterruttoreVibrazione, INPUT_PULLUP);

  // Interrupt vibrazione
  attachInterrupt(digitalPinToInterrupt(pinVibrazione), blink, RISING);
}

void loop() {

  // -------------------------------
  // LETTURA DIRETTA DEGLI INTERRUTTORI
  // -------------------------------
  // Interruttori a massa ⇒ LOW = acceso, HIGH = spento
  letturaDistanzaAttiva = (digitalRead(pinInterruttoreDistanza) == LOW);
  letturaVibrazioneAttiva = (digitalRead(pinInterruttoreVibrazione) == LOW);

  // Messaggi stato (opzionale, solo quando cambia)
  static bool statoPrecedenteD = false;
  static bool statoPrecedenteV = false;

  if (letturaDistanzaAttiva != statoPrecedenteD) {
    if (letturaDistanzaAttiva) Serial.println("\nSensore DISTANZA ATTIVO");
    else Serial.println("\nSensore DISTANZA DISATTIVATO");
    statoPrecedenteD = letturaDistanzaAttiva;
  }

  if (letturaVibrazioneAttiva != statoPrecedenteV) {
    if (letturaVibrazioneAttiva) Serial.println("\nSensore VIBRAZIONE ATTIVO");
    else Serial.println("\nSensore VIBRAZIONE DISATTIVATO");
    statoPrecedenteV = letturaVibrazioneAttiva;
  }

  // -------------------------------
  // ESECUZIONE OGNI 3 SECONDI
  // -------------------------------
  if (millis() - ultimoAggiornamento >= intervallo) {
    ultimoAggiornamento = millis();

    // ------- Sensore distanza --------
    if (letturaDistanzaAttiva) {
      int valoreAnalogico = analogRead(pinDistanza);
      float tensione = valoreAnalogico * (3.3 / 1023.0);
      float distanzaCM = 27.86 * pow(tensione, -1.15);

      Serial.print("Distanza: ");
      Serial.print(distanzaCM);
      Serial.println(" cm");
    } else {
      Serial.println("Sensore distanza disattivato.");
    }

    // ------- Sensore vibrazione --------
    if (letturaVibrazioneAttiva) {
      if (state != 0) {
        state = 0;
        Serial.println("VIBRAZIONE RILEVATA!");
      } else {
        Serial.println("Nessuna vibrazione.");
      }
    } else {
      Serial.println("Sensore vibrazione disattivato.");
    }

    Serial.println("---------");
  }
}

void blink() {
  if (letturaVibrazioneAttiva) {
    state++;
  }
}
