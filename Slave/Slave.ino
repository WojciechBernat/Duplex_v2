/* Slave */

/* Libraries */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/* Directives and Macros */
#define PIPE_ADDRESS_SIZE  5    //Only 5 byte!
#define BUFFER_SIZE       32   //Size of TX and RX buffers


#define UART_SPEED_48    4800   //UART speeds
#define UART_SPEED_96    9600
#define UART_SPEED_288   28800
#define UART_SPEED_115   115200

#define TX_PIN_LED 5            //Pins numbers
#define RX_PIN_LED 6

/* Variables */
boolean RxState = false;
boolean TxState = false;

boolean RxRole = true;
boolean TxRole = false;

uint8_t TxCounter = 0x00;
uint8_t RxCounter = 0x00;
uint8_t ChangeRole = 0x0F;

//uint32_t TimeExecute = 0; niepotrzebne
uint32_t TxTimeExecute = 0;
uint32_t RxTimeExecute = 0;
uint16_t blinkTime = 200;

/* Arrays */
uint8_t TxBuffer[BUFFER_SIZE];
uint8_t RxBuffer[BUFFER_SIZE];

uint8_t TxAddresses[PIPE_ADDRESS_SIZE] =  {0x0B, 0x0B, 0x0B, 0x0B, 0x02};  //TX pipeline address
uint8_t RxAddresses[PIPE_ADDRESS_SIZE] =  {0x0A, 0x0A, 0x0A, 0x0A, 0x01};   //RX pipeline address

/* Prototypes */
boolean doubleBlink(uint8_t ledPin_1, uint8_t ledPin_2, uint16_t blinkTime);
void bufferReset(uint8_t *buf, uint8_t bufSize);

RF24 receiver(7, 8);

void setup() {
  /* UART init */
  Serial.begin(UART_SPEED_96, SERIAL_8E1);                        //UART 8 bits with EVEN mode - że bit parzystości
  Serial.println("\nreceiver application start\nUART init OK\n");
  delay(10);

  /* GPIO init */
  pinMode(TX_PIN_LED, OUTPUT);
  pinMode(RX_PIN_LED, OUTPUT);
  doubleBlink(TX_PIN_LED, RX_PIN_LED, blinkTime);
  Serial.println("\nLEDs init OK\nTX LED pin: 6 \nRX LED pin: 5 \n");
  delay(10);

  /* nRF24L01+ init */
  receiver.begin();
  receiver.openWritingPipe(TxAddresses);
  receiver.openReadingPipe(1, RxAddresses);
  receiver.setPALevel(RF24_PA_MIN);
  Serial.println("\nNRF24 init OK\n Set TX and RX pipeline addresses\n");
  delay(10);

  /* Clean buffers */
  bufferReset(TxBuffer, sizeof(TxBuffer));
  bufferReset(RxBuffer, sizeof(RxBuffer));
  Serial.println("\nTX and RX buffers RESET OK\n");

  receiver.startListening();
}

void loop() {
  /* Start receive */
  RxTimeExecute = micros();                      //time execute measure
  receiver.startListening();

  if (true) {                                   //if module is Receiver
    digitalWrite(RX_PIN_LED, HIGH);
    if(receiver.available()) {                   //receiving data while they are in nRF24 FIFO's buffer
      receiver.read(RxBuffer, BUFFER_SIZE);     //read data
      RxState = true;
    }
    else {
      //RxState = false;
    }
    digitalWrite(RX_PIN_LED, LOW);
    Serial.println("\nReceive state: " + String(RxState) + "\n" );
  }
  /* End of receive */


  RxTimeExecute = micros() - RxTimeExecute;
  Serial.println("\nRx execute time: " + (String(RxTimeExecute)) + " us\n" );    //Print time of execute

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
