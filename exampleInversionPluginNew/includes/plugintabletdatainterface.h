#ifndef PLUGINTABLETDATAINTERFACE_H
#define PLUGINTABLETDATAINTERFACE_H

#include <common.h>

class PluginTabletDataInterface
{
public:
	/**
	 * @brief Получение количества гридов выбраных пользователем
	 * 
	 * @return uint32 количество выбраных гридов
	 */
	virtual uint32 gridsCount() const = 0;
	
	/**
	 * @brief Получение значения с грида
	 * 
	 * @param ind : Индекс грида в массиве выбраных, должен быть меньше чем количество выбраных гридов, иначе поведение неопределено
	 * @param x : х-координата точки в которой брать значение
	 * @param y : y-координата точки в которой брать значение
	 * @param ok : ссылка на переменную куда запишется успешно ли получено значение
	 * @return double значение из грида в случае успеха, 0 в противном случае
	 */
	virtual double gridValue(uint32 ind, double x, double y, bool &ok) const = 0;
};


#endif // PLUGINTABLETDATAINTERFACE_H
