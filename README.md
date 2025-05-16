# Auto Greenhouse - Estufa Automatizada com Arduino MEGA e ESP32

Um projeto de estufa automatizada com controle ambiental e notificações via Wi-Fi, utilizando Arduino MEGA para gerenciamento local e ESP32 para conectividade e alertas via WhatsApp.

## Sobre o Projeto

A Auto Greenhouse é um sistema DIY (Faça Você Mesmo) projetado para automatizar o controle de condições ambientais cruciais em uma estufa, como umidade do solo, luminosidade e ventilação. O Arduino MEGA atua como o cérebro principal, lendo dados de sensores e acionando componentes como bombas de irrigação, ventiladores e um servo para o teto. O ESP32 adiciona capacidade de conexão Wi-Fi, permitindo o envio de notificações (por exemplo, através do CallMeBot para o WhatsApp) sobre o status ou eventos da estufa.

## Funcionalidades

* **Controle de Umidade do Solo:** Ativação automática de bombas de irrigação ou drenagem com base nos níveis de umidade.
* **Controle de Luminosidade:** Acionamento de LEDs (simulando luz de crescimento) com base na luminosidade ambiente.
* **Controle de Ventilação:**
    * Ventilador com acionamento manual via botão.
    * Abertura/fechamento do teto da estufa via servo motor, controlado por botão.
* **Monitoramento Local:** Display LCD 16x2 para exibir dados de sensores (umidade, luminosidade) e status dos atuadores (ventilador).
* **Notificações Remotas (via ESP32):**
    * Conexão Wi-Fi.
    * Envio de mensagens de status ou alertas para o WhatsApp utilizando a API CallMeBot (acionado por evento, por exemplo, quando o teto é aberto/fechado pelo Arduino).
* **Interface Serial:** Saída de dados de debug e status via monitor serial.

## Componentes Necessários

*(Esta seção pode usar a lista já fornecida no seu README original, bem formatada)*

### Arduino MEGA (2560)
* 1x Arduino MEGA 2560
* 1x Fonte 12V AC/DC (para ventilador e alimentação geral, se necessário)
* Resistores: 2x 220 ohms (LEDs), 1x 1k ohm, 1x 10k ohm (pull-ups/pull-downs)
* 1x Display LCD 16x2
* 2x Botões de pressão (para servo e ventilador)
* 1x Servo Motor (ex: SG90, MG996R)
* 1x Sensor de Umidade do Solo (analógico)
* 1x Sensor de Luminosidade (LDR)
* 1x Sensor de Temperatura LM35 (opcional, não totalmente implementado no loop principal do código atual)
* 1x Micro-ventilador 12V (conectar ao Vin do Arduino ou fonte externa com relé)
* 1x LED (para simular iluminação de crescimento)
* 1x Módulo Relé de 2 canais 5V (para bombas)
* 2x Pequenas bombas d'água 5V/12V (para irrigação e drenagem)
* Jumpers e protoboard

### ESP32
* 1x Placa de desenvolvimento ESP32
* (O ESP32 pode ser alimentado via USB ou compartilhar alimentação se planejado corretamente)

## Configuração do Ambiente de Desenvolvimento (Arduino IDE)

1.  **Instale o Arduino IDE:** [arduino.cc/en/software](https://www.arduino.cc/en/software)
2.  **Instale os Drivers (se necessário):**
    * Para ESP32 com chip CP210x: [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
3.  **Adicione o Suporte para Placas ESP32:**
    * No Arduino IDE, vá em `Arquivo > Preferências`.
    * No campo "URLs Adicionais para Gerenciadores de Placas", adicione:
        ```
        [https://dl.espressif.com/dl/package_esp32_index.json](https://dl.espressif.com/dl/package_esp32_index.json)
        ```
    * Vá em `Ferramentas > Placa > Gerenciador de Placas...`.
    * Procure por "esp32" e instale o pacote da Espressif Systems.
    * Selecione a placa ESP32 correta em `Ferramentas > Placa` (ex: "ESP32 Dev Module").
4.  **Instale as Bibliotecas Necessárias (via Gerenciador de Bibliotecas):**
    * `LiquidCrystal` (geralmente já vem com o IDE)
    * `Servo` (geralmente já vem com o IDE)
    * Para ESP32: `WiFi`, `HTTPClient` (geralmente já vêm com o core ESP32)

## Montagem e Código

* **Diagrama de Conexões:** (Recomendado adicionar um diagrama Fritzing ou similar aqui)
    * Conecte os sensores, atuadores, LCD e botões ao Arduino MEGA conforme os pinos definidos no código (`.ino` do Arduino).
    * Conecte um pino de saída digital do Arduino MEGA (ex: pino 13 no código) a um pino de entrada digital do ESP32 para sinalizar eventos (ex: abertura do teto).
* **Código Arduino MEGA:**
    * Faça o upload do sketch fornecido (`seu_sketch_arduino.ino`) para o Arduino MEGA.
    * Ajuste os limiares dos sensores (`humRead`, `brightRead`) conforme necessário para sua calibração.
* **Código ESP32:**
    * Faça o upload do sketch fornecido (`seu_sketch_esp32.ino`) para o ESP32.
    * **Importante:** No código do ESP32, configure:
        * `ssid`: Nome da sua rede Wi-Fi.
        * `password`: Senha da sua rede Wi-Fi.
        * `token`: Seu API token do CallMeBot.
        * `phoneNumber`: Seu número de telefone para WhatsApp (com código do país, ex: 55XXXXXXXXXXX).
        * `message`: A mensagem que deseja enviar.
    * Modifique o `loop()` do ESP32 para ler o sinal do Arduino e chamar `sendMessage()` quando o evento ocorrer.

## Como Usar

1.  Após a montagem e upload dos códigos, energize o sistema.
2.  O Arduino MEGA começará a monitorar os sensores e controlar os atuadores.
3.  O display LCD mostrará leituras de umidade, luminosidade e status do ventilador.
4.  Use os botões para controlar o teto (servo) e o ventilador manualmente.
5.  Quando um evento configurado ocorrer (ex: abertura do teto, acionando o sinal para o ESP32), o ESP32 se conectará ao Wi-Fi e enviará uma notificação para o WhatsApp via CallMeBot.

## Demonstração / Vídeo (Opcional)

[Link para um vídeo do projeto em funcionamento, se houver]

## Licença

Este projeto é licenciado sob a **GNU Affero General Public License v3.0**.

---
