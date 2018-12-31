#include "Arduino.h"
#include <Wire.h>
#include <GT811.h>

GT811::GT811(uint16_t _RST, uint16_t _INT) {
  RST = _RST;
  INT = _INT;
  touches = 0;
}

void GT811::begin() {
  pinMode(INT, INPUT);
  pinMode(RST, OUTPUT);
  Wire.begin();
  Wire.setClock(400000);
  digitalWrite(RST,LOW);
  delay(100);
  digitalWrite(RST, HIGH);
  delay(100);
  
  Wire.beginTransmission(byte(GT811_ADDRESS >> 1));
  Wire.write(byte(GT811_REGISTERS_CONFIGURATION >> 8));
  Wire.write(byte(GT811_REGISTERS_CONFIGURATION & 0xFF));
  for(int i = 0; i < (sizeof(gt811_config)/sizeof(gt811_config[0])); i++){
      Wire.write(byte(gt811_config[i]));
  }
  Wire.endTransmission();  
  
}

TS_Point GT811::getPoint(uint8_t n) {
  if ((touches == 0) || (n > 5) || (n < 1)) {
    return TS_Point(0, 0, 0);
  } else {
    return TS_Point(touchX[n-1], touchY[n-1], touchPressure[n-1]);
  }
}

uint16_t GT811::poll(void) {
	if(digitalRead(INT) == 0){
		Wire.beginTransmission(byte(GT811_ADDRESS >> 1));
		Wire.write(byte(GT811_REGISTERS_READ >> 8));
		Wire.write(byte(GT811_REGISTERS_READ & 0xFF));
		Wire.endTransmission();
		Wire.requestFrom(GT811_ADDRESS >> 1, 34);
		int counter = 1;
		byte rawData[28];
		int i = 0;
		while (Wire.available()) {
		   int rdata = Wire.read();
		   if ( !((counter > 17) && (counter < 24))){
			 rawData[i] = rdata;
			 i++;
		   }       
		   counter++;
		}
		int count = 0;
		int rdata = rawData[0];
		while(rdata){
			count += rdata & 1;
			rdata >>= 1;
		}
		touches = count;
		if (touches > 0){
		   for (int i  = 0; i < touches; i++){
				touchX[i] = (rawData[(i*5) + 2] << 8) | (rawData[(i*5) + 3] & 0xFF);
				touchY[i] = (rawData[(i*5) + 4] << 8) | (rawData[(i*5) + 5] & 0xFF);
				touchPressure[i] = rawData[(i*5) + 6];
			 }
		   }  	
	   Wire.endTransmission();  
	} else {
		touches = 0;		
	}
	return touches;
}

TS_Point::TS_Point(void) {
  x = y = pressure = 0;
}

TS_Point::TS_Point(int16_t _x, int16_t _y, int16_t _pressure) {
  x = _x;
  y = _y;
  pressure = _pressure;
}


bool TS_Point::operator==(TS_Point p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.pressure == pressure));
}


bool TS_Point::operator!=(TS_Point p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.pressure != pressure));
}



