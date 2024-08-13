#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3

#define TIMEOUT 1500  // milisegundos

#define JOGADOR 4

RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL };

byte payload[7] = { 0, 1, 2, 3, 4, 5, 6 };
byte payloadRx[7] = "    ";
uint8_t origem = 4;
uint8_t indice = 0;
uint8_t rede = 12;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  }

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setChannel(100);
  radio.setPayloadSize(sizeof(payload));
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_2MBPS);

  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);

  printf_begin();
  radio.printPrettyDetails();
}

bool aguardaMsg(int tipo) {
  radio.startListening();
  unsigned long tempoInicio = millis();
  while (millis() - tempoInicio < TIMEOUT) {
    if (radio.available()) {
      uint8_t bytes = radio.getPayloadSize();
      radio.read(&payloadRx[0], bytes);
      if (payloadRx[1] == origem && payloadRx[3] == tipo) {
        radio.stopListening();
        return true;
      }
    }
    radio.flush_rx();
    delay(10);
  }
  radio.stopListening();
  return false;
}

bool sendPacket(byte *pacote, int tamanho, int destino, int controle, int posicao, int jogador) {
  pacote[0] = rede;
  pacote[1] = destino;
  pacote[2] = origem;
  pacote[3] = controle;
  pacote[4] = indice;
  pacote[5] = posicao;
  pacote[6] = jogador;

  for (int i = 0; i < tamanho; i++) {
    Serial.print(pacote[i]);
    Serial.print(" ");
  }
  Serial.println();

  while (1) {
    radio.startListening();
    delayMicroseconds(70);
    radio.stopListening();
    if (!radio.testCarrier()) {
      return radio.write(&pacote[0], tamanho);
    } else {
      Serial.println("Meio Ocupado");
      delayMicroseconds(270);
    }
    radio.flush_rx();
  }
}

void loop() {
  if (Serial.available()) {
    int c = Serial.read() - '0';
    if (c >= 0 && c <= 8) {
      Serial.println(c);

      unsigned long start_timer = micros();
      bool report = sendPacket(&payload[0], sizeof(payload), 22, RTS, c, JOGADOR);
      report = aguardaMsg(CTS);
      if (report) {
        sendPacket(&payload[0], sizeof(payload), 22, MSG, c, JOGADOR);
        report = aguardaMsg(ACK);
      }

      unsigned long end_timer = micros();
      if (report) {
        Serial.println("Sucesso!");
      } else {
        Serial.println("FALHA!");
      }
    }
  }

  radio.flush_rx();
  delay(10);
}
