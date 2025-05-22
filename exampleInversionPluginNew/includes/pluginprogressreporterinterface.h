#ifndef PLUGINPROGRESSREPORTERINTERFACE_H
#define PLUGINPROGRESSREPORTERINTERFACE_H

#include <QString>

class PluginProgressReporterInterface
{
public:
	virtual ~PluginProgressReporterInterface() {}
	
	/**
	 * @brief Выставить диапазон значений для прогресса
	 * 
	 * @param min : минимальное значение
	 * @param max : максимальное значение
	 */
	virtual void setProgressRange(int min, int max) = 0;
	/**
	 * @brief Выставить значение прогресса
	 * 
	 * @param value : значение для прогресса в диапазоне выставленном при помощи setProgressRange
	 */
	virtual void setPorgressValue(int value) = 0;
	/**
	 * @brief Установить пояснительную подпись для прогресса
	 * 
	 * @param text : текст подписи
	 */
	virtual void setProgressText(const QString& text) = 0;
	
	/**
	 * @brief Проверка за запрашивал ли пользователь отмену операции
	 * 
	 * @return bool true в случае необходимости завершить работу операции
	 */
	virtual bool wasCanceled() = 0;
	
	/**
	 * @brief Записать сообщение в журнал событий Десманы
	 * 
	 * @param text : текст сообщения
	 */
	virtual void logMessage(const QString& text) = 0;
	/**
	 * @brief Записать предупреждение в журнал событий Десманы
	 * 
	 * @param text : текст предупреждения
	 */
	virtual void logWarning(const QString& text) = 0;
	/**
	 * @brief Записать ошибку в журнал событий Десманы
	 * 
	 * @param text : текст ошибки
	 */
	virtual void logError(const QString& text) = 0;
};

#endif // PLUGINPROGRESSREPORTERINTERFACE_H
