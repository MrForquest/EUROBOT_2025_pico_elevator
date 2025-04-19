#include "HardwareSerial.h"

HardwareSerial::HardwareSerial(uart_inst_t *uartPort, uint baudrate, int tx_pin, int rx_pin)
    : uartPort(uartPort), baudrate(baudrate) {
    uart_init(uartPort, baudrate);
    uart_set_format(uartPort, 8, 1, UART_PARITY_NONE);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
}

int HardwareSerial::available() {
    // Функция uart_is_readable возвращает ненулевое значение, если данные доступны
    return uart_is_readable(uartPort) ? 1 : 0;
}

int HardwareSerial::read() {
    if (uart_is_readable(uartPort)) {
        return uart_getc(uartPort);
    }
    else {
        return -1; // Нет данных для чтения
    }
}

int HardwareSerial::write(const uint8_t *data, size_t len) {
    // Отправка пакета данных заданной длины с использованием блокирующей функции
    uart_write_blocking(uartPort, data, len);
    return 0;
}