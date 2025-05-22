#ifndef EXAMPLEINVERSIONPLUGIN_H
#define EXAMPLEINVERSIONPLUGIN_H

#include <inversionplugininterfaces.h>

/**
 * Описания переопределяемых методов смотреть в inversionplugininterfaces.h
 */

namespace Ui
{
	// лучше использовать какие-нибудь более адекватные имена
	class Ex1UI;
	class Ex2UI;
}

class ExampleInversionPlugin : public QObject, public InversionPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID InversionPluginInterface_iid FILE "exmpleinversionplugin.json")
	Q_INTERFACES(InversionPluginInterface)

public:
	ExampleInversionPlugin(QObject *parent = nullptr);
	~ExampleInversionPlugin();

	QStringList inversionTypes() const override;
	QStringList cubeTypes(const QString &shortName) const override;
	QString baseCubeType(const QString &shortName) const override;
	void initInversionSettingsWidget(const QString &shortName, QWidget *widget) override;
	bool doInversion(const QString &shortName, QWidget *uiWidget
					 ,PluginProgressReporterInterface *progressReporter
					 ,const QHash<QString, QVector<InversionCubeDataInterface *> > &cubesData
					 ,PluginTabletDataInterface *tabletData
					 ,InversionResultInterface *resultData
					) override;
	QString getLastError() const override;

private:
	QString m_error;

	// для удобства типы кубов определяю в статических переменных
	static const QStringList i1CubeTypes;
	static const QStringList i2CubeTypes;
	static const QStringList i3CubeTypes;
};

#endif // EXAMPLEINVERSIONPLUGIN_H
