/*
 * Por: [Seu Nome]
 * Solução: Diagnóstico de Computadores via OLED com Menu, Joystick Analógico,
 *           Piscar na Opção Selecionada, Atualização a cada 1 minuto e LED RGB.
 *
 * Este programa utiliza o display OLED SSD1306 (128x64 pixels) e o microcontrolador
 * RP2040 (Raspberry Pi Pico W) para exibir informações de diagnóstico simuladas.
 * No MENU, a navegação é feita via eixo Y do joystick (GP27) e a seleção via botão
 * do joystick (GP22). O botão A (pino 5) retorna ao MENU na tela de diagnóstico.
 * Na tela DIAGNÓSTICO, o valor simulado muda a cada 1 minuto e, conforme o valor,
 * a cor de um LED RGB (Verde: pino 11, Azul: pino 12, Vermelho: pino 13) é atualizada.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "pico/stdlib.h"
 #include "hardware/i2c.h"
 #include "hardware/adc.h"
 #include "hardware/pwm.h"
 #include "lib/ssd1306.h"
 #include "lib/font.h"
 
 // Definições para I2C e Display OLED
 #define I2C_PORT i2c1
 #define I2C_SDA 14
 #define I2C_SCL 15
 #define OLED_ADDR 0x3C
 
 // Configuração do joystick (analógico para X e Y, digital para o botão)
 #define JOYSTICK_X_PIN 26  // ADC (não utilizado neste exemplo)
 #define JOYSTICK_Y_PIN 27  // ADC para detectar movimentos para cima/baixo
 #define JOYSTICK_PB    22  // Botão de seleção do joystick (ativo em nível baixo)
 #define BUTTON_A_PIN    5  // Botão para retornar ao menu (ativo em nível baixo)
 
 // Configuração dos pinos do LED RGB conforme solicitado:
 #define LED_G_PIN 11   // Verde
 #define LED_B_PIN 12   // Azul
 #define LED_R_PIN 13   // Vermelho
 
 // Definições para a leitura do joystick
 #define ADC_CENTER 2048       // Valor médio aproximado para ADC 12-bit (0-4095)
 #define ADC_THRESHOLD 300     // Limiar para detectar movimento
 
 // Funções para simular os dados de diagnóstico
 int get_cpu_usage() {
     return rand() % 101; // 0 a 100%
 }
 
 int get_memory_usage() {
     return rand() % 160; // Exemplo: 0 a 16000 MB
 }
 
 int get_temperature() {
     return 20 + (rand() % 81); // 20 a 100 °C
 }
 
 typedef enum {
     MENU,
     DIAG
 } State;
 
 /* Configura o PWM para o pino especificado com wrap 4095 */
 void setup_pwm(uint pin) {
     gpio_set_function(pin, GPIO_FUNC_PWM);
     uint slice = pwm_gpio_to_slice_num(pin);
     pwm_set_wrap(slice, 4095);
     pwm_set_enabled(slice, true);
 }
 
 /* Atualiza o duty cycle do PWM no pino especificado */
 void set_pwm_duty(uint pin, uint16_t value) {
     pwm_set_gpio_level(pin, value);
 }
 
 /* Atualiza a cor do LED RGB conforme o valor simulado e a opção selecionada.
    Para cada diagnóstico (CPU, MEM ou TMP), os limiares definem a cor:
      - Valor baixo → verde (LED_G aceso)
      - Valor intermediário → amarelo (LED_R e LED_G acesos)
      - Valor alto → vermelho (LED_R aceso) */
 void update_led(int value, int option) {
     if (option == 0) { // CPU
         if (value < 50) {
             set_pwm_duty(LED_R_PIN, 0);
             set_pwm_duty(LED_G_PIN, 4095);
             set_pwm_duty(LED_B_PIN, 0);
         } else if (value < 80) {
             set_pwm_duty(LED_R_PIN, 4095);
             set_pwm_duty(LED_G_PIN, 4095);
             set_pwm_duty(LED_B_PIN, 0);
         } else {
             set_pwm_duty(LED_R_PIN, 4095);
             set_pwm_duty(LED_G_PIN, 0);
             set_pwm_duty(LED_B_PIN, 0);
         }
     } else if (option == 1) { // MEM
         if (value < 80) {
             set_pwm_duty(LED_R_PIN, 0);
             set_pwm_duty(LED_G_PIN, 4095);
             set_pwm_duty(LED_B_PIN, 0);
         } else if (value < 120) {
             set_pwm_duty(LED_R_PIN, 4095);
             set_pwm_duty(LED_G_PIN, 4095);
             set_pwm_duty(LED_B_PIN, 0);
         } else {
             set_pwm_duty(LED_R_PIN, 4095);
             set_pwm_duty(LED_G_PIN, 0);
             set_pwm_duty(LED_B_PIN, 0);
         }
     } else if (option == 2) { // TMP
         if (value < 40) {
             set_pwm_duty(LED_R_PIN, 0);
             set_pwm_duty(LED_G_PIN, 4095);
             set_pwm_duty(LED_B_PIN, 0);
         } else if (value < 70) {
             set_pwm_duty(LED_R_PIN, 4095);
             set_pwm_duty(LED_G_PIN, 4095);
             set_pwm_duty(LED_B_PIN, 0);
         } else {
             set_pwm_duty(LED_R_PIN, 4095);
             set_pwm_duty(LED_G_PIN, 0);
             set_pwm_duty(LED_B_PIN, 0);
         }
     }
 }
 
 int main() {
     stdio_init_all();
 
     // Inicializa o ADC e configura os pinos analógicos
     adc_init();
     adc_gpio_init(JOYSTICK_X_PIN);
     adc_gpio_init(JOYSTICK_Y_PIN);
 
     // Inicialização da comunicação I2C para o OLED
     i2c_init(I2C_PORT, 400 * 1000);
     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(I2C_SDA);
     gpio_pull_up(I2C_SCL);
 
     // Inicializa o display OLED
     ssd1306_t ssd;
     ssd1306_init(&ssd, 128, 64, false, OLED_ADDR, I2C_PORT);
     ssd1306_config(&ssd);
     ssd1306_fill(&ssd, false);
     ssd1306_send_data(&ssd);
 
     // Inicializa os pinos dos botões
     gpio_init(JOYSTICK_PB);
     gpio_set_dir(JOYSTICK_PB, GPIO_IN);
     gpio_pull_up(JOYSTICK_PB);
 
     gpio_init(BUTTON_A_PIN);
     gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
     gpio_pull_up(BUTTON_A_PIN);
 
     // Inicializa os pinos do LED RGB
     setup_pwm(LED_R_PIN);
     setup_pwm(LED_G_PIN);
     setup_pwm(LED_B_PIN);
 
     // Buffers para as strings dos valores de diagnóstico
     char str_cpu[10];
     char str_mem[10];
     char str_temp[10];
 
     // Cores fixas para desenho: borda branca e fundo preto
     const bool border_color = true;
     const bool bg_color = false;
 
     // Inicializa o gerador de números aleatórios para simulação
     srand(time_us_32());
 
     // Estado inicial: MENU, com opção selecionada (0: CPU, 1: MEM, 2: TMP)
     State currentState = MENU;
     int selectedOption = 0;
 
     // Variável para controle do piscar (blink)
     uint32_t blinkCounter = 0;
 
     // Variável para armazenar o valor simulado (atualizado na tela DIAG)
     int sim_value = 0;
     // Armazena o tempo (em ms) da última atualização do valor simulado
     uint32_t last_sim_update = to_ms_since_boot(get_absolute_time());
     // Flag para atualizar imediatamente ao entrar em DIAG
     bool first_update = true;
 
     while (true) {
         blinkCounter++;
         bool blink = (blinkCounter % 10) < 5;
 
         // Limpa o display com fundo fixo
         ssd1306_fill(&ssd, bg_color);
 
         // Desenha o quadro externo (retângulo maior) sem a borda inferior
         ssd1306_line(&ssd, 3, 3, 125, 3, border_color);   // Borda superior
         ssd1306_line(&ssd, 3, 3, 3, 63, border_color);      // Borda esquerda
         ssd1306_line(&ssd, 125, 3, 125, 63, border_color);   // Borda direita
 
         // Desenha a linha divisória horizontal
         ssd1306_line(&ssd, 3, 25, 123, 25, border_color);
 
         if (currentState == MENU) {
             // Exibe os títulos
             ssd1306_draw_string(&ssd, "DIAG PC", 8, 6);
             ssd1306_draw_string(&ssd, "STATUS SYS", 20, 16);
 
             // Exibe as opções do menu – a opção selecionada pisca
             if (selectedOption == 0) {
                 if (blink)
                     ssd1306_draw_string(&ssd, "CPU", 10, 28);
             } else {
                 ssd1306_draw_string(&ssd, "CPU", 10, 28);
             }
             if (selectedOption == 1) {
                 if (blink)
                     ssd1306_draw_string(&ssd, "MEM", 10, 42);
             } else {
                 ssd1306_draw_string(&ssd, "MEM", 10, 42);
             }
             if (selectedOption == 2) {
                 if (blink)
                     ssd1306_draw_string(&ssd, "TMP", 10, 56);
             } else {
                 ssd1306_draw_string(&ssd, "TMP", 10, 56);
             }
 
             // Leitura do eixo Y do joystick (GP27, canal 1)
             adc_select_input(1);
             uint16_t joy_y = adc_read();
 
             // Detecta movimento para cima ou para baixo com base no limiar
             if (joy_y < (ADC_CENTER - ADC_THRESHOLD)) {
                 selectedOption = (selectedOption - 1 + 3) % 3;
                 sleep_ms(500);  // debounce
             } else if (joy_y > (ADC_CENTER + ADC_THRESHOLD)) {
                 selectedOption = (selectedOption + 1) % 3;
                 sleep_ms(500);  // debounce
             }
 
             // Se o botão do joystick for pressionado, entra em DIAGNÓSTICO
             if (!gpio_get(JOYSTICK_PB)) {
                 currentState = DIAG;
                 last_sim_update = to_ms_since_boot(get_absolute_time());
                 first_update = true;
                 sleep_ms(300);  // debounce
             }
         } else { // MODO DIAGNÓSTICO
             // Exibe títulos e layout fixo
             ssd1306_draw_string(&ssd, "DIAG PC", 8, 6);
             ssd1306_draw_string(&ssd, "STATUS SYS", 20, 16);
             ssd1306_draw_string(&ssd, "CPU  MEM  TMP", 10, 28);
 
             // Desenha as linhas verticais para separar os diagnósticos
             ssd1306_line(&ssd, 44, 37, 44, 60, border_color);
             ssd1306_line(&ssd, 84, 37, 84, 60, border_color);
 
             // Atualiza o valor simulado a cada 1 minuto (60000 ms)
             uint32_t now = to_ms_since_boot(get_absolute_time());
             if (first_update || (now - last_sim_update >= 60000)) {
                 last_sim_update = now;
                 first_update = false;
                 if (selectedOption == 0) {
                     sim_value = get_cpu_usage();
                 } else if (selectedOption == 1) {
                     sim_value = get_memory_usage();
                 } else if (selectedOption == 2) {
                     sim_value = get_temperature();
                 }
                 update_led(sim_value, selectedOption);
             }
 
             // Exibe o valor simulado conforme a opção selecionada
             if (selectedOption == 0) {  // CPU
                 sprintf(str_cpu, "%d%%", sim_value);
                 ssd1306_draw_string(&ssd, str_cpu, 8, 52);
             } else if (selectedOption == 1) {  // MEM
                 sprintf(str_mem, "%dMB", sim_value);
                 ssd1306_draw_string(&ssd, str_mem, 49, 52);
             } else if (selectedOption == 2) {  // TMP
                 sprintf(str_temp, "%dC", sim_value);
                 ssd1306_draw_string(&ssd, str_temp, 90, 52);
             }
 
             // Se o botão A for pressionado, retorna ao MENU e apaga os LEDs
             if (!gpio_get(BUTTON_A_PIN)) {
                 // Apaga o LED RGB
                 set_pwm_duty(LED_R_PIN, 0);
                 set_pwm_duty(LED_G_PIN, 0);
                 set_pwm_duty(LED_B_PIN, 0);
                 currentState = MENU;
                 sleep_ms(300);  // debounce
             }
         }
 
         // Atualiza o display
         ssd1306_send_data(&ssd);
         sleep_ms(100);
     }
 
     return 0;
 }
 