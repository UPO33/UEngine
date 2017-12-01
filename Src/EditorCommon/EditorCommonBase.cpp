#include "EditorCommonBase.h"
#include "../Core/Meta.h"
#include "Project.h"

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/qmenu.h>
#include <QtCore/qmap.h>


namespace UEditor
{

	UEDITORCOMMON_API StringStreamOut& operator<<(StringStreamOut& stream, const QString& str)
	{
		stream << str.toStdString();
		return stream;
	}



	//////////////////////////////////////////////////////////////////////////
	void UShowInExplorer(const QString& pathIn)
	{
#if defined(Q_OS_WIN)
		QString param;
		if (!QFileInfo(pathIn).isDir())
			param = QLatin1String("/select,");
		param += QDir::toNativeSeparators(pathIn);
		QString command = QString("explorer.exe") + " " + param;
		QProcess::startDetached(command);
#elif defined(Q_OS_MAC)
		QStringList scriptArgs;
		scriptArgs << QLatin1String("-e")
			<< QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
			.arg(pathIn);
		QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
		scriptArgs.clear();
		scriptArgs << QLatin1String("-e")
			<< QLatin1String("tell application \"Finder\" to activate");
		QProcess::execute("/usr/bin/osascript", scriptArgs);
#else
		// we cannot select a file here, because no file browser really supports it...
		const QFileInfo fileInfo(pathIn);
		const QString folder = fileInfo.absoluteFilePath();
		const QString app = Utils::UnixUtils::fileBrowser(Core::ICore::instance()->settings());
		QProcess browserProc;
		const QString browserArgs = Utils::UnixUtils::substituteFileBrowserParameters(app, folder);
		if (debug)
			qDebug() << browserArgs;
		bool success = browserProc.startDetached(browserArgs);
		const QString error = QString::fromLocal8Bit(browserProc.readAllStandardError());
		success = success && error.isEmpty();
		if (!success)
			showGraphicalShellError(parent, app, error);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	QMenu* UMenuFindMenu(const QMenu* pMenu, const QString& nameToFind)
	{
		QList<QMenu*> lst = pMenu->findChildren<QMenu*>();
		for (QMenu* iter : lst)
		{
			if (iter && iter->title() == nameToFind)
				return iter;
		}
		return nullptr;
	}

};