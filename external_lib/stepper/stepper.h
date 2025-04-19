/******************************************************************Обозначения*********************************************************************************/

//Определяем пины, к которым будем подключать шим-входы контроллера мотора
#define STEPPER_PWM_PIN_1 0     //Подключаем к GP2 - 4 ножка микроконтроллера

#define STEPPER_DIR_PIN_1 1


 

//Определяем предделитель тактовой частоты микроконтроллера (125Мгц) - один для всех шим-каналов
//#define PWM_DIV 6250
#define STEPPER_PWM_DIV 30
//Определяем число тиковs, через которое таймер будет сброшен. От этого числа будет зависеть точность установки скважности импульсов(чем больше, тем выше точность)
#define STEPPER_PWM_WRAP 1000
//Заметим, что при таких настройках частота шима 125 000 000/6250 = 20 Кгц, что необходимо для работы драйвера мотора
#define STEPPER_SPEED 125000000/(STEPPER_PWM_DIV*STEPPER_PWM_WRAP)

#define LOWER_LIMIT_SWITCH_PIN 20

/***************************************************************Библиотечные функции***************************************************************************/

void stepper_pwm_init();
void stepper_direction_init();
void stepper_init();
/*
void motor1_controller(float speed );
void motor2_controller(float speed );
void motor3_controller(float speed );
void motor4_controller(float speed );
*/
void stepper(int direction, int angle );
void stepper_go_home();

