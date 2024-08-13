#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3

#define JOGADOR1 4
#define JOGADOR2 9

RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

byte payload[8] = {0,1,2,3,4,5,6,7};
byte payloadRx[8] = "       ";

uint8_t origem = 22;
uint8_t indice = 0;   
uint8_t rede = 12;
uint8_t jogador; // Jogador atual

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  if (!radio.begin()) {
    Serial.println(F("O hardware do rádio não está respondendo!!"));
    while (1) {}
  }

  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(100);
  radio.setPayloadSize(sizeof(payload));
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_2MBPS);

  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);

  printf_begin();  // Necessário apenas uma vez para impressão de detalhes
  radio.printPrettyDetails(); // Função que imprime dados legíveis para humanos
}

bool jogada(uint8_t pos) {
  indice += 1;
  payloadRx[5] = pos;
  Serial.println(payloadRx[5] + String(" ") + payloadRx[6]);

  radio.flush_rx();
  return true;
}

bool aguardaMsg(int tipo) {
  radio.startListening();
  unsigned long tempoInicio = millis();
  while (millis() - tempoInicio < 1000) {
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

bool sendPacket(byte *pacote, int tamanho, int destino, int controle) {
  pacote[0] = rede;
  pacote[2] = origem;
  pacote[1] = destino;
  pacote[3] = controle;
  pacote[4] = indice;

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
  // Aguardando RTS
  radio.startListening();
  if (radio.available()) {
    uint8_t bytes = radio.getPayloadSize();
    radio.read(&payloadRx[0], bytes);

    // Se a rede for a minha (12)
    if (payloadRx[0] == rede && (payloadRx[6] == JOGADOR1 || payloadRx[6] == JOGADOR2)) {
      // Se o destino da mensagem for minha origem e o controle for RTS
      if (payloadRx[1] == origem && payloadRx[3] == RTS) {
        bool report = sendPacket(&payloadRx[0], sizeof(payloadRx), payloadRx[2], CTS);
        // radio.startListening();
        report = aguardaMsg(MSG);
        if (report) {
          jogada(payloadRx[5]);
          report = sendPacket(&payloadRx[0], sizeof(payloadRx), payloadRx[2], ACK);
        }
      }
    }
  }
  radio.flush_rx();
  delay(10);
}
