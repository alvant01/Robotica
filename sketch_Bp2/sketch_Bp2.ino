#include "Wire.h"
#include "CMPS03.h"
CMPS03 cmps03;

void setup() {
 Serial.begin(9600);
 Wire.begin();
}

void loop() {
 Serial.print ("Bearing: ");
 Serial.println(cmps03.read());
 delay(1000);
}
