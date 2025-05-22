import pandas as pd
import matplotlib.pyplot as plt
import matplotlib

# Загрузка данных из csv файлов
points = pd.read_csv('C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/points.csv')
edges = pd.read_csv('C:/Users/nikit/source/repos/W-SEIS_DEV_NEW/edges.csv')

matplotlib.use('TkAgg')

# Создание фигуры
plt.figure(figsize=(8, 6))

pointsXUnique = points['x'].unique()
pointsYUnique = points['y'].unique()
# Установка диапазона осей
countByX = len(pointsXUnique)
plt.xlim(left=-1, right=countByX)
countByY = len(pointsYUnique)
plt.ylim(bottom=-1, top=countByY)
# Устанавливаем только целые числа на оси X и Y
plus = 10
step = 2
plt.xticks(range(min(pointsXUnique)-plus, max(pointsXUnique)+plus, step))
plt.yticks(range(min(pointsYUnique)-plus, max(pointsYUnique)+plus, step))
plt.grid(which='major', axis='both', linestyle='-', color='gray', linewidth=0.5, zorder = 0) # цвет сетки, размер

   # Отображение рёбер
plt.plot(
    [edges['startX'], edges['endX']],
    [edges['startY'], edges['endY']],
    color='black',
    zorder = 2,
    linewidth = 3
)

# Отображение точек
plt.scatter(points['x'], points['y'], color='black', zorder = 2)
# Поиск точки с координатами (0, 0)
target_x, target_y = 5, 10
red_point = points[(points['x'] == target_x) & (points['y'] == target_y)]
# Если такая точка найдена, покрасить её красным
if not red_point.empty:
    plt.scatter(red_point['x'], red_point['y'], color='red', zorder = 2)
else:
    print(f"Точка с координатами ({target_x}, {target_y}) не найдена.")

# Отображение рёбер
#for _, edge in edges.iterrows():
#    plt.plot([edge['startX'], edge['endX']], [edge['startY'], edge['endY']], color='white')


# Настройка осей и отображение графа
plt.xlabel('Индекс трассы в сейсмическом кубе вдоль оси X')
plt.ylabel('Индекс трассы в сейсмическом кубе вдоль оси Y')
plt.title('Остовное дерево')
plt.grid(True)
plt.axis('equal')

plt.show()
