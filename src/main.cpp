/*
  Truck-07 · Simple UART protocol - SENDER (ESP32)
  Every 2 s sends:   <DATA,91.4,62,73.4>     (temp, speed, fuel)
  and waits for:     <OK>                    (max 3 tries)
  Board wiring (7Semi ESP32-S3 + EC200U): ESP32 TX=GPIO13 -> EC200 UART1 RX, ESP32 RX=GPIO12 <- EC200 UART1 TX
*/

#include <Arduino.h>

HardwareSerial modemUart(2);           // UART2
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  modemUart.begin(115200, SERIAL_8N1, 12, 13);   // RX 12, TX 13
  modemUart.setTimeout(500);     
  Serial.println("Modem UART initialized");                // wait max 500 ms for a reply
}

void loop() {
  if (millis() - lastSend < 2000) return;
  lastSend = millis();

  // 1. Readings (fake for now)
  float temp  = random(880, 960) / 10.0;
  int   speed = random(40, 81);
  float fuel  = random(600, 750) / 10.0;

  // 2. Make the message:  <DATA,temp,speed,fuel>
  String msg = "<DATA," + String(temp, 1) + "," + String(speed) + "," + String(fuel, 1) + ">";

  // 3. Send it and wait for <OK>. No OK? Try again (3 times)
  for (int attempt = 1; attempt <= 3; attempt++) {
    modemUart.print(msg);
    Serial.println("Sent: " + msg);

    String reply = modemUart.readStringUntil('>');   // read until '>' or 500 ms
    if (reply.indexOf("<OK") >= 0) {
      Serial.println("  got OK");
      return;
    }
    Serial.println("  no OK, sending again...");
  }
  Serial.println("  FAILED: is the other side connected?");
}