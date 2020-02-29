#include <RH_RF95.h>

#define RFM_CS 10
#define RFM_RESET 9
#define RFM_INT 2

#define RFM_FREQ 433.0

RH_RF95 radio(RFM_CS, RFM_INT);

void (*reset)(void) = 0;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
  while (!Serial);
  
  pinMode(RFM_RESET, OUTPUT);
  digitalWrite(RFM_RESET, HIGH);
  delay(10);
  digitalWrite(RFM_RESET, LOW);
  delay(10);
  digitalWrite(RFM_RESET, HIGH);
  delay(10);

  if (!radio.init()) {
    Serial.println("INIT_FAIL");
    while(true);
  } else {
    Serial.println("INIT_OK");
  }

  if (!radio.setFrequency(RFM_FREQ)) {
    Serial.println("FREQ_FAIL");
    while(true);
  } else {
    Serial.println("FREQ_OK");
  }  
}

void loop() {
  Serial.println("READY");

  /* --- Transmit --- */
  String txString;
  while ((txString = Serial.readString()).length() == 0); // Wait for data to send

  uint8_t txLength = txString.length();
  byte* txBuffer = new byte[txLength];
  memcpy(txBuffer, txString.c_str(), txLength);
  
  radio.send(txBuffer, txLength);
  radio.waitPacketSent();

  /* --- Receive --- */
  uint8_t rxLength = RH_RF95_MAX_MESSAGE_LEN;
  byte rxBuffer[rxLength];

  if (radio.waitAvailableTimeout(5000)) {
    if (radio.recv(rxBuffer, &rxLength)) {
      Serial.println("REPLY");
      Serial.println((char)rxLength);

      // rxBuffer doesn't have a null-terminator so manually outputing array
      for (uint8_t i = 0; i < rxLength; i++)
        Serial.print((char)rxBuffer[i]);

      Serial.print("\r\n");
    } else {
      Serial.println("REPLY_FAIL");
    }
  } else {
    Serial.println("NO_REPLY");
  }
}
