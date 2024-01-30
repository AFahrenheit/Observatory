# //___________________________________________________________________________________________________________________
# //                                   ПОВОРОТ КУПОЛА
# //‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
# ПРАВО +169 1
# last_position = 190   # Текущая позиция
# Az = 359   # Азимут

# ПРАВО +160 1
# last_position = 0   # Текущая позиция
# Az = 160   # Азимут

# ПРАВО +140 1
# last_position = 10   # Текущая позиция
# Az = 150   # Азимут

# ПРАВО +126 1
# last_position = 14   # Текущая позиция
# Az = 140   # Азимут

# ВЛЕВО -170
# last_position = 0   # Текущая позиция
# Az = 190   # Азимут

# ПРАВО +109
# last_position = 290   # Текущая позиция
# Az = 39   # Азимут

# ЛЕВО -20 4
# last_position = 290   # Текущая позиция
# Az = 270   # Азимут

# ЛЕВО -175
# last_position = 290   # Текущая позиция
# Az = 115   # Азимут

# ПРАВО +175
# last_position = 290   # Текущая позиция
# Az = 105   # Азимут


# //___________________________________________________________________________________________________________________
# //                                   ПОВОРОТ КУПОЛА
# //‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

last_position = 0   # Текущая позиция
Az = 0   # Азимут
count = 0     #  для WHILE

while (Az < 359):                #  для WHILE

    count += 1
    if last_position >= 358:     #  для WHILE
        Az += 5                 #  для WHILE
        last_position = 0        #  для WHILE

    turn_L = last_position - Az   # вычисление шагов вЛево
    turn_R = Az - last_position   # вычисление шагов вПраво

    print(f'\nДвигаемся с {last_position} на {Az}. Разница {Az - last_position}')

    pos = min(turn_L, turn_R)              # выбор наименьшего пути право/лево

    if (turn_R == 0):
        print('Стоим.')
    elif (Az > last_position) and (abs(pos) < 180): # or (Az - last_position < -180):
        print('1 Еду ПРАво', abs(pos), 'углов')
    # elif (turn_R < -180):
    #     pos = 360 + (Az - last_position)
    #     print('1 Еду ПРАво', abs(pos), 'углов')
    elif abs(pos) >= 180:
        pos += 360
        if pos > 0:
            pos *= -1
        print('2 Еду ЛЕво', pos, 'углов') # сделать минус
    # elif (turn_R > 180):
    #     pos = turn_R
    #     print('3 Еду ПРАво', pos, 'углов')
    elif (abs(turn_R) > 180):
        if (turn_R < -180):
            pos = 360 + (turn_R)
            print('1 Еду ПРАво', abs(pos), 'углов')
        elif (turn_R > 180):
            pos = turn_R
            print('3 Еду ПРАво', pos, 'углов')

    else:
        print('4 Еду лево', pos, 'углов')



    last_position += 5         #  для WHILE
print(f'\nКоличество вариантов {count}')