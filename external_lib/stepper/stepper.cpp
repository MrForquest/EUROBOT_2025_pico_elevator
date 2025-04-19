#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "stepper.h"
#include <stdlib.h>



/*https://www.webwork.co.uk/2023/06/raspberry-pi-pico-as-switching_21.html */

void stepper(int direction, int angle) {
    if (direction == 1) {
        gpio_put(STEPPER_DIR_PIN_1, 1);
    }
    else gpio_put(STEPPER_DIR_PIN_1, 0);

    pwm_set_gpio_level(STEPPER_PWM_PIN_1, 500);
    sleep_ms((angle / (1.8 * STEPPER_SPEED)) * 1000);
    //Определяем необходимую скважность шим, от которой зависит скорость мотора
    pwm_set_gpio_level(STEPPER_PWM_PIN_1, 0);
}


//Функция инициализации ШИМ-выводов для управления скоростью и пинов для управления направлением вращения
void stepper_pwm_init() {
    //https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=1
    //Инициализируем пины для управления направлением вращения
    gpio_init(STEPPER_DIR_PIN_1);

    gpio_set_dir(STEPPER_DIR_PIN_1, GPIO_OUT);

    //Выделяем пины PWM_PIN_i под генерацию ШИМА 
    gpio_set_function(STEPPER_PWM_PIN_1, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num1 = pwm_gpio_to_slice_num(STEPPER_PWM_PIN_1);

    //Устанавливаем предделитель от тактовой частоты процессора (125[Мгц])
    pwm_set_clkdiv(slice_num1, STEPPER_PWM_DIV);


    //Устанавливаем частоту ШИМА равной 125[Мгц]/PWM_DIV/PWM_WRAP  
    pwm_set_wrap(slice_num1, STEPPER_PWM_WRAP);



    //Задаем начальное заполнение шим 50%
    pwm_set_gpio_level(STEPPER_PWM_PIN_1, 0);


    //Включаем генерацию ШИМ
    pwm_set_enabled(slice_num1, true);

}

void stepper_direction_init() {
    gpio_init(STEPPER_DIR_PIN_1);
    gpio_set_dir(STEPPER_DIR_PIN_1, GPIO_OUT);
}

void stepper_go_home() {
    while (!gpio_get(LOWER_LIMIT_SWITCH_PIN)) {
        stepper(0, 10);
    }
}

void stepper_init() {
    stepper_direction_init();
    stepper_pwm_init();
    gpio_init(LOWER_LIMIT_SWITCH_PIN);
    gpio_set_dir(LOWER_LIMIT_SWITCH_PIN, GPIO_IN);
}

