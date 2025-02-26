# Sistema Embarcado para Diagnóstico de Computadores

## Descrição do Projeto

Este projeto implementa um sistema embarcado utilizando a placa **BitDogLab** para diagnóstico de computadores. O sistema coleta e exibe informações de uso de CPU, consumo de memória e temperatura em um **display OLED SSD1306**, permitindo que técnicos e usuários monitorem o estado dos computadores de maneira prática. O sistema também utiliza um **joystick analógico** para navegação no menu e um **LED RGB** para indicar visualmente o status dos diagnósticos.

## Componentes Utilizados

- **Placa BitDogLab** (baseada no RP2040)
- **Display OLED SSD1306** (com comunicação I2C)
- **Joystick analógico** (para navegação no menu)
- **LED RGB** (indicação visual de status)
- **Botões físicos** (para controle e seleção de opções)

## Funcionalidades

- Monitoramento de uso da CPU
- Monitoramento do consumo de memória
- Leitura e exibição da temperatura do sistema
- Interface gráfica simples via **display OLED**
- Navegação intuitiva utilizando **joystick analógico**
- Indicação visual por **LED RGB**
- Atualização dos dados a cada 1 minuto

## Instalação e Configuração

### Pré-requisitos

- **Placa BitDogLab** conectada ao computador
- **Ambiente de desenvolvimento configurado** (VS Code + extensões para RP2040)
- **Bibliotecas necessárias instaladas**

### Passos para Configuração

1. Clone este repositório:
   ```sh
   git clone https://github.com/camillerodriguees/embarcatech-projeto_final.git
   ```
2. Compile e carregue o código na BitDogLab usando a IDE compatível.

## Uso do Sistema

1. **Ligue a placa BitDogLab** e aguarde a inicialização.
2. **Use o joystick** para navegar entre as opções do menu.
3. **Pressione o botão** para selecionar uma métrica a ser visualizada.
4. **O LED RGB indicará** o status crítico da métrica selecionada.
5. **Os valores são atualizados automaticamente** a cada minuto.
6. **Para voltar ao menu principal, pressione o botão correspondente.**

