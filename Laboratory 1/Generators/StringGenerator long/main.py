import rstr
import random
import string
from tqdm import trange

#Создали массив со строками нужной размерности (потом запихнем его в файл)
count = 1
strMas = [0] * count
optimas = [0] * count

#Начинаем портить строки по одной
for i in trange(count):
    mas = []  # буффер, массив символов новой строки
    if random.randint(0, 3) == 0:
        strMas[i] = rstr.xeger('nfs://[a-zA-Z]{1,15}/(?:([a-zA-Z._]{1,20}/'
                               '([a-zA-Z._]{1,12})|([a-zA-Z._]{1,12})))') #нормальная строка
    else:
        strMas[i] = rstr.xeger('.{0,1}nfs://') #не очень, но нормальная строка
        len = 25000

        for j in range(int(len/100)):
            strMas[i] += rstr.xeger('(?:([a-zA-Z._]{1,100}/'
                               '([a-zA-Z._]{1,100})|([a-zA-Z._]{1,100}))|([a-zA-Z]{1,100}/))')

    if random.randint(0, 2) != 0:
        for j in strMas[i]: #для каждого символа в старой строке
            if random.randint(0, 10) == 0: # c вероятностью (0 1 2 3) 0.25 портим строку
                j = chr((ord(j) + random.randint(0, 255)) % 255) #измененный символ
            mas.append(j)  # добавляем символ (нормальный или испорченный)
        strMas[i] = ''.join(mas)  # перезаписываем в массив новую строку
        optimas[i] = ''.join(c for c in mas if c in string.printable)  # убираем спец. символы
    else:
        optimas[i] = strMas[i]

#здесь все строки уже изменены и записаны в strMas
f = open("Generated_strings.txt", "w", encoding="utf-8")
f.seek(0, 0)
for i in range(count):
    f.write(optimas[i] + "\n")
f.close()





