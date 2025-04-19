#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include "myutils.h"

int my_sscanf(const char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int count = 0;
    const char *p = str;
    const char *f = format;

    while (*f) {
        // Пропускаем пробельные символы в формате
        if (isspace((unsigned char)*f)) {
            while (*f && isspace((unsigned char)*f))
                f++;
            while (*p && isspace((unsigned char)*p))
                p++;

        }
        else if (*f == '%') {
            f++; // пропускаем '%'
            if (*f == 'd') {  // поддерживаем только %d
                // Пропускаем пробелы во входной строке
                while (*p && isspace((unsigned char)*p)) p++;

                int sign = 1;
                // Обработка знака
                if (*p == '-') {
                    sign = -1;
                    p++;
                }
                else if (*p == '+') {
                    p++;
                }
                // Проверка наличия цифры
                if (!isdigit((unsigned char)*p)) {
                    // Если цифр нет, то завершить разбор
                    break;
                }
                int value = 0;
                while (*p && isdigit((unsigned char)*p)) {
                    value = value * 10 + (*p - '0');
                    p++;
                }
                int *dest = va_arg(args, int *);
                *dest = sign * value;
                count++;
                f++; // переходим к следующему символу в формате
            }
            else {
                // Если спецификатор не %d, выходим (или можно вернуть ошибку)
                break;
            }
        }
        else {
            // Если символы формата не совпадают с входными, требуем буквальное совпадение
            if (*f == *p) {
                f++;
                p++;
            }
            else {
                break;
            }
        }
    }
    va_end(args);
    return count;
}
