import math

""""             1                    2                    3                    4
Имя           Арктур                Мирах              Кебалраи              Капелла
pUT           (16:12)             (20:15)-1m         (20:15)-1m            (00:39)-2m         всемирное время (время по Гринвичу)
pGST          (20:06)+6            (20.38)+4          (15.44)+4            (17.07)+6          (ОШИБКА!)звездное время по Гринвичу
hourAngel     (5:51)+9m           (19:28)+8m         (21:59)+7m            (11:47)+11m        часовой угол
Азимут         (279) 281            (84) 85           (143) 145             (357) 360                
Высота        (16.5) 15.3        (40.5) 42.3          (33.3) 35            (11.52) 11         """


# ТЕСТОВЫЕ ЗНАЧЕНИЯ
star = 4
if star == 1:
    name = 'Арктур'
    Ra = 855
    Dec = 1143
    Hours = 19
    Minutes = 12
    pDay = 12
    pMonth = 10
    pYear = 2023
elif star == 2:
    name = 'Мирах'
    Ra = 69
    Dec = 2135
    Hours = 23
    Minutes = 15
    pDay = 19
    pMonth = 8
    pYear = 2023
elif star == 3:
    name = 'Кебалраи'
    Ra = 1064
    Dec = 273
    Hours = 23
    Minutes = 15
    pDay = 5
    pMonth = 6
    pYear = 2022
elif star == 4:
    name = 'Капелла'
    Ra = 318
    Dec = 2761
    Hours = 3
    Minutes = 39
    pDay = 21
    pMonth = 4
    pYear = 2022
print('Название: ', name)
print('Время:  ', Hours, ':', Minutes)
print('Дата:  ', pDay, '.', pMonth, '.', pYear)

# КОНСТАНТЫ
pNextPrevousDayIndicator = 0
pLongitude = 37.9111 * 0.0175             # с.ш.            (в градусах?)
pLatitude = 54.703 * 0.0175              # в.д.           (в радианах?)
pDs = 0                                 # переход на летнее время
pTz = 3

print('Долгота: ', pLongitude)
print('Широта:  ', pLatitude)


# ВОСХОЖДЕНИЕ И СКЛОНЕНИЕ С ЭНКОДЕРОВ
Ra_h = Ra * 0.016666667             # перевод в часы/градусы
Dec_rad = abs(Dec / 60) * 0.0175             # перевод в часы/градусы
print('Прямое. восх', round(Ra_h, 4))
print('Склонение гр.  ', round(Dec_rad, 4))


# ВЫСЧИТЫВАЕМ ДАТУ ПО ЮЛИАНСКОМУ КАЛЕНДАРЮ
A = math.floor(pYear / 100)
B = 2 - A + math.floor(A / 4)
JD = math.floor(365.25 * (pYear + 4716)) + math.floor(30.6001 * (pMonth + 1)) + pDay + (0.04 * Hours) + B - 1524.5
#print('Юлиан. кал.:     ', JD)


# ВЫЧИСЛЯЕМ ВСЕМИРНОЕ ВРЕМЯ ПО ГРИНВИЧУ
lT = (JD / 36525.0) - 1
print('lT:  ', round(lT, 2))
lR0 = lT * (0.0513366 + lT * (0.00002586222 - lT * 0.000000001722))
print('lR0:  ', round(lR0, 2))
lR1 = 6.697374558 + 2400.0 * (lT - ((pYear - 2000.0) / 100.0))
print('lR1:           ', round(lR1, 2))
lT0 = lR0 + lR1
print('lT0 начальное: ', round(lT0, 2))
while lT0 > 24:
    lT0 -= 24.0                                              # ХЭШ
while lT0 < 0:
    lT0 += 24.0                                              # ХЭШ
print('lT0 итоговое:  ', round(lT0, 2))


# ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ
pLocalTime = Hours + (0.016 * Minutes)
print('pLocalTime (десятич.): ', round(pLocalTime, 2))
pUT = pLocalTime - pDs - pTz
while pUT < 0:
    pUT += 24
    pNextPrevousDayIndicator = -1
while pUT > 24:
    pUT -= 24
    pNextPrevousDayIndicator = 1
if pNextPrevousDayIndicator == 1:
    lT0 += 0.0657098244
if pNextPrevousDayIndicator == -1:
    lT0 -= 0.0657098244
pUT_hour = int(pUT)
pUT_min = ((pUT - int(pUT))*0.6)*100
print('pUT: ', round(pUT, 2), '   ', pUT_hour, ':', round(pUT_min))


# ВЫЧИСЛЯЕМ ЗВЕЗДНОЕ ВРЕМЯ ПО ГРИНВИЧУ
#pGST = (pUT * 1.002737908) + lT0           # оригинал
pGST = (pUT * 0.997269625) + lT0 - 0.11           # ошибка меньше 0.11 поправочный коэф
while pGST > 24:
    pGST -= 24.0                                             # ХЭШ
while pGST < 0:
    pGST += 24.0                                             # ХЭШ
print('pGST:', round(pGST, 3), '   ', int(pGST), ':', round(((pGST - int(pGST))*0.6)*100))


# ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ (LST)
pLST = pGST + (pLongitude / 15.0)
while pLST > 24:
    pLST -= 24.0                                             # ХЭШ
while pLST < 0:
    pLST += 24.0                                             # ХЭШ
print('pLST:', round(pLST, 3)) #, '   ', int(pLST), ':', round(((pLST - int(pLST))*0.6)*100))


# ВЫЧИСЛЯЕМ ЧАСОВОЙ УГОЛ (hourAngel)
hourAngel_1 = pGST+(pLongitude / 15.0)   # долгота передается в градусах (переводим градусы в часы)
while hourAngel_1 > 24:
    hourAngel_1 -= 24.0                                      # ХЭШ
while hourAngel_1 < 0:
    hourAngel_1 += 24.0                                      # ХЭШ
print("Часовой угол_1: ", round(hourAngel_1, 4))
hourAngel = hourAngel_1 - Ra_h
while hourAngel > 24:
    hourAngel -= 24.0                                       # ХЭШ
while hourAngel < 0:
    hourAngel += 24.0                                       # ХЭШ
print("Часовой угол:   ", round(hourAngel, 4)) #, 'должен 21.96',  '   ', int(hourAngel), ':', round(((hourAngel - int(hourAngel))*0.6)*100))


# ВЫЧИСЛЯЕМ АЗИМУТ И ВЫСОТУ
AzEq = (hourAngel * 15.0) * 0.0175
pAltHor = math.asin((math.sin(Dec_rad) * math.sin(pLatitude)) + (math.cos(Dec_rad) * math.cos(pLatitude) * math.cos(AzEq)))
pAzHor = math.acos((math.sin(Dec_rad) - (math.sin(pLatitude) * math.sin(pAltHor))) / (math.cos(pLatitude) * math.cos(pAltHor)))
# Check if equatorial azimuth is > 180 degrees (sin is negative) and correct for horizontal azimuth
if math.sin(AzEq) > 0:
    pAzHor = 2 * math.pi - pAzHor
print("AzEq: ", round(AzEq, 2), 'радианы')
print("Азимут: ", round(pAzHor*57.3, 2))
print("Высота: ", round(pAltHor*57.3, 2))