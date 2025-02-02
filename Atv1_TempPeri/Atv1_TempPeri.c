#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#define LED_RED 11
#define LED_YELLOW 12
#define LED_GREEN 13

char semaphor = 'Y'; // Variavel de estado do semaphoro

/**
 * Função de callback do timer
 */
bool repeating_timer_callback(struct repeating_timer *t)
{
    switch (semaphor)
    {
    case 'R': // Codigo de mudança vermelho
        gpio_put(LED_RED, 1);
        gpio_put(LED_YELLOW, 0);
        gpio_put(LED_GREEN, 0);
        printf("Semaphor: Sinal trocado para RED\n");
        semaphor = 'Y';
        break;
    case 'Y':
        gpio_put(LED_RED, 0);
        gpio_put(LED_YELLOW, 1);
        gpio_put(LED_GREEN, 0);
        semaphor = 'G';
        printf("Semaphor: Sinal trocado para YELLOW\n");
        break;
    case 'G':
        gpio_put(LED_RED, 0);
        gpio_put(LED_YELLOW, 0);
        gpio_put(LED_GREEN, 1);
        printf("Semaphor: Sinal trocado para GREEN\n");
        semaphor = 'R';
        break;

    default:
        break;
    }
}

/**
 * Estrutura de repetição principal
 */
int main()
{
    stdio_init_all();

    // Inicia os LEDS
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, true);
    gpio_init(LED_YELLOW);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_put(LED_YELLOW, false);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, false);

    // Define um intervalo de tempo em milisegundos de 1 segundo
    uint32_t interval = 1000;

    // Calcula o proximo tmepo absoluto da ação
    absolute_time_t next_wake_time = delayed_by_us(get_absolute_time(), interval * 1000);

    // Declaração da estrutura do temporizador de repetição
    struct repeating_timer timer;

    // Configura o temporizador para chamar o callback a cada 3 segundos
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);

    while (true)
    {
        // verifica se o tempo atual ultrapassaou o timestamp
        if (time_reached(next_wake_time))
        {
            printf("System: 1 segundo se passou em loop.\n");
            next_wake_time = delayed_by_us(next_wake_time, interval * 1000);
        }
        // Pausa para reduzir o uso da CPU
        sleep_ms(1);
    }
}
