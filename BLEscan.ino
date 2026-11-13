#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// generated script to test radio
// CE, CSN pins
RF24 radio(9, 8);

// Communication address (must match transmitter)
const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  // Initialize radio
  if (!radio.begin()) {
    Serial.println("Radio hardware not responding!");
    while (1) {
      delay(1000);
    }
  }

  // Configure communication
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(76);

  // Open reading pipe
  radio.openReadingPipe(0, address);

  // Start receiving
  radio.startListening();

  Serial.println("NRF24L01 Receiver Ready!");
  Serial.println("Waiting for messages...");
}

void loop() {

  if (radio.available()) {

    char receivedMessage[32] = {0};

    radio.read(&receivedMessage, sizeof(receivedMessage));

    Serial.print("Received: ");
    Serial.println(receivedMessage);
  }
}