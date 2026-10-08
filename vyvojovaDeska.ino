#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <stdio.h>
#include <stdlib.h>

#define NEOPIXEL_POWER 23
#define PIN_NEOPIXEL 22
#define NUM_LEDS 1
#define USER_LED 25

Adafruit_NeoPixel pixels(NUM_LEDS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

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
}