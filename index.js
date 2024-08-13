const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

const PORTA_SERIAL = 'COM12'; // Porta serial do Arduino
const BAUDRATE = 115200; // Taxa de transmissão

// Inicializa a conexão com o Arduino
const arduino = new SerialPort({ path: PORTA_SERIAL, baudRate: BAUDRATE });
const parser = arduino.pipe(new ReadlineParser({ delimiter: '\n' }));

let posicoes = Array(9).fill(" ");
let pontos_jogador1 = 0;
let pontos_jogador2 = 0;
let pontos_empate = 0;
let jogada_vez = 4;

parser.on('data', (data) => {
  const dados = data.toString().trim();
  if (dados !== "Meio Ocupado" && dados.split(' ').length === 2) {
    const [jogada, jogador] = dados.split(' ').map(Number);
    jogada_vez = makeMove(jogada, jogador, jogada_vez);
    io.emit('atualizar-tabuleiro', { posicoes, pontos_jogador1, pontos_jogador2, pontos_empate, jogada_vez });
  }
});

function verificarVitoria() {
  const combinacoes = [
    [0, 1, 2], [3, 4, 5], [6, 7, 8], // Linhas
    [0, 3, 6], [1, 4, 7], [2, 5, 8], // Colunas
    [0, 4, 8], [2, 4, 6]             // Diagonais
  ];
  for (const linha of combinacoes) {
    if (posicoes[linha[0]] === posicoes[linha[1]] && posicoes[linha[1]] === posicoes[linha[2]] && posicoes[linha[0]] !== " ") {
      return posicoes[linha[0]];
    }
  }
  return posicoes.includes(" ") ? null : "Empate";
}

function makeMove(jogada, jogador, jogada_vez) {
  if (posicoes[jogada] !== " ") {
    io.emit('jogada-invalida', "Esta posição já está ocupada. Tente novamente.");
    return jogada_vez;
  }

  if (jogador !== jogada_vez) {
    io.emit('jogada-invalida', `Não é a vez do jogador ${jogador}.`);
    return jogada_vez;
  }

  posicoes[jogada] = jogador === 4 ? "X" : "O";
  const vencedor = verificarVitoria();
  if (vencedor) {
    if (vencedor === "Empate") {
      pontos_empate++;
      io.emit('resultado', "O jogo terminou em empate!");
    } else if (vencedor === "X") {
      pontos_jogador1++;
      io.emit('resultado', "O jogador 1 venceu!");
    } else {
      pontos_jogador2++;
      io.emit('resultado', "O jogador 2 venceu!");
    }
    resetarJogo();
    return 4;
  }

  return jogador === 4 ? 9 : 4;
}

function resetarJogo() {
  posicoes.fill(" ");
}

app.use(express.static('public'));

server.listen(3000, () => {
  console.log('Servidor rodando em http://localhost:3000');
});
