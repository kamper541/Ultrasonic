#include <GoGoBoardArduino.h>
#include <Wire.h>        // include Arduino Wire library
#include "Ultrasonic.h"  // include Seeed Studio ultrasonic ranger library
#include <numeric>

#define GOGO_DEFAULT_BUFFER_SIZE            256
#define RANGERPIN                           2
#define BYTE_HEADER_OFFSET                  4
#define BYTE_PACKET_LENGTH                  3
#define gogoSerial                          Serial1
#define numType                             1
#define stringType                          2

//GoGoBoardArduino GoGoBoard;

// initialize ultrasonic library
Ultrasonic ultrasonic(RANGERPIN);

String dataStr;
uint8_t reportPkt[GOGO_DEFAULT_BUFFER_SIZE] = {0x54, 0xfe, 31};
uint8_t dataPkt[GOGO_DEFAULT_BUFFER_SIZE] = {0};

void sendDist(const String &key,const int centi);
void sendReportPacket(uint8_t *data,uint8_t length);
 
void setup() {
  GoGoBoard.begin();
  Serial.begin(9600);
  GoGoBoard.beep();
} 
 
// main loop
void loop() {
  int centimeters = ultrasonic.MeasureInCentimeters();
  int inches = ultrasonic.MeasureInInches();
//  int millism = ultrasonic.MeasureInMillimeters();
  delay(250);  // wait 250 milliseconds between readings

  Serial.println(centimeters);
  sendDist("data",centimeters);
}

void sendDist(const String &key, const int centi){
  dataStr = key + "," + String(centi);

  *(dataPkt) = numType;
  *(dataPkt + 1) = sizeof dataStr;
  memcpy(dataPkt + 2, dataStr.c_str(), sizeof dataStr);

  sendReportPacket(dataPkt, (sizeof dataStr) + 2);
}

void sendReportPacket(uint8_t *data, uint8_t length){
  reportPkt[BYTE_PACKET_LENGTH] = length + 1;

  memcpy(reportPkt + BYTE_HEADER_OFFSET, data, length);
  reportPkt[length + BYTE_HEADER_OFFSET] = std::accumulate(reportPkt + BYTE_HEADER_OFFSET, reportPkt + BYTE_HEADER_OFFSET + length, 0);//sum

  delay(10);
  gogoSerial.write(reportPkt, length + BYTE_HEADER_OFFSET + 1);
}