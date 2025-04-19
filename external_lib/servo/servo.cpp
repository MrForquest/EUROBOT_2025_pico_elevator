#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "servo.h"
#include <stdlib.h>



/*https://www.webwork.co.uk/2023/06/raspberry-pi-pico-as-switching_21.html */

void servo(int number, int angle ){
    if(number == 1){

        pwm_set_gpio_level(PWM_PIN_1, (250+angle*5.55) );
        
    }
    
    else if(number == 2){
        pwm_set_gpio_level(PWM_PIN_2, (250+angle*5.55) );
        
    }

    else if(number == 3){
        pwm_set_gpio_level(PWM_PIN_3, (250+angle*5.55) );
        
    }

    else if(number == 4){
        pwm_set_gpio_level(PWM_PIN_4, (250+angle*5.55) );
        
    }
}


//Функция инициализации ШИМ-выводов для управления сервоприводами
void servo_init(){
    //https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=1
    
    //Выделяем пины PWM_PIN_i под генерацию ШИМА 
    gpio_set_function(PWM_PIN_1, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN_2, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN_3, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN_4, GPIO_FUNC_PWM);
  
    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num1 = pwm_gpio_to_slice_num(PWM_PIN_1);
    uint slice_num2 = pwm_gpio_to_slice_num(PWM_PIN_2);
    uint slice_num3 = pwm_gpio_to_slice_num(PWM_PIN_3);
    uint slice_num4 = pwm_gpio_to_slice_num(PWM_PIN_4);

    //Устанавливаем предделитель от тактовой частоты процессора (125[Мгц])
    pwm_set_clkdiv(slice_num1, PWM_DIV);
    pwm_set_clkdiv(slice_num2, PWM_DIV);
    pwm_set_clkdiv(slice_num3, PWM_DIV);
    pwm_set_clkdiv(slice_num4, PWM_DIV);

    //Устанавливаем частоту ШИМА равной 125[Мгц]/PWM_DIV/PWM_WRAP  
    pwm_set_wrap(slice_num1, PWM_WRAP);
    pwm_set_wrap(slice_num2, PWM_WRAP);
    pwm_set_wrap(slice_num3, PWM_WRAP);
    pwm_set_wrap(slice_num4, PWM_WRAP);

    //Задаем начальное заполнение шим 50%
    pwm_set_gpio_level(PWM_PIN_1, 500);
    pwm_set_gpio_level(PWM_PIN_2, 500);
    pwm_set_gpio_level(PWM_PIN_3, 500);
    pwm_set_gpio_level(PWM_PIN_4, 500);

    //Включаем генерацию ШИМ
    pwm_set_enabled(slice_num1, true);
    pwm_set_enabled(slice_num2, true);
    pwm_set_enabled(slice_num3, true);
    pwm_set_enabled(slice_num4, true);
}



