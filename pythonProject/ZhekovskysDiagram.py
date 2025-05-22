import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

# Функция для замены некорректных значений на NaN
def replace_invalid_values(line):
    return line.replace('-nan(ind)', 'nan')

# Чтение файла с заменой некорректных значений
with open('C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/A_jekowsky.txt', 'r') as file:
    lines = file.readlines()
    cleaned_lines = [replace_invalid_values(line) for line in lines]
# Загрузка данных из файла
data = np.loadtxt(cleaned_lines)
# Замените nan на 0 для визуализации (или другим значением по вашему выбору)
data_without_nan = np.nan_to_num(data)
# Определяем цветовую карту
cmap = plt.get_cmap('jet')
norm = mcolors.Normalize(vmin=np.nanmin(data), vmax=np.nanmax(data))

# Создание графика
plt.figure(figsize=(8, 6))
plt.imshow(data_without_nan, cmap=cmap, norm=norm)

# Добавление цветовой шкалы
cbar = plt.colorbar()
cbar.set_label('Значения похожести пары сигналов')

# Добавление подписей
#for (i, j), val in np.ndenumerate(data):
#    if not np.isnan(val):  # Не отображаем значения, если это nan
#       plt.text(j, i, f'{val:.2f}', ha='center', va='center', color='white')

# Настройки отображения
plt.xlabel('Индекс сигнала в 1 трассе')
plt.ylabel('Индекс сигнала во 2 трассе')
plt.title('Корреляционный планшет')

step = 3
#plt.xticks(ticks=np.arange(data.shape[1]), labels=[f' {i + 1}' for i in range(data.shape[1])])
plt.xticks(ticks=np.arange(0, data.shape[1], step),
           labels=[f'{i + 1}' for i in range(0, data.shape[1], step)],
           rotation=45, ha='right')
#plt.yticks(ticks=np.arange(data.shape[0]), labels=[f' {i + 1}' for i in range(data.shape[0])])
plt.yticks(ticks=np.arange(0, data.shape[0], step),
           labels=[f'{i + 1}' for i in range(0, data.shape[0], step)])
plt.grid(False)

def drawCorrelationPath():
    # Координаты для линии
    coordinates = np.loadtxt('C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/path.txt')
    # Добавление линии по координатам
    plt.plot(coordinates[:, 1], coordinates[:, 0], color='red', marker='o', label='Линия по координатам', markersize=1)

drawCorrelationPath()

# Показать график
plt.show()

