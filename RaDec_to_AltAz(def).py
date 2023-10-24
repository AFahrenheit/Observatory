import math

""""             1                    2                    3                    4
Имя           Arcturus              Mirach               Кebalrai              Сapella
pUT           (16:12)OK           (20:15)-1m           (20:15)1m             (00:39)-2m
pGST          (20:06)OK           (20.38)-2m           (15.44)-3m            (17.07)OK
Ч.У.          (5:51)+2m           (19:28)+1m           (21:59)+1m            (11:47)+4m
Азимут         (279) 280            (84) 84             (143) 143             (357) 358                
Высота        (16.5) 16.3         (40.5) 41.3           (33.3) 34            (11.52) 11 """


def test(star):
    if star == 1:
        name = 'Arcturus'
        Ra = 855
        Dec = 1143
        Hours = 19
        Minutes = 12
        pDay = 12
        pMonth = 10
        pYear = 2023
    elif star == 2:
        name = 'Mirach'
        Ra = 69
        Dec = 2135
        Hours = 23
        Minutes = 15
        pDay = 19
        pMonth = 8
        pYear = 2023
    elif star == 3:
        name = 'Кebalrai'
        Ra = 1064
        Dec = 273
        Hours = 23
        Minutes = 15
        pDay = 5
        pMonth = 6
        pYear = 2022
    elif star == 4:
        name = 'Kapella'
        Ra = 318
        Dec = 2761
        Hours = 3
        Minutes = 39
        pDay = 21
        pMonth = 4
        pYear = 2022
    return name, Ra, Dec, Hours, Minutes, pDay, pMonth, pYear
def dec_ra_to_radians(Ra, Dec):
    # ВОСХОЖДЕНИЕ И СКЛОНЕНИЕ С ЭНКОДЕРОВ
    Ra_h = Ra * 0.016666667  # перевод в часы/градусы
    Dec_rad = abs(Dec / 60 * 0.0175)  # перевод в часы/градусы
    return Ra_h, Dec_rad
def julian_date(pYear, pMonth, pDay, Hours):
    # ВЫСЧИТЫВАЕМ ДАТУ ПО ЮЛИАНСКОМУ КАЛЕНДАРЮ
    A = math.floor(pYear / 100)
    B = 2 - A + math.floor(A / 4)
    JD = math.floor(365.25 * (pYear + 4716)) + math.floor(30.6001 * (pMonth + 1)) + pDay + (0.04 * Hours) + B - 1524.5
    return JD
def universal_time(JD, pYear, Hours, Minutes, pDs, pTz):
    # ВЫЧИСЛЯЕМ ВСЕМИРНОЕ ВРЕМЯ ПО ГРИНВИЧУ
    ulT = (JD / 36525.0) - 1
    ulR0 = ulT * (0.0513366 + ulT * (0.00002586222 - ulT * 0.000000001722))
    ulR1 = 6.697374558 + 2400.0 * (ulT - ((pYear - 2000.0) / 100.0))
    ulT0 = norm_0_to_24(ulR0 + ulR1)
    pLTime = Hours + (0.016 * Minutes)
    pUT = pLTime - pDs - pTz
    norm_0_to_24(pUT)
    return pUT, ulT, ulT0, ulR0, ulR1, pLTime
def greenwich_sidereal_time(pUT, ulT0):
    # ВЫЧИСЛЯЕМ ЗВЕЗДНОЕ ВРЕМЯ ПО ГРИНВИЧУ
    # pGST = (pUT * 1.002737908) + lT0           # оригинал
    pGST = (pUT * 0.997269625) + ulT0 - 0.11  # ошибка меньше 0.11 поправочный коэф
    return norm_0_to_24(pGST)
def local_sidereal_time(pGST, pLongitude):
    # ВЫЧИСЛЯЕМ МЕСТНОЕ ЗВЕЗДНОЕ ВРЕМЯ (LST)
    pLST = pGST + (pLongitude / 15.0)
    return norm_0_to_24(pLST)
def hour_angel(pGST, pLongitude, Ra_h):
    # ВЫЧИСЛЯЕМ ЧАСОВОЙ УГОЛ
    #pHA_1 = pGST + (pLongitude / 15.0)  # pGST + (переводим градусы в часы)
    pHA_1 = pGST + (pLongitude / 15.0)  # pGST + (переводим градусы в часы)
    pHA = norm_0_to_24(pHA_1) - Ra_h
    return norm_0_to_24(pHA)
def dec_ra_to_alt_az(pHA, Dec_rad, pLatitude):
    # ВЫЧИСЛЯЕМ АЗИМУТ И ВЫСОТУ
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
    return AzEq, Az, Alt
def norm_0_to_24(x):
    while x > 24:
        x -= 24.0
    while x < 0:
        x += 24.0
    return x


# ТЕСТОВЫЕ ЗНАЧЕНИЯ
star = 2

# КОНСТАНТЫ
pLongitude = 37.9111 * 0.0175
pLatitude = 54.703 * 0.0175

name, Ra, Dec, Hours, Minutes, pDay, pMonth, pYear = test(star)
Ra_h, Dec_rad = dec_ra_to_radians(Ra, Dec)
JD = julian_date(pYear, pMonth, pDay, Hours)
pDs, pTz = 0, 3
pUT, ulT, ulT0, ulR0, ulR1, pLTime = universal_time(JD, pYear, Hours, Minutes, pDs, pTz)
pGST = greenwich_sidereal_time(pUT, ulT0)
pLST = local_sidereal_time(pGST, pLongitude)
pHA = hour_angel(pGST, pLongitude, Ra_h)
AzEq, Az, Alt = dec_ra_to_alt_az(pHA, Dec_rad, pLatitude)

""
print('Название: ', name)
print('Время:    ', str(Hours) + ':' + str(Minutes))
print('Дата:     ', str(pDay)+'.'+str(pMonth)+'.'+str(pYear), '\n')
print('Долгота:  ', round(pLongitude, 4))
print('Широта:   ', round(pLatitude, 4), '\n')
print('Прямое. восх:  ', round(Ra_h, 2))
print('Склонение гр.: ', round(Dec_rad, 3))
print('Юлиан. кал.:   ', JD, '\n')
print('ulT:   ', round(ulT, 2))
print('ulR0:  ', round(ulR0, 2))
print('ulR1:  ', round(ulR1, 2))
print('ulT0:  ', round(ulT0, 2),'\n')
print('pUT:  ', int(pUT), ':', round((pUT - int(pUT)) * 60))
print('pGST итог: ', int(pGST), ':', round((pGST - int(pGST)) * 60))
print('pLST: ', round(pLST, 2),'  ', int(pLST), ':', round((pLST - int(pLST)) * 60))
print('LocalTime:   ', pLTime, '  ', int(pLTime), ':', round((pLTime - int(pLTime)) * 60))
#print('Ч.У.: ', pHA_1)
print('Ч.У.: ', pHA, '  ',  int(pHA), ':', round((pHA - int(pHA)) * 60), '\n')
print('AzEq: ', round(AzEq, 2), 'радианы', '\n')
print('Азимут: ', round(Az, 1))
print('Высота: ', round(Alt, 1))
""