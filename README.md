# Low Level Programming - LAB 1, 2 & 3

## Вариант

- Реляционные таблицы
- Чтение/Запись
- SQL
- Apache Thrift

## Отчёты

В директории report  
[Lab 1](report/report-lab1.pdf)  
[Lab 2](report/report-lab2.pdf)  
[Lab 3](report/report-lab3.pdf)

## Сборка и запуск проекта

### Linux

Под линукс составлен скрипт `build-and-test.sh`, который соберёт проект и запустит тесты.

### Windows

Сборка и запуск проекта проверялись в следующих конфигурациях:

- MinGW toolset с Clang
- MSVC toolset с Clang-cl

Для запуска составлен скрипт `build-and-test.bat`, который соберёт проект и запустит тесты.  
ВНИМАНИЕ: ЭТОТ СКРИПТ ТРЕБУЕТ РУЧНОГО ВЫСТАВЛЕНИЯ ПУТЕЙ К БИНАРНИКАМ `CMAKE`, `MAKE`, `CC` и `CTEST`