# отображение двух сигналов и отображений

import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import make_interp_spline

# Чтение данных из файла
with open('C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/A_couple_traces.txt', 'r') as file:
    lines = file.readlines()

# Извлечение данных для сигналов
signal1_length = int(lines[0].strip())  # Длина первого сигнала
signal1 = list(map(float, lines[1].strip().split()))  # Данные первого сигнала
signal2_length = int(lines[2].strip())  # Длина второго сигнала
signal2 = list(map(float, lines[3].strip().split()))  # Данные второго сигнала

###  Код для интерполяции, чтобы сделать более плавный график  ###
# Создание массива индексов для исходных данных
#x1 = np.arange(len(signal1))
#x2 = np.arange(len(signal2))
# Создание новых точек для интерполяции
#x1_new = np.linspace(x1.min(), x1.max(), 300)  # 300 точек для сглаживания
#x2_new = np.linspace(x2.min(), x2.max(), 300)
# Интерполяция с использованием сплайнов
#spline1 = make_interp_spline(x1, signal1, k=3)  # k=3 для кубического сплайна
#smooth_signal1 = spline1(x1_new)
#signal1 = smooth_signal1
#spline2 = make_interp_spline(x2, signal2, k=3)
#smooth_signal2 = spline2(x2_new)
#signal2 = smooth_signal2

# Создание графика
plt.figure(figsize=(10, 6))

# Отображение сигнала 1
plt.plot(signal1, label='Сигнал 1', color='blue')

# Отображение сигнала 2
plt.plot(signal2, label='Сигнал 2', color='red')

# Добавление заголовка и меток осей
#plt.title('Сопоставление сигналов пары трасс с использованием DTW алгоритма')
plt.title('Сопоставление сигналов пары трасс с использованием DTW алгоритма')
plt.xlabel('Индекс сигнала')
plt.ylabel('Амплитуда сигнала')

def drawMapping():
    # Координаты для соединения точек двух трасс
    coordinates = np.loadtxt('C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/path.txt')
    # Добавление соединений между точками
    for idx1, idx2 in coordinates:
        # Преобразуем индексы в целые числа
        idx1 = int(idx1)
        idx2 = int(idx2)
        # Рисуем линию между точками
        plt.plot([idx1, idx2], [signal1[idx1], signal2[idx2]], color='green', linestyle='--', linewidth=1)

drawMapping()

# Добавление легенды
plt.legend()

# Отображение графика
plt.show()