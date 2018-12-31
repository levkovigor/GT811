#include <Wire.h>
#include "GT811.h"

GT811 gt = GT811(2, 4); //RESET Pin, Interrupt Pin
TS_Point p;

void setup() {
  Serial.begin(115200);
  gt.begin();
}

void loop() {
  int touches = gt.poll();
  if (touches > 0){
     for (int i = 1; i <= touches; i++){
          p = gt.getPoint(i);
          Serial.print("(");
          Serial.print(i);
          Serial.print(", "); Serial.print(p.x);
          Serial.print(", "); Serial.print(p.y);
          Serial.println(")");
     }
  }
}
