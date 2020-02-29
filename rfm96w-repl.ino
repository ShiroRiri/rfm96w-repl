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
    while(1);
  } else {
    Serial.println("INIT_OK");
  }

  if (!radio.setFrequency(RFM_FREQ)) {
    Serial.println("FREQ_FAIL");
    while(1);
  } else {
    Serial.println("FREQ_OK");
  }  
}

void loop() {
  Serial.println("READY");

  String txString;
  char* txBuffer;
  while (txString.length() == 0) {
    txString = Serial.readString();
  }

  txBuffer = new char[txString.length() + 1];
  strcpy(txBuffer, txString.c_str());
  radio.waitPacketSent();

  unsigned char rxBuffer[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(rxBuffer);

  if (radio.waitAvailableTimeout(5000)) {
    if (radio.recv(rxBuffer, &len)) {
      Serial.println("REPLY");
      Serial.println((char*)rxBuffer);
    } else {
      Serial.println("REPLY_FAIL");
    }
  } else {
    Serial.println("NO_REPLY");
  }
}
