#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Crypto.h>
#include <SHA256.h>
#include <stdint.h>
#include <string.h>

SHA256 sha256;

#define NEOPIXEL_POWER 23
#define PIN_NEOPIXEL 22
#define NUM_LEDS 1
#define USER_LED 25
Adafruit_NeoPixel pixels(NUM_LEDS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
/*

// Linear finding

int R;
int G;
int B;
int size = 3;
int findColors[3] = {50, 47, 157};
int attempt = 1;
int *array = NULL;

void setup() {
  pinMode(NEOPIXEL_POWER, OUTPUT);
  pinMode(USER_LED, OUTPUT);
  digitalWrite(USER_LED, LOW);
  digitalWrite(NEOPIXEL_POWER, HIGH);

  pixels.begin();
  pixels.clear();
  pixels.show();

  Serial.begin(115200);

  array = (int*)malloc(size * sizeof(int));
  if (array == NULL){
    Serial.print("Cant create array");
  }
}

void findedColor(int numberOfAttempts){
  Serial.print("Number of attempts: ");
  Serial.print(numberOfAttempts);
  while(1){
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(500);
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
    delay(500);
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
    delay(500);
  }
}

void generateColorForArray(){

  size = attempt * 3;

  int *newArray = (int*)realloc(array, size * sizeof(*array));

  if (newArray == NULL) {
    Serial.println("REALLOC FAILED");
    return;
  }

  array = newArray;
  for(int i = 0; i < size; i++){
    array[i] = rand() % 256;
  }
  for(int j = 0; j < attempt; j++){
    R = array[j * 3];
    G = array[j * 3 + 1];
    B = array[j * 3 + 2];
    Serial.print(j);
    Serial.println("R: ");
    Serial.println(R);
    Serial.print(j);
    Serial.println("G: ");
    Serial.println(G);
    Serial.print(j);
    Serial.println("B: ");
    Serial.println(B);
    pixels.setPixelColor(0, pixels.Color(R, G, B));
    pixels.show();
    if((findColors[0] == R) && (findColors[1] == G) && (findColors[2] == B)){
    findedColor(attempt);
    }
  }
  Serial.print("Current attempt: ");
  Serial.println(attempt);
  attempt++;
}

void loop() {
  generateColorForArray();
  delay(500);
}*/

// Exponential finding -- RGB

/*


int R;
int G;
int B;
int findColors[3] = {50, 47, 157};
int attempt = 1;

void setup() {
  pinMode(NEOPIXEL_POWER, OUTPUT);
  pinMode(USER_LED, OUTPUT);
  digitalWrite(USER_LED, LOW);
  digitalWrite(NEOPIXEL_POWER, HIGH);

  pixels.begin(); 
  pixels.clear();
  pixels.show();
}

void findedColor(int numberOfAttempts){
  Serial.print("Number of attempts: ");
  Serial.println(numberOfAttempts);
  while(1){
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    delay(500);
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
    delay(500);
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();
    delay(500);
  }
}

int numberOfColors = 1;

void generateColorForArray() {

  numberOfColors *= 2;

  for (int j = 0; j < numberOfColors; j++) {

    R = rand() % 256;
    G = rand() % 256;
    B = rand() % 256;

    if (findColors[0] == R &&
        findColors[1] == G &&
        findColors[2] == B) {

      findedColor(attempt);
    }
  }

  Serial.print("Current attempt: ");
  Serial.println(attempt);

  Serial.print("Colors checked: ");
  Serial.println(numberOfColors);

  attempt++;
}

void loop() {
  generateColorForArray();
  delay(500);
}*/

#include <atomic>
SHA256 sha256_0;
SHA256 sha256_1;

// Každý core má vlastní vstup i výstup.
uint8_t input0[16] = {1, 2, 3, 4};
uint8_t input1[16] = {5, 6, 7, 8};

uint8_t digest0[32];
uint8_t digest1[32];

// Počítadla dokončených hashů.
std::atomic<uint32_t> hashes0{0};
std::atomic<uint32_t> hashes1{0};

void setup() {
  Serial.begin(115200);

  pinMode(NEOPIXEL_POWER, OUTPUT);
  pinMode(USER_LED, OUTPUT);

  digitalWrite(USER_LED, LOW);
  digitalWrite(NEOPIXEL_POWER, HIGH);

  pixels.begin();
  pixels.clear();
  pixels.show();
}

void setup1() {
}

// Provede jeden SHA-256 a změní vstup podle výsledku.
inline void calculateHash(
  SHA256 &hasher,
  uint8_t *input,
  uint8_t *digest
) {
  hasher.reset();
  hasher.update(input, 16);
  hasher.finalize(digest, 32);

  // Zajistí, že další vstup závisí na výsledku hashe.
  input[0] = digest[0];
}

void loop() {
  static uint32_t localCount = 0;
  static uint32_t lastReport = millis();

  calculateHash(sha256_0, input0, digest0);
  localCount++;

  // Hromadné aktualizování počítadla snižuje režii.
  if (localCount >= 256) {
    hashes0.fetch_add(localCount, std::memory_order_relaxed);
    localCount = 0;
  }

  uint32_t now = millis();

  if (now - lastReport >= 1000) {
    // Zahrne i nedokončenou dávku hlavního core.
    uint32_t count0 = hashes0.exchange(0, std::memory_order_relaxed);
    uint32_t count1 = hashes1.exchange(0, std::memory_order_relaxed);

    count0 += localCount;
    localCount = 0;

    Serial.print("SHA-256 za sekundu: ");
    Serial.println(count0 + count1);

    lastReport = now;
  }
}

void loop1() {
  static uint32_t localCount = 0;

  calculateHash(sha256_1, input1, digest1);
  localCount++;

  if (localCount >= 256) {
    hashes1.fetch_add(localCount, std::memory_order_relaxed);
    localCount = 0;
  }
}