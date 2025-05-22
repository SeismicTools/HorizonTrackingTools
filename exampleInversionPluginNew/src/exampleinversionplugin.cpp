#include "exampleinversionplugin.h"
#include "ui_ex1form.h"
#include "ui_ex2form.h"
#include <FotiadiConnector.h>
#include <QFile>
#include "PathHelper.h"
#include <QJsonDocument>
#include <AppSettings.h>

const QStringList ExampleInversionPlugin::i1CubeTypes({ "Тип1", "Тип2" });
const QStringList ExampleInversionPlugin::i2CubeTypes({ "Краткое описание" });
const QStringList ExampleInversionPlugin::i3CubeTypes({ "Краткое описание" });

ExampleInversionPlugin::ExampleInversionPlugin(QObject *parent)
: QObject(parent)
{
}

ExampleInversionPlugin::~ExampleInversionPlugin()
{
}

QStringList ExampleInversionPlugin::inversionTypes() const
{
	QStringList names;
	names << QStringLiteral("ex1") << QString("ТЕСТЕР1") << 
		QStringLiteral("ex2") << QString("Метод для заполнения куба номерами границ.") <<
		QStringLiteral("ex3") << QString("Корреляция опорной трассы ко всем остальным.");
	return names;
}

QStringList ExampleInversionPlugin::cubeTypes(const QString &shortName) const
{
	if(shortName==QStringLiteral("ex1"))
	{
		return ExampleInversionPlugin::i1CubeTypes;
	}
	else if(shortName==QStringLiteral("ex2"))
	{
		return ExampleInversionPlugin::i2CubeTypes;
	}
	else if (shortName == QStringLiteral("ex3"))
	{
		return ExampleInversionPlugin::i3CubeTypes;
	}
	else
		return QStringList();
}

QString ExampleInversionPlugin::baseCubeType(const QString &shortName) const
{
	if(shortName==QStringLiteral("ex1"))
	{
		return ExampleInversionPlugin::i1CubeTypes.first();
	}
	else if(shortName==QStringLiteral("ex2"))
	{
		return ExampleInversionPlugin::i2CubeTypes.first();
	}
	else if (shortName == QStringLiteral("ex3"))
	{
		return ExampleInversionPlugin::i3CubeTypes.first();
	}
	else
		return QString();
}

void ExampleInversionPlugin::initInversionSettingsWidget(const QString &shortName, QWidget *widget)
{
	if(shortName==QStringLiteral("ex1"))
	{
		Ui::Ex1UI ex1Ui;
		ex1Ui.setupUi(widget);
	}
	else if(shortName==QStringLiteral("ex2"))
	{
		Ui::Ex2UI ex2Ui;
		ex2Ui.setupUi(widget);

		QLabel* label = ex2Ui.label;
		label->setText("Размер диагонали в корреляционном планшете");
		// тут третий аргумент виджет-родитель, чтобы лямбда точно не выполнялась если он не существует
		//connect(ex2Ui.spinBox, qOverload<int>(&QSpinBox::valueChanged), widget, [=](int value){ label->setText(QString::number(value)); });
	}
	else if (shortName == QStringLiteral("ex3"))
	{
		Ui::Ex2UI ex2Ui;
		ex2Ui.setupUi(widget);

		QLabel* label = ex2Ui.label;
		label->setText("Размер диагонали в корреляционном планшете");
		// тут третий аргумент виджет-родитель, чтобы лямбда точно не выполнялась если он не существует
		//connect(ex2Ui.spinBox, qOverload<int>(&QSpinBox::valueChanged), widget, [=](int value) { label->setText(QString::number(value)); });
	}
}

bool ExampleInversionPlugin::doInversion(const QString &shortName, QWidget *uiWidget
										 ,PluginProgressReporterInterface *progressReporter
										 ,const QHash<QString, QVector<InversionCubeDataInterface *> > &cubesData
										 ,PluginTabletDataInterface *tabletData
										 ,InversionResultInterface *resultData
										)
{
	m_error = QString();

	AppSettings appSettings;

	std::string filePath = "\\plugins\\fotiadi_math_internal\\appsettings.json";
	QString pathToFotiadiMathAppsettings
		= QString::fromStdString(PathHelper::GetFullPathRelativeStartUpDirectoryOfApplication(filePath));
	QFile file(pathToFotiadiMathAppsettings);
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Не удалось открыть файл:" << file.errorString();
		return false;
	}
	QByteArray jsonData = file.readAll();
	file.close();
	QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
	if (jsonDoc.isNull()) {
		qDebug() << "Не удалось разобрать JSON:" << pathToFotiadiMathAppsettings;
		return false;
	}
	if (jsonDoc.isObject()) {
		QJsonObject jsonObj = jsonDoc.object();
		if (jsonObj.contains("InlineCount")) {
			appSettings.InlineCount = jsonObj["InlineCount"].toInt();
		}
		if (jsonObj.contains("CrosslineCount")) {
			appSettings.CrosslineCount = jsonObj["CrosslineCount"].toInt();
		}
		if(jsonObj.contains("RefTraceInline"))
		{
			appSettings.RefTraceInline = jsonObj["RefTraceInline"].toInt();
		}
		if (jsonObj.contains("RefTraceCrossline"))
		{
			appSettings.RefTraceCrossline = jsonObj["RefTraceCrossline"].toInt();
		}
		if (jsonObj.contains("Ridx"))
		{
			appSettings.Ridx = jsonObj["Ridx"].toInt();
		}
		if(jsonObj.contains("HorizonsBreadth"))
		{
			appSettings.HorizonsBreadth = jsonObj["HorizonsBreadth"].toInt();
		}
		if (jsonObj.contains("SpaceBetweenHorizons"))
		{
			appSettings.SpaceBetweenHorizons = jsonObj["SpaceBetweenHorizons"].toInt();
		}
	}

	if(shortName==QStringLiteral("ex1"))
	{
		// проверка что виджет настроек прислали тот что надо
		// проверяемое имя - то что задано в .ui файле
		if(!uiWidget || uiWidget->objectName()!="Ex1UI" || !progressReporter)
		{
			m_error = "Неправильный виджет настроек";
			return false;
		}
		// ищем нужные нам формы по именам, что были заданы в .ui файле
		QComboBox* opCombo = uiWidget->findChild<QComboBox*>("opCombo");
		if(!opCombo)
		{
			m_error = "Неправильный виджет настроек";
			return false;
		}

		bool minusOP = (opCombo->currentIndex()==0 ? false : true);

		// базовый тип куба (возвращается из baseCubeType()) - берётся как куб-основа
		// из него берутся горизонты для определения границ и берётся набор трасс за основу
		// перед запуском плагина проверяется что у всех кубов их количество совпадают

		// проверяю всех ли типов кубы определены
		// можно так же проверить количество кубов определённого типа в массиве, если это важно
		// тип определённый как базовый всегда должен присутсвовать, но, на всякий случай можно проверить
		foreach(const QString& type, ExampleInversionPlugin::i1CubeTypes)
		{
			if(cubesData.contains(type)==false || cubesData.value(type).isEmpty())
			{
				return false;
			}
		}

		// как минимум один куб должен быть, если прошло проверку ранее
		InversionCubeDataInterface *cube1 = cubesData.value(i1CubeTypes.at(0)).at(0);
		InversionCubeDataInterface *cube2 = cubesData.value(i1CubeTypes.at(1)).at(0);

		// проверка наличия горизонта что нужен данному алгоритму
		if( cube1->horizonsCount()==0 // всего выбран хотябы один горизонт
			|| cube1->isHorizonExist(0) == false // на кубе первого типа проведён первый выбраный горизонт
		)
		{
			m_error = QString("Необходимо выбрать один горизонт проведёный на кубе с типом %1").arg(i1CubeTypes.at(0));
			return false;
		}

		// поскольку трассы совпадают можно брать как с результата, так и с слюбого другого куба
		uint16 ilCount = resultData->ilCount();
		uint16 clCount = resultData->clCount();

		progressReporter->setProgressRange(0,ilCount);

		for(uint16 il=0; il<ilCount; ++il)
		{
			progressReporter->setPorgressValue(il);

			for(uint16 cl=0; cl<clCount; ++cl)
			{
				if(progressReporter->wasCanceled())
					return false;

				if(resultData->isTraceExist(il,cl))
				{
					// рассчёт в какой отсчёт попадает значение с горизонта
					bool horizonValue_ok;
					float v = cube1->horizonValue(0,il,cl,horizonValue_ok);
					uint16 horizonSample = 0;
					if(horizonValue_ok)
						horizonSample = v/cube1->sampleIntervalMs();

					// рассчёт диапазона не бесплатный - лучше не использовать в условиях цикла
					uint16 sBegin = cube1->beginSample(il,cl);
					uint16 sEnd   = cube1->endSample(il,cl);
					double sum = 0;
					for(uint16 s=sBegin; s<sEnd; ++s)
					{
						if(horizonValue_ok && std::abs(s-horizonSample) < 5)
						{
							// зануление вокруг горизонта
							resultData->setResultSample(il,cl,s,0.0f);
						}
						else
						{
							double v1 = cube1->traceData(il,cl,s);
							double v2 = cube2->traceData(il,cl,s);
							resultData->setResultSample(il,cl,s,static_cast<float>(minusOP? v1-v2 : v1+v2));
						}
					}
				}
			}
		}
		progressReporter->setPorgressValue(ilCount);
		return true;
	}
	else if(shortName==QStringLiteral("ex2"))
	{
		QSlider* maxShiftopCombo = uiWidget->findChild<QSlider*>("horizontalSlider");
		auto maxShift = maxShiftopCombo->value();

		foreach(const QString & type, ExampleInversionPlugin::i2CubeTypes)
		{
			if (cubesData.contains(type) == false || cubesData.value(type).isEmpty())
			{
				progressReporter->logMessage("Cube wasn't selected!");
				return false;
			}
		}
		InversionCubeDataInterface* cube1 = cubesData.value(i2CubeTypes.at(0)).at(0);

		auto value = progressReporter->wasCanceled();
		if(value)
		{
			return false;
		}

		FotiadiConnector connector(resultData, cube1, maxShift, progressReporter, appSettings);
		progressReporter->setProgressRange(0, 1000);
		return connector.DrawS3DCubeWithReflectionHorizons();
		
		// примеры логгирования
		if(progressReporter)
		{
			progressReporter->logMessage("CJJ<OTYBT!");
		}
		qDebug() << "test1";
		qWarning() << "test2";
		qCritical() << "test3";
		return false;
	}
	else if (shortName == QStringLiteral("ex3"))
	{
		QSlider* maxShiftopCombo = uiWidget->findChild<QSlider*>("horizontalSlider");
		auto maxShift = maxShiftopCombo->value();


		foreach(const QString& type, ExampleInversionPlugin::i3CubeTypes)
		{
			if (cubesData.contains(type) == false || cubesData.value(type).isEmpty())
			{
				progressReporter->logMessage("Cube wasn't selected!");
				return false;
			}
		}
		InversionCubeDataInterface* cube1 = cubesData.value(i3CubeTypes.at(0)).at(0);

		FotiadiConnector connector(resultData, cube1, maxShift, progressReporter, appSettings);
		progressReporter->setProgressRange(0, 1000);
		return connector.DrawS3DCubeWithReflectionHorizonsOneToAllTraces();
	}
	else
		return false;
}

QString ExampleInversionPlugin::getLastError() const
{
	return m_error;
}
