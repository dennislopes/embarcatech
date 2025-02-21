#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pico/binary_info.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_i2c.c"
#include "hardware/i2c.h"

#define MIC_CHANNEL 2                                 // Canal do microfone no ADC.
#define MIC_PIN (26 + MIC_CHANNEL)                    // Pino do microfone no ADC.
#define ADC_CLOCK_DIV 96.f                            // Parâmetros e macros do ADC.
#define SAMPLES 200                                   // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f / 5.f)                         // Intervalos de volume do microfone.
#define abs(x) ((x < 0) ? (-x) : (x))
#define BUTTON_PIN 5      // Pino do botão A

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
ssd1306_t oled;

// Canal e configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;

// Buffer de amostras do ADC.
uint16_t adc_buffer[SAMPLES];

void sample_mic();
float mic_power();
uint8_t get_intensity(float v);

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic()
{
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false);   // Desliga o ADC (se estiver ligado) para configurar o DMA.

  dma_channel_configure(dma_channel, &dma_cfg,
                        adc_buffer,      // Escreve no buffer.
                        &(adc_hw->fifo), // Lê do ADC.
                        SAMPLES,         // Faz "SAMPLES" amostras.
                        true             // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);

  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);
}

/**
 * Calcula a potência média das leituras do ADC. (Valor RMS)
 */
float mic_power()
{
  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];

  avg /= SAMPLES;
  return sqrt(avg);
}

/**
 * Calcula a intensidade do volume registrado no microfone, usando a tensão.
 */
uint8_t get_intensity(float v)
{
  uint count = 0;

  while ((v -= ADC_STEP / 20) > 0.f)
    ++count;

  return count;
}

/**
 *  Programa principal
 */
int main()
{
  stdio_init_all();

  // Inicialização do i2c
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Processo de inicialização completo do OLED SSD1306
  ssd1306_init();

  // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);

  // zera o display inteiro
  uint8_t ssd[ssd1306_buffer_length];
  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
      true,  // Habilitar FIFO
      true,  // Habilitar request de dados do DMA
      1,     // Threshold para ativar request DMA é 1 leitura do ADC
      false, // Não usar bit de erro
      false  // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);

  // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);
  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false);           // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true);           // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)
  channel_config_set_dreq(&dma_cfg, DREQ_ADC);                  // Usamos a requisição de dados do ADC

  // Configuração do GPIO do Botão A como entrada com pull-up interno
  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN);


  while (true)
  {

    if (gpio_get(BUTTON_PIN) == 0){
    float avg;
    float avg_sum = 0;
    float peak = 0;
    int count = 0;
    uint intensity = 0;

    char *text[] = {
      "Realizando     ",
      "Amostragem  ",
      "               ",
      "               ",
      "               ",
      "               "};

  int y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += 8;
  }
  render_on_display(ssd, &frame_area);

    for (int i = 0; i < 20; i++)
    {
      // Realiza uma amostragem do microfone.
      sample_mic();
      // Pega a potência média da amostragem do microfone.
      avg = mic_power();
      avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)
      avg_sum += avg;
      if (avg > peak)
      {
        peak = avg;
      }
      count++;
      sleep_ms(500);
    }

    float avg_final = avg_sum / count;

    for (int i = 0; i < 10; i++)
    {
      uint intensity = get_intensity(avg_final); // Calcula intensidade a ser mostrada na matriz de LEDs.
      uint peak_intensity = get_intensity(peak); // Calcula intensidade a ser mostrada na matriz de LEDs.
      char intensity_str[10];
      sprintf(intensity_str, "%d", intensity);
      char peak_str[10];
      sprintf(peak_str, "%d", peak_intensity); // Converte o valor de pico para string com 2 casas decimais

      char *text[] = {
          " Decibelimetro  ",
          "               ",
          "Valor Medio    ",
          intensity_str,
          "Valor de Pico     ",
          peak_str};

      int y = 0;
      for (uint i = 0; i < count_of(text); i++)
      {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
      }
      render_on_display(ssd, &frame_area);

      sleep_ms(500);
    }
  }
}
}