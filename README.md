<h3 align="center">
        <samp>&gt; Olá, bem-vindos ao
                <b><a target="_blank" href="https://github.com/devagas-utfpr">Projeto Jogo da Velha IoT</a></b>
        </samp>
</h3>

<p align="center"> 
  <samp>
    「 Disciplina de Interfaces Computacionais 」
    <br>
    Universidade Tecnológica Federal do Paraná, Campo Mourão
  </samp>
</p>

---

### Sobre

- Este projeto consiste em um **Jogo da Velha** implementado com base no conceito de **Internet das Coisas (IoT)**, desenvolvido para a disciplina de Interfaces Computacionais na UTFPR, campus de Campo Mourão.
- O objetivo principal é demonstrar a interação entre dispositivos físicos e uma interface web, onde os jogadores podem visualizar o progresso do jogo em tempo real.

---

### Visão geral

- O sistema é composto por dois principais componentes:
  1. **Arduino**: Controla a lógica do jogo e a interação com os sensores e atuadores. Os arquivos de código relacionados estão na pasta `arduino`.
  2. **Servidor Node.js com Express**: Gerencia a comunicação entre o Arduino e a interface do usuário, exibindo o estado atual do jogo em um navegador web.

---

### Como executar o projeto

1. **Preparar o Arduino**:
   - Carregue o código contido na pasta `arduino` para o Arduino utilizando a IDE apropriada.
   - Conecte os sensores e atuadores conforme o esquema do projeto.
   - Compile todos os arquivos de **jogador** e inicie-os.
   - E por fim apenas compile o arquivo de **jogo**.

2. **Configurar o servidor**:
   - Instale as dependências necessárias:

     ```bash
     npm install
     ```

   - Inicie o servidor:
     ```bash
     npm start
     ```

   - O servidor estará rodando localmente, acessível em `http://localhost:3000`.

3. **Acessar a interface web**:
   - Abra um navegador e acesse `http://localhost:3000` para visualizar o estado do jogo da velha em tempo real.

---

### Tecnologias utilizadas

<img src="https://skillicons.dev/icons?i=arduino,nodejs,express" />

---

### Equipe

- [Eduardo Riki](https://github.com/eduardo-riki) e [Yagho Petini](https://github.com/zYagho)