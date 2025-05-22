#ifndef INVERSIONPLUGININTERFACE_H
#define INVERSIONPLUGININTERFACE_H

#include <QtPlugin>
#include <common.h>
#include "pluginprogressreporterinterface.h"
#include "plugintabletdatainterface.h"

class QString;
class QStringList;

class InversionCubeDataInterface
{
public:	
	virtual ~InversionCubeDataInterface() = default;
	
	
	virtual QString getType() const = 0;
	
	/**
	 * @brief Получение количества инлайнов в кубе
	 */
	virtual uint16 ilCount() const = 0;
	
	/**
	 * @brief Получение количества кросслайнов в кубе
	 */
	virtual uint16 clCount() const  = 0;
	
	/**
	 * @brief шаг дискретизации в микросекундах
	 */
	virtual uint16 sampleInterval() const = 0;
	
	/**
	 * @brief шаг дискретизации в миллисекундах
	 */
	virtual double sampleIntervalMs() const = 0;
	
	/**
	 * @brief  Получение индекса первого отсчёта в области загруженных данных куба
	 * 
	 * @param il : Индекс инлайна куба, значения в пределах 0 - ilCount-1
	 * @param cl : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @return int32 индекс отсчёта, или -1 в случае если выбранный горизонт не проведён но трассе
	 */
	virtual int32 beginSample(uint16 il, uint16 cl) const  = 0;
	
	/**
	 * @brief  Получение индекса отсчёта следующего за последним в области загруженных данных куба
	 *
	 * @param il : Индекс инлайна куба, значения в пределах 0 - ilCount-1
	 * @param cl : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @return int32 индекс отсчёта, или -1 в случае если выбранный горизонт не проведён но трассе
	 */
	virtual int32 endSample(uint16 il, uint16 cl) const  = 0;
	
	/**
	 * @brief Получение амплитуды в sample отчёте в трассе с индексами il cl
	 * 
	 * @param il : Индекс инлайна куба, значения в пределах 0 - ilCount-1
	 * @param cl : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @param sample : индекс отсчёта, значения в пределах beginSample - endSample-1
	 */
	virtual double traceData(uint16 il, uint16 cl, uint16 sample) const = 0;
	
	/**
	 * @brief Проверка существования трассы в кубе
	 * 
	 * @param il : Индекс инлайна куба, значения в пределах 0 - ilCount-1
	 * @param cl : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @return bool true если существует, иначе false
	 */
	virtual bool isTraceExist(uint16 il, uint16 cl) const  = 0;
	
	/**
	 * @brief Получение количества горизонтов выбранных пользователем.
	 */
	virtual uint16 horizonsCount() const = 0;
	
	/**
	 * @brief Проверка проведён ли горизонт на данном кубе
	 * 
	 * @param ind   : Индекс горизонта, значения в пределах 0 - horizonsCount()-1
	 * @return bool : true в случае если горизонт проведён где-либо на данном кубе
	 *                и индекс в допустимых пределах. иначе false
	 */
	virtual bool isHorizonExist(uint16 ind) const = 0;
	
	/**
	 * @brief Полечение значения горизонта в трассе куба
	 * 
	 * @param ind   : Индекс горизонта, значения в пределах 0 - horizonsCount()-1
	 * @param il    : Индекс инлайна куба, значения в пределах 0 - ilCount-1
	 * @param cl    : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @param ok    : Было ли найдено значение горизонта. Выставляется в:
	 *                false - если трасса попадает в разрыв горизонта 
	 *                        или горизонт не был проведён на данной трассе
	 *                        или указан неверный индекс горизонта
	 *                true - иначе
	 * @return double : значение горизонта если он попадает на текущую трассу,
	 *                  иначе неопределено
	 */
	virtual float horizonValue(uint16 ind, uint16 il, uint16 cl, bool& ok) const = 0;
};

class InversionResultInterface
{
public:
	virtual ~InversionResultInterface() = default;
	
	// для удобства
	/**
	 * @brief Получение количества инлайнов в кубе
	 */
	virtual uint16 ilCount() const = 0;
	
	/**
	 * @brief Получение количества кросслайнов в кубе
	 */
	virtual uint16 clCount() const  = 0;
	
	/**
	 * @brief шаг дискретизации в микросекундах
	 */
	virtual uint16 sampleInterval() const = 0;
	
	/**
	 * @brief шаг дискретизации в миллисекундах
	 */
	virtual double sampleIntervalMs() const = 0;
	
	/**
	 * @brief Проверка существования трассы в кубе
	 * 
	 * @param il : Индекс инлайнка куба, значения в пределах 0 - ilCount-1
	 * @param cl : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @return bool true если существует, иначе false
	 */
	virtual bool isTraceExist(uint16 il, uint16 cl) const  = 0;
	//
	
	/**
	 * @brief Записать значение для отсчёта в куб-результат
	 * 
	 * @param il : Индекс инлайнка куба, значения в пределах 0 - ilCount-1
	 * @param cl : Индекс кросслайна куба, значения в пределах 0 - clCount-1
	 * @param sample : Индекс отсчёта в кубе
	 * @param float : Значение для отсчёта
	 */
	virtual void setResultSample(uint16 il, uint16 cl, uint16 sample, float value) = 0;
};

class InversionPluginInterface
{
public:
	virtual ~InversionPluginInterface() = default;
	
	/** 
	 * @brief  Получение имён типов преобразования в данном плигине; формат: shortName0, displayName0, shortName1, displayName1...
	 *         shortName - короткое имя для разделения типов преобразования в данном плагине
	 *         displayName - имя преобразования для вывода в интерфейсе
	 */
	virtual QStringList inversionTypes() const = 0;
	
	/** 
	 * @brief  Получение имён типов кубов которые требуются типу преобразования
	 * @param shortName : короткое имя типа преобразования
	 */
	virtual QStringList cubeTypes(const QString& shortName) const = 0;
	
	/** 
	 * @brief  Получение имени типа куба которое считается базовым для типа преобразования.
	 *         Этот тип должен присутствовать в списке, который возвращается из cubeTypes(),
	 *         желательно первым.
	 * @param shortName : короткое имя типа преобразования
	 */
	virtual QString baseCubeType(const QString& shortName) const = 0;
	
	/**
	 * @brief Инициализация виджета с настройками
	 * @param shortName : короткое имя типа преобразования
	 * @param widget : указатель на виджет-родитель для инициализации
	 */
	virtual void initInversionSettingsWidget(const QString& shortName, QWidget* widget) = 0;
	
	/**
	 * @brief Провести инверсию
	 * 
	 * @param shortName : короткое имя типа преобразования
	 * @param uiWidget   : виджет параметров для выбранного типа преобразования
	 * @param progressReporter : указатель на объект для отсылки прогресса и записей в журнал, а так же проверки запроса на отмену
	 * @param cubesData : таблица объектов для получения данных с кубов выбранных пользователем,
	 *                    ключ - тип куба из cubeTypes()
	 * @param tabletData : указатель на объект для получения данных с предметов планшета (гридов)
	 * @param resultData : указатель на объект для записи результата
	 * @return bool : true если преобразование прошло без ошибок, иначе false
	 */
	virtual bool doInversion(const QString& shortName, QWidget* uiWidget
		,PluginProgressReporterInterface* progressReporter
		,const QHash< QString, QVector<InversionCubeDataInterface*> >& cubesData
		,PluginTabletDataInterface* tabletData
		,InversionResultInterface* resultData
	) = 0;
	
	/**
	 * @brief Получение текста ошибки, если она произошла.
	 */
	virtual QString getLastError() const = 0;
};

#define InversionPluginInterface_iid "ru.ipgg.Desmana.InversionPluginInterface/2.0"
Q_DECLARE_INTERFACE(InversionPluginInterface, InversionPluginInterface_iid)

#endif // INVERSIONPLUGININTERFACE_H
