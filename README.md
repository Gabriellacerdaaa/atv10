# 🚀 Comunicação Serial com RP2040

📅 Trabalho do dia 10/02/2025 do projeto **EmbarcaTech**

## 🛠️ Componentes BigDogLab

- **Joystick**: Potenciômetros X e Y conectados aos pinos GPIO 26 e 27.
- **LED RGB**: Pinos conectados às GPIOs 11 (vermelho), 12 (azul) e 13 (verde).
- **Botões**:
  - Botão A conectado à GPIO 5.
  - Botão do joystick (JOY) conectado à GPIO 22.
- **Display SSD1306**: Conectado via I2C (GPIO 14 e GPIO 15).

## ⚡ Funcionalidades

1) **Controle de Intensidade dos LEDs RGB**:
   - **LED Azul**: Brilho ajustado pelo eixo Y do joystick. Quando centralizado (valor 2048), o LED permanece apagado. Movimentos para cima (valores menores) ou para baixo (valores maiores) aumentam a intensidade até o máximo (0 ou 4095).
   - **LED Vermelho**: Brilho ajustado pelo eixo X do joystick. Quando centralizado (valor 2048), o LED permanece apagado. Movimentos para a esquerda (valores menores) ou para a direita (valores maiores) aumentam a intensidade até o máximo (0 ou 4095).
   - **Controle via PWM**: Permite transições suaves de intensidade luminosa.

2) **Movimentação no Display SSD1306**:
   - Um quadrado de 8x8 pixels será exibido e se moverá proporcionalmente aos valores capturados pelo joystick, partindo da posição central.

3) **Botão do Joystick (JOY)**:
   - Alterna o estado do LED Verde a cada acionamento.
   - Modifica a borda do display para indicar a ação, alternando entre diferentes estilos a cada ativação.

4) **Botão A**:
   - Ativa ou desativa os LEDs PWM a cada acionamento.
