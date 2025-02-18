#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "I2C/display.h"

#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11

#define BUTTON_A 5
#define BUTTON_B 6
#define BUTTON_JOY 22

#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define ADC_CHANNEL_X 1
#define ADC_CHANNEL_Y 0

#define DEBOUNCE_DELAY 500
#define PWM_DIVISER 29.0
#define WRAP_PERIOD 2200
#define JOYSTICK_CENTER_X 2186
#define JOYSTICK_CENTER_Y 1886

static volatile uint32_t last_interrupt_time = 0;
bool isButtonA = false, isButtonB = false, isButtonJoy = false;
bool leds_active = true;
uint led_red_slice, led_blue_slice;
uint16_t joystick_value_x, joystick_value_y;

uint init_gpio_PWM(uint gpio, float diviser, uint16_t period) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_set_clkdiv(slice, diviser);
    pwm_set_wrap(slice, period);
    pwm_set_gpio_level(gpio, 0);
    pwm_set_enabled(slice, true);
    return slice;
}

void joystick_read_axis(uint16_t *x, uint16_t *y) {
    adc_select_input(ADC_CHANNEL_X);
    sleep_us(2);
    *x = adc_read();
    adc_select_input(ADC_CHANNEL_Y);
    sleep_us(2);
    *y = adc_read();
    *x = (*x >= 2180 && *x <= 2190) ? JOYSTICK_CENTER_X : *x;
    *y = (*y >= 1880 && *y <= 1890) ? JOYSTICK_CENTER_Y : *y;
}

void moveButton() {
    uint8_t x = (joystick_value_x * 127) / 4096;
    uint8_t y = 63 - ((joystick_value_y * 63) / 4096);
    x = (joystick_value_x == JOYSTICK_CENTER_X) ? 63 : x;
    y = (joystick_value_y == JOYSTICK_CENTER_Y) ? 31 : y;
    x = x < 3 ? 3 : (x > 122 ? 122 : x);
    y = y < 3 ? 3 : (y > 58 ? 58 : y);
    resetDisplay();
    setDisplay("0", x, y);
}

void updateLeds() {
    pwm_set_gpio_level(LED_RED, abs(joystick_value_x - JOYSTICK_CENTER_X));
    pwm_set_gpio_level(LED_BLUE, abs(joystick_value_y - JOYSTICK_CENTER_Y));
}

void handleButtonA() {
    leds_active = !leds_active;
    pwm_set_enabled(led_red_slice, leds_active);
    pwm_set_enabled(led_blue_slice, leds_active);
    isButtonA = false;
}

void handleButtonJOY() {
    gpio_put(LED_GREEN, !gpio_get(LED_GREEN));
    change_ret();
    isButtonJoy = false;
}

void gpio_callback(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_interrupt_time > DEBOUNCE_DELAY) {
        if (gpio == BUTTON_A) isButtonA = true;
        if (gpio == BUTTON_B) isButtonB = true;
        if (gpio == BUTTON_JOY) isButtonJoy = true;
        last_interrupt_time = current_time;
    }
}

void setup() {
    led_red_slice = init_gpio_PWM(LED_RED, PWM_DIVISER, WRAP_PERIOD);
    led_blue_slice = init_gpio_PWM(LED_BLUE, PWM_DIVISER, WRAP_PERIOD);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, false);

    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BUTTON_JOY);
    gpio_set_dir(BUTTON_JOY, GPIO_IN);
    gpio_pull_up(BUTTON_JOY);
    gpio_set_irq_enabled(BUTTON_JOY, GPIO_IRQ_EDGE_FALL, true);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    display_init();
}

int main() {
    stdio_init_all();
    setup();
    printf("Programa Iniciado: Joystick PWM\n");

    while (true) {
        if (isButtonA) handleButtonA();
        else if (isButtonJoy) handleButtonJOY();
        else {
            joystick_read_axis(&joystick_value_x, &joystick_value_y);
            updateLeds();
            moveButton();
        }
        sleep_ms(50);
    }
}
