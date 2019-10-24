/* Remote */


/* Libraries */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/* Directives and Macros */
#define PIPE_ADDRESS_SIZE  5    //Only 5 byte!
#define BUFFER_SIZE        4   //Size of TX and RX buffers


#define UART_SPEED_48    4800   //UART speeds
#define UART_SPEED_96    9600
#define UART_SPEED_288   28800
#define UART_SPEED_115   115200

#define TX_PIN_LED 6            //Pins numbers
#define RX_PIN_LED 5

/* Variables */
boolean RxState = false;
boolean TxState = true;

/* Arrays */
uint8_t TxBuffer[BUFFER_SIZE];
uint8_t RxBuffer[BUFFER_SIZE];

uint8_t TxAddresses[PIPE_ADDRESS_SIZE] = {0x0A, 0x0A, 0x0A, 0x0A, 0x01};  //TX pipeline address
uint8_t RxAddresses[PIPE_ADDRESS_SIZE] = {0x0B, 0x0B, 0x0B, 0x0B, 0x02};  //RX pipeline address

/* Prototypes */
boolean doubleBlink(uint8_t ledPin_1, uint8_t ledPin_2, uint16_t blinkTime);
void bufferReset(uint8_t *buf, uint8_t bufSize);

RF24 remote(7,8);

void setup() {
  /* UART init */
  Serial.begin(UART_SPEED_96, SERIAL_8E1);                        //UART 8 bits with EVEN mode - że bit parzystości
  Serial.println("\nRemote application start\nUART init OK\n");
  delay(10);
  
  /* GPIO init */ 
  pinMode(TX_PIN_LED, OUTPUT);
  pinMode(RX_PIN_LED, OUTPUT); 
  Serial.println("\LEDs init OK\nTX LED pin: 6 \nRX LED pin: 5 \n");
  delay(10);
  
  /* nRF24L01+ init */
  remote.begin();
  remote.openWritingPipe(TxAddresses);
  remote.openReadingPipe(1, RxAddresses);
  Serial.println("\nNRF24 init OK\n Set TX and RX pipeline addresses\n");
  delay(10);
  
  /* Clean buffers */
  bufferReset(TxBuffer, sizeof(TxBuffer));
  bufferReset(RxBuffer, sizeof(RxBuffer));
  Serial.println("\nTX and RX buffers RESET OK\n");

}

void loop() {
  remote.stopListening(); 
  digitalWrite(TX_PIN_LED, HIGH); 

}



/* Functions */

/* Blink two LEDs */
boolean doubleBlink(uint8_t ledPin_1, uint8_t ledPin_2, uint16_t blinkTime) {
  if ( ((ledPin_1 < 0) || (ledPin_1 > 13)) || ((ledPin_2 < 0) || (ledPin_2 > 13)) ) {
    return false;
  }
  if ( blinkTime > 10000) {   //jezeli wieksze do 10 sek
    blinkTime = 2000;
  }

  if ( digitalRead(ledPin_1) == HIGH || digitalRead(ledPin_2) == HIGH ) {       // high high
    digitalWrite(ledPin_1, LOW);  digitalWrite(ledPin_2, LOW);
    delay(blinkTime);
    digitalWrite(ledPin_1, HIGH); digitalWrite(ledPin_2, HIGH);
    return true;

  } else if ( digitalRead(ledPin_1) == LOW || digitalRead(ledPin_2) == LOW ) {    //low low
    digitalWrite(ledPin_1, HIGH); digitalWrite(ledPin_2, HIGH);
    delay(blinkTime);
    digitalWrite(ledPin_1, LOW);  digitalWrite(ledPin_2, LOW);
    return true;

  } else if ( digitalRead(ledPin_1) == LOW || digitalRead(ledPin_2) == HIGH) {    //low high
    digitalWrite(ledPin_1, HIGH); digitalWrite(ledPin_2, LOW);
    delay(blinkTime);
    digitalWrite(ledPin_1, LOW);  digitalWrite(ledPin_2, HIGH);
    return true;

  } else if ( digitalRead(ledPin_1) == HIGH || digitalRead(ledPin_2) == LOW ) {   //high low
    digitalWrite(ledPin_1, LOW); digitalWrite(ledPin_2, HIGH);
    delay(blinkTime);
    digitalWrite(ledPin_1, HIGH);  digitalWrite(ledPin_2, LOW);
    return true;
  } else {
    return false;
  }
}

/* Clean arrays functions */
void bufferReset(uint8_t *buf, uint8_t bufSize) {            //Funkcja resetowanai bufora
  for (uint8_t i = 0; i < bufSize; i++) {
    buf[i] = 0;
  }
}
