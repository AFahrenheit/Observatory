import math

""""             1                    2                    3                    4
Имя           Arcturus              Mirach              Кebalrai              Сapella
pUT           (16:12)OK           (20:15)-1m           (20:15)1m             (00:39)-2m
pGST          (20:06)OK           (20.38)-2m           (15.44)-3m            (17.07)OK
Ч.У.          (5:51)+2m           (19:28)+1m           (21:59)+1m            (11:47)+4m
Азимут       (279) 280.2          (84) 84.3            (143) 143.8           (357) 358.8                
Высота       (16.5) 16.3         (40.5) 41.3           (33.3) 34.5          (11.52) 11.8 """


def test(test_star):
    '''
    ТЕСТОВЫЕ ЗНАЧЕНИЯ
    подставляет табличные значения для проверки вычислений

    :param test_star: значение в диапазоне от 1 до 4
    :return:
            name: название звезды
            Ra: прямое восхождение в минутах
            Dec: склонение в минутах
            Hours: текущий час
            Minutes: текущая минута
            pDay: день (число / дата)
            pMonth: месяц
            pYear: год
    '''
    if test_star == 1:
        name = 'Arcturus'
        Ra = 855
        Dec = 1143
        Hours = 19
        Minutes = 12
        pDay = 12
        pMonth = 10
        pYear = 2023
    elif test_star == 2:
        name = 'Mirach'
        Ra = 69
        Dec = 2135
        Hours = 23
        Minutes = 15
        pDay = 19
        pMonth = 8
        pYear = 2023
    elif test_star == 3:
        name = 'Кebalrai'
        Ra = 1064
        Dec = 273
        Hours = 23
        Minutes = 15
        pDay = 5
        pMonth = 6
        pYear = 2022
    elif test_star == 4:
        name = 'Kapella'
        Ra = 318
        Dec = 2761
        Hours = 3
        Minutes = 39
        pDay = 21
        pMonth = 4
        pYear = 2022
    return name, Ra, Dec, Hours, Minutes, pDay, pMonth, pYear
    
def dec_ra_to_radians(Ra: int, Dec: int) -> float:
    '''
    ВОСХОЖДЕНИЕ И СКЛОНЕНИЕ С ЭНКОДЕРОВ
    текущее положение телескопа узнаём исходя из полученных шагов энкодера при повороте телескопа

    :param Ra: прямое восхождение
    :param Dec: склонение
    :return:
            Ra_h: прямое восхождение в часах
            Dec_rad: склонение в радианах
    '''
    Ra_h = Ra * 0.016666667
    Dec_rad = abs(Dec / 60 * 0.0175)
    return Ra_h, Dec_rad
    
def julian_date(pYear: int, pMonth: int, pDay: int) -> float:   # , Hours: int
    '''
    ВЫСЧИТЫВАЕМ ДАТУ ПО ЮЛИАНСКОМУ КАЛЕНДАРЮ

    :param pYear: год
    :param pMonth: месяц
    :param pDay: день (число)
    :param Hours: час
    :return:
            JD: дата по Юлианскому календарю
    '''
    A = math.floor(pYear / 100)
    B = 2 - A + math.floor(A / 4)
    #JD = math.floor(365.25 * (pYear + 4716)) + math.floor(30.6001 * (pMonth + 1)) + pDay + (0.04 * Hours) + B - 1524.5
    JD = math.floor(365.25 * (pYear + 4716) + 30.6001 * (pMonth + 1)) + pDay + B - 1524.5
    return JD
    
def universal_time(JD: float, pYear: int, Hours: int,
                   Minutes: int, pDs: int, pTz: int) -> float:
    '''
    ВЫЧИСЛЯЕМ ВСЕМИРНОЕ ВРЕМЯ ПО ГРИНВИЧУ

    :param JD: дата по Юлианскому календарю
    :param pYear: год
    :param Hours: час
    :param Minutes: минуты
    :param pDs: часовой пояс
    :param pTz: лентнее время (0- 1)
    :return:
             pUT: всемирное время по Гринвичу
             ulT0: ulT0
    '''
    ulT = (JD / 36525.0) - 1
    ulR0 = ulT * (0.0513366 + ulT * (0.00002586222 - ulT * 0.000000001722))
    ulR1 = 6.697374558 + 2400.0 * (ulT - ((pYear - 2000.0) / 100.0))
    ulT0 = norm_0_to_24(ulR0 + ulR1)
    pLTime = Hours + (0.016 * Minutes)
    pUT = pLTime - pDs - pTz
    norm_0_to_24(pUT)
    return pUT, ulT0 #, ulT, ulR0, ulR1, pLTime
    
def greenwich_sidereal_time(pUT: float, ulT0: float) -> float:
    '''
    ВЫЧИСЛЯЕМ ЗВЕЗДНОЕ ВРЕМЯ ПО ГРИНВИЧУ

    :param pUT: всемирное время по Гринвичу
    :param ulT0: ulT0
    :return:
            pGST: звездное время по Гринвичу в диапазоне от 0 до 24
    '''
    # pGST = (pUT * 1.002737908) + ulT0           # оригинал
    pGST = (pUT * 0.997269625) + ulT0 - 0.11     # ошибка меньше 0.11 поправочный коэф
    return norm_0_to_24(pGST)
    
def local_sidereal_time(pGST: float, pLongitude: float) -> float:
    '''
    ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ (LST)

    :param pGST: звездное время по Гринвичу
    :param pLongitude: долгота
    :return:
            pLST: местное звездное время

    '''
    pLST = pGST + (pLongitude / 15.0)
    return norm_0_to_24(pLST)
    
def hour_angel(pGST: float, pLongitude: float, Ra_h: float) -> float:
    '''
    ВЫЧИСЛЯЕМ ЧАСОВОЙ УГОЛ

    :param pGST: звездное время по Гринвичу
    :param pLongitude: долгота
    :param Ra_h: прямое восхождение в часах
    :return:
            pHA: часовой угол
    '''
    pHA_1 = pGST + (pLongitude / 15.0)  # pGST + (переводим градусы в часы)
    pHA = norm_0_to_24(pHA_1) - Ra_h
    return norm_0_to_24(pHA)
    
def dec_ra_to_alt_az(pHA: float, Dec_rad: float, pLatitude: float) -> float:
    '''
    ВЫЧИСЛЯЕМ АЗИМУТ И ВЫСОТУ

    :param pHA: часовой угол
    :param Dec_rad: склонение в радианах
    :param pLatitude: широта
    :return:
            Az: азимут
            Alt: высота
    '''
    AzEq = (pHA * 15.0) * 0.0175
    pAltHor = math.asin(
        (math.sin(Dec_rad) * math.sin(pLatitude)) + (math.cos(Dec_rad) * math.cos(pLatitude) * math.cos(AzEq)))
    pAzHor = math.acos(
        (math.sin(Dec_rad) - (math.sin(pLatitude) * math.sin(pAltHor))) / (math.cos(pLatitude) * math.cos(pAltHor)))
    # Check if equatorial azimuth is > 180 degrees (sin is negative) and correct for horizontal azimuth
    if math.sin(AzEq) > 0:
        pAzHor = 2 * math.pi - pAzHor
    Az = pAzHor * 57.3
    Alt = pAltHor * 57.3
    return Az, Alt, #AzEq
    
def norm_0_to_24(x: float)  -> float:
    '''
    ПРИВЕДЕНИЕ К ДИАПАЗОНУ ОТ 0 ДО 24

    :param x: параметр для приведения
    :return:
            x: приведенное к диапазону от 0 до 24
    '''
    while x > 24:
        x -= 24.0
    while x < 0:
        x += 24.0
    return x


# ТЕСТОВЫЕ ЗНАЧЕНИЯ
test_star = 4

# КОНСТАНТЫ ДЛЯ МОЕГО РАСПОЛОЖЕНИЯ
pLongitude = 37.9111 * 0.0175
pLatitude = 54.703 * 0.0175

# ПЕРЕМЕННЫЕ
name, Ra, Dec, Hours, Minutes, pDay, pMonth, pYear = test(test_star)
Ra_h, Dec_rad = dec_ra_to_radians(Ra, Dec)
JD = julian_date(pYear, pMonth, pDay)   # , Hours
pDs, pTz = 0, 3
pUT, ulT0 = universal_time(JD, pYear, Hours, Minutes, pDs, pTz) # ulT, ulR0, ulR1, pLTime
pGST = greenwich_sidereal_time(pUT, ulT0)
pLST = local_sidereal_time(pGST, pLongitude)
pHA = hour_angel(pGST, pLongitude, Ra_h)
Az, Alt = dec_ra_to_alt_az(pHA, Dec_rad, pLatitude) # AzEq,

# ВЫВОД ИНФОРМАЦИИ
print('Название: ', name)
print('Время:    ', str(Hours) + ':' + str(Minutes))
print('Дата:     ', str(pDay)+'.'+str(pMonth)+'.'+str(pYear), '\n')
print('Долгота:  ', round(pLongitude, 4))
print('Широта:   ', round(pLatitude, 4), '\n')
print('Прямое. восх:  ', round(Ra_h, 2))
print('Склонение гр.: ', round(Dec_rad, 3))
print('Юлиан. кал.:   ', JD, '\n')
# print('ulT:   ', round(ulT, 2))
# print('ulR0:  ', round(ulR0, 2))
# print('ulR1:  ', round(ulR1, 2))

#print('ulT0:  ', round(ulT0, 2),'\n')
print('pUT:  ', round(pUT, 2), '   ', int(pUT), ':', round((pUT % 1) * 60))
print('pGST: ', round(pGST, 2), '  ', int(pGST), ':', round((pGST % 1) * 60))
print('pLST: ', round(pLST, 2),'  ', int(pLST), ':', round((pLST % 1) * 60))
# print('LocalTime:   ', pLTime, '  ', int(pLTime), ':', round((pLTime - int(pLTime)) * 60))
print('Ч.У.: ', round(pHA, 2), '  ',  int(pHA), ':', round((pHA % 1) * 60), '\n')
# print('AzEq: ', round(AzEq, 2), 'радианы', '\n')
print('Азимут: ', round(Az, 1))
print('Высота: ', round(Alt, 1))
