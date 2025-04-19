#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H

#include "pico/stdlib.h"
#include "hardware/uart.h"

class HardwareSerial {
public:
    // Конструктор принимает указатель на UART-инстанс (например, uart0 или uart1)
    // а также номера пинов TX и RX.
    HardwareSerial(uart_inst_t *uartPort, uint baudrate, int tx_pin, int rx_pin);

    uint baudrate;
    
    // Возвращает 1, если данные доступны для чтения, иначе 0.
    int available();

    // Чтение одного байта. Если данных нет, возвращает -1.
    int read();

    int write(const uint8_t *data, size_t len);

private:
    uart_inst_t *uartPort;
    uint txPin;
    uint rxPin;
};

#endif // HARDWARESERIAL_H
