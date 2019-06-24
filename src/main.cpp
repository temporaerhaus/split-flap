#include <Arduino.h>

// NOTE: ADL has to be always low.
#define ADC_PIN   D6 //# gpio12
#define START_PIN D8 //# gpio15

#define PIN_EC_0 D7
#define PIN_EC_1 D1
#define PIN_EC_2 D2
#define PIN_EC_3 D3
#define PIN_EC_4 D4
#define PIN_EC_5 D5

void setup() {
  Serial.begin(9600);
  delay(100);
  //Serial.setDebugOutput(true);
  Serial.println("flapflapflap\n");
  
  pinMode(START_PIN, OUTPUT);
  digitalWrite(START_PIN, LOW);

  pinMode(ADC_PIN, OUTPUT);
  digitalWrite(ADC_PIN, LOW);

  pinMode(PIN_EC_0, INPUT_PULLUP);
  pinMode(PIN_EC_1, INPUT_PULLUP);
  pinMode(PIN_EC_2, INPUT_PULLUP);
  pinMode(PIN_EC_3, INPUT_PULLUP);
  pinMode(PIN_EC_4, INPUT_PULLUP);
  pinMode(PIN_EC_5, INPUT_PULLUP);
}

volatile uint8_t encoder_val = 0; // last read value

void readNoWrite() {
  encoder_val = digitalRead(PIN_EC_0);
  encoder_val += digitalRead(PIN_EC_1) << 1;
  encoder_val += digitalRead(PIN_EC_2) << 2;
  encoder_val += digitalRead(PIN_EC_3) << 3;
  encoder_val += digitalRead(PIN_EC_4) << 4;
  encoder_val += digitalRead(PIN_EC_5) << 5;
}

void printSingle() {
  Serial.print("0: "); Serial.print(digitalRead(PIN_EC_0)); Serial.println("");
  Serial.print("1: "); Serial.print(digitalRead(PIN_EC_1)); Serial.println("");
  Serial.print("2: "); Serial.print(digitalRead(PIN_EC_2)); Serial.println("");
  Serial.print("3: "); Serial.print(digitalRead(PIN_EC_3)); Serial.println("");
  Serial.print("4: "); Serial.print(digitalRead(PIN_EC_4)); Serial.println("");
  Serial.print("5: "); Serial.print(digitalRead(PIN_EC_5)); Serial.println("");
}

void read() {
  digitalWrite(ADC_PIN, HIGH);
  readNoWrite();
  digitalWrite(ADC_PIN, LOW);
}

void print() {
  Serial.print("encoder: ");
  Serial.print(encoder_val);
  Serial.print(" bin: ");
  Serial.print(encoder_val, BIN);
  Serial.println("");
}

void readAndPrint() {
  read();
  if (encoder_val == 62) return;
  if (encoder_val == 63) return;
  print();
}

void flap() {
  uint8_t pre = encoder_val;
  digitalWrite(START_PIN, HIGH);
  // happens inside the first read
  //digitalWrite(ADC_PIN, HIGH);
  //digitalWrite(ADC_PIN, LOW);
  do {
    read();
    delay(1);
  } while (encoder_val == 63 || encoder_val == 62 || encoder_val == pre);
  digitalWrite(START_PIN, LOW);
  // flank down:
  digitalWrite(ADC_PIN, HIGH);
  digitalWrite(ADC_PIN, LOW);
}

void flapUntil(int target) {
  digitalWrite(START_PIN, HIGH);
  // happens inside the first read
  //digitalWrite(ADC_PIN, HIGH);
  //digitalWrite(ADC_PIN, LOW);
  do {
    readAndPrint();
    delay(1);
  } while (encoder_val != target);
  digitalWrite(START_PIN, LOW);
  // flank down:
  digitalWrite(ADC_PIN, HIGH);
  digitalWrite(ADC_PIN, LOW);
}

void loop() {
 if (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == 's'){
      Serial.println("START: low");
      digitalWrite(START_PIN, LOW);
    }
    if (ch == 'S'){
      Serial.println("START: high");
      digitalWrite(START_PIN, HIGH);
    }
    if (ch == 'a'){
      Serial.println("ADC: low");
      digitalWrite(ADC_PIN, LOW);
    }
    if (ch == 'A'){
      Serial.println("ADC: high");
      digitalWrite(ADC_PIN, HIGH);
    }
    if (ch == 't'){
      Serial.println("ADC: flank down");
      digitalWrite(ADC_PIN, HIGH);
      delay(25);
      digitalWrite(ADC_PIN, LOW);
    }
    if (ch == 'r') {
      readAndPrint();
    }
    if (ch == 'R') {
      readNoWrite();
      print();
    }
    if (ch == 'p') {
      print();
    }
    if (ch == 'P') {
      printSingle();
    }
    if (ch == 'f') {
      Serial.println("single flap");
      flap();
    }
    if (ch == 'z') {
      int tries = 0;
      readAndPrint();
      while(encoder_val != 2 && tries < 75) {
        flap();
        readAndPrint();
        tries++;
        Serial.print("try: ");
        Serial.print(tries);
        Serial.println("");
        delay(50);
      }
    }
    if (ch == 'Z') {
      flapUntil(50);
    }
  }
}
