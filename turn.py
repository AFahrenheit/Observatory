last_position = 0  # int(input('Начальная позиция: '))
new_position = 0   # int(input('Новая позиция: '))

while True:              # для цикла WHILE

    turn = 0                                # шаги в градусах до намеченной цели
    turn_L = last_position + new_position   # вычисление шагов вЛево
    turn_R = last_position - new_position   # вычисление шагов вПраво
    if turn_R < 0:                          # если вПраво через точку 0
        turn_R += 361

    pos = min(turn_L, turn_R)               # выбор наименьшего пути право/лево

    if pos == turn_R and pos <= 180:        # движение вправо
        print('Еду ПРАВО')
        while pos != turn:
            # КОД ДЛЯ МОТОРА
            turn = pos - 1                  # отсчёт шагов для цели
            pos = turn
            if new_position == turn:        # если достиг цели
                break
    else:
        print('Еду лево')                   # движение влево
        while new_position - last_position != turn:
            # КОД ДЛЯ МОТОРА
            turn += 1                       # отсчёт шагов до цели
            if pos == turn:                 # если достиг цели
                break
    print('Начальная позиция:', last_position)
    print('Новая позиция:', new_position)
    print('Повернул на', turn, 'углов')
    print('Последняя позиция: ', last_position, '\n')

    new_position += 10              # для цикла WHILE
    if new_position >= 361:         # для цикла WHILE
        break                       # для цикла WHILE
