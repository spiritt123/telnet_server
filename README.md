# Сервер для тестового задания

## Сборка и запуск

Сборка :
```
$ make -j4
```
Запуск сервера по умолчанию 

ip =  127.0.0.1 

port = 1234
```
$ ./server
```

Запуск сервера на определённом ip адресе и порту.

```
$ ./server 192.168.100.10 1333
```

Подключение к серверу

```
$ telnet 127.0.0.1 1234
```

## Описание классов

- Server -- Отвечает за поиск новых подключений 
- ClientHandler -- Отвечает за получение и отправку сообщений(+ логика задачи)
- BD -- Общее хранилице пользовательских настроек

## Информация для тестирования

Значение для начального сдвига и шага интерации заданы 64 битным числом.

Максимальное значение -- 18446744073709551615.

1. Проверить на корректный вывод последовательности

2. Проверить пограничные условия
	- Если последовательность станет больше максимального значения, последовательность должна быть выставлена в начальное положение, заданное пользователем.
	- Если начальное значение и шаг в сумме дают значение больше максимального, последовательность не меняется.

## Возможные улучшения при изменении задачи.

- В случае увеличения количества последовательностей стоит модифицировать структуру Record - вместо 3 структур хранить вектор.

- Если вычисление последовательности станет дорогой операцией, то стоит распараллелить вычисление каждой последовательности в рамках одного ответа клиенту.


