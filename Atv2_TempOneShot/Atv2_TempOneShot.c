#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define LED_BLUE 11
#define LED_RED 12
#define LED_GREEN 13

#define BUTTON_A 5

#define DEBOUNCE_DELAY 200 // Atraso de 2000ms para debouncing

typedef struct LED
{
  uint8_t led_id;
  bool state;
} LED;
LED led_active[3] = {{LED_BLUE, 0}, {LED_RED, 0}, {LED_GREEN, 0}}; // Indica se o LED está atualmente acesso.
absolute_time_t turn_off_time;                                     // Variável para armazenar o tempo em que o LED deve ser desligado.
static volatile uint32_t last_interrupt_time = 0;

/**
 * Função de Callback chamada pelo alarme
 */
int64_t turnOff_callback(alarm_id_t id, void *user_data)
{
  if (user_data)
  {
    LED *led = (LED *)user_data; // Cast para acessar os dados da struct LED

    if (gpio_get(led->led_id) == 1) // Verifica se está ligado
    {
      gpio_put(led->led_id, 0); // Desliga o LED
      led->state = false;       // Atualiza o estado na struct
      printf("TIMER_CALLBACK: LED no pino %d desligado.\n", led->led_id);
    }
  }
  return 0; // Retorna 0 para não repetir o alarme
}

/**
 * Função que realiza o trabalho de ativação do timer
 */
void buttonA_handler()
{
  if (!led_active[0].state && !led_active[1].state && !led_active[2].state)
  {
    // BLUE LED: Chama callback tipo alarme com tempo de 3 segundos.
    gpio_put(LED_BLUE, true);
    led_active[0].state = 1;
    alarm_id_t blue_alarm = add_alarm_in_ms(3000, turnOff_callback, &led_active[0], false);
    if (!blue_alarm)
    {
      printf("BUTTON_CALLBACK: Erro em adicionar o timer, BLUE_LED\n");
    }
    else
    {
      printf("BUTTON_CALLBACK: Timer adicionado, BLUE_LED\n");
    }

    // RED LED: Chama callback tipo alarme com tempo de 6 segundos.
    gpio_put(LED_RED, true);
    led_active[1].state = 1;
    alarm_id_t red_alarm = add_alarm_in_ms(6000, turnOff_callback, &led_active[1], false);
    if (!red_alarm)
    {
      printf("BUTTON_CALLBACK: Erro em adicionar o timer, RED_LED\n");
    }
    else
    {
      printf("BUTTON_CALLBACK: Timer adicionado, RED_LED\n");
    }

    // GREEN LED: Chama callback tipo alarme com tempo de 9 segundos.
    gpio_put(LED_GREEN, true);
    led_active[2].state = 1;
    alarm_id_t green_alarm = add_alarm_in_ms(9000, turnOff_callback, &led_active[2], false);
    if (!green_alarm)
    {
      printf("BUTTON_CALLBACK: Erro em adicionar o timer, GREEN_LED\n");
    }
    else
    {
      printf("BUTTON_CALLBACK: Timer adicionado, GREEN_LED\n");
    }
  }
}

/**
 * Função que organiza os callbacks do sistema
 */
void button_callback(uint gpio, uint32_t events)
{
  // static uint32_t last_interrupt_time = 0;
  uint32_t current_time = to_ms_since_boot(get_absolute_time());

  // Verifica se o tempo de debouncing passou
  if (current_time - last_interrupt_time > DEBOUNCE_DELAY)
  {
    if (gpio == BUTTON_A)
      buttonA_handler();

    // Atualiza o tempo da última interrupção
    last_interrupt_time = current_time;
  }
}

/**
 * Estrutura de repetição principal
 */
int main()
{
  stdio_init_all();

  // Inicia os LEDS
  gpio_init(LED_BLUE);
  gpio_set_dir(LED_BLUE, GPIO_OUT);
  gpio_put(LED_BLUE, false);
  gpio_init(LED_RED);
  gpio_set_dir(LED_RED, GPIO_OUT);
  gpio_put(LED_RED, false);
  gpio_init(LED_GREEN);
  gpio_set_dir(LED_GREEN, GPIO_OUT);
  gpio_put(LED_GREEN, false);

  // Inicia o botão
  gpio_init(BUTTON_A);
  gpio_set_dir(BUTTON_A, GPIO_IN);
  gpio_pull_up(BUTTON_A);

  // Configuração da interrupção com callback
  gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);

  // Programa Inciado!
  printf("System: Programa Inciado!\n");

  //  Program looping
  while (true)
  {
    // Sleep para diminuir uso da cpu
    sleep_ms(10);
  }
}
