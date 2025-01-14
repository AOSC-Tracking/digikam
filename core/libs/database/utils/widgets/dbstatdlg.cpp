/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-05-28
 * Description : database statistics dialog
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dbstatdlg.h"

// Qt includes

#include <QStringList>
#include <QString>
#include <QFont>
#include <QTreeWidget>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_globals.h"
#include "daboutdata.h"
#include "coredb.h"
#include "applicationsettings.h"
#include "coredbaccess.h"

namespace Digikam
{

DBStatDlg::DBStatDlg(QWidget* const parent)
    : InfoDlg(parent)
{
    qApp->setOverrideCursor(Qt::WaitCursor);

    setWindowTitle(i18nc("@title:window", "Database Statistics"));
    listView()->setHeaderLabels(QStringList() << i18n("Format") << i18n("Count"));

    // get image format statistics

    int totalImages = generateItemsList(DatabaseItem::Image, i18n("Images"));
    int totalVideos = generateItemsList(DatabaseItem::Video, i18n("Videos"));
    int totalAudio  = generateItemsList(DatabaseItem::Audio, i18n("Audio"));
    int total       = totalImages + totalVideos + totalAudio;

    // --------------------------------------------------------

    // To see total count of items at end of list.

    new QTreeWidgetItem(listView(), QStringList() << i18n("Total Items") << QString::number(total));

    // get album statistics

    int albums   = CoreDbAccess().db()->scanAlbums().count();
    new QTreeWidgetItem(listView(), QStringList() << i18n("Albums") << QString::number(albums));

    // get tags statistics

    int tags     = CoreDbAccess().db()->scanTags().count();
    new QTreeWidgetItem(listView(), QStringList() << i18n("Tags") << QString::number(tags));

    // Database Backend information

    new QTreeWidgetItem(listView(), QStringList());
    DbEngineParameters prm = ApplicationSettings::instance()->getDbEngineParameters();
    QString dbBe           = prm.databaseType;
    new QTreeWidgetItem(listView(), QStringList() << i18n("Database backend") << dbBe);

    if (dbBe == QLatin1String("QSQLITE"))
    {
        new QTreeWidgetItem(listView(), QStringList() << i18n("Database Path") << prm.getCoreDatabaseNameOrDir());
    }
    else
    {
        if (prm.internalServer)
        {
            new QTreeWidgetItem(listView(), QStringList() << i18n("Database internal server")      << i18nc("@info: database server", "Yes"));
            new QTreeWidgetItem(listView(), QStringList() << i18n("Database internal server Path") << prm.internalServerDBPath);
        }
        else
        {
            new QTreeWidgetItem(listView(), QStringList() << i18n("Host Name")       << prm.hostName);
            new QTreeWidgetItem(listView(), QStringList() << i18n("Host Port")       << QString::number(prm.port));
            new QTreeWidgetItem(listView(), QStringList() << i18n("User account")    << prm.userName);
            new QTreeWidgetItem(listView(), QStringList() << i18n("Connect options") << prm.connectOptions);
            new QTreeWidgetItem(listView(), QStringList() << i18n("Core Db name")    << prm.databaseNameCore);
            new QTreeWidgetItem(listView(), QStringList() << i18n("Thumbs Db name")  << prm.databaseNameThumbnails);
            new QTreeWidgetItem(listView(), QStringList() << i18n("Face Db name")    << prm.databaseNameFace);
        }
    }

    QString dbLocale = CoreDbAccess().db()->getSetting(QLatin1String("Locale"));
    new QTreeWidgetItem(listView(), QStringList() << i18n("Database locale") << dbLocale);

    qApp->restoreOverrideCursor();
}

int DBStatDlg::generateItemsList(DatabaseItem::Category category, const QString& title)
{
    // get image format statistics

    QMap<QString, int> stat = CoreDbAccess().db()->getFormatStatistics(category);

    // do not add items if the map is empty

    if (stat.isEmpty())
    {
        return 0;
    }

    int total = 0;
    QMap<QString, QString> map;

    for (QMap<QString, int>::const_iterator it = stat.constBegin() ; it != stat.constEnd() ; ++it)
    {
        total += it.value();
        map.insert(it.key(), QString::number(it.value()));
    }

    // --------------------------------------------------------

    QTreeWidgetItem* ti = new QTreeWidgetItem(listView(), QStringList() << title << QString());
    QFont ft            = ti->font(0);
    ft.setBold(true);
    ti->setFont(0, ft);
    ti->setFont(1, ft);

    setInfoMap(map);

    ti = new QTreeWidgetItem(listView(), QStringList()
                                         << i18nc("@info: total number of items", "total")
                                         << QString::number(total));
    ti->setFont(0, ft);
    ti->setFont(1, ft);

    // Add space.

    new QTreeWidgetItem(listView(), QStringList());

    return total;
}

void DBStatDlg::slotHelp()
{
    openOnlineDocumentation(QLatin1String("setup_application"),
                            QLatin1String("database_settings"),
                            QLatin1String("database_stats"));
}

} // namespace Digikam

#include "moc_dbstatdlg.cpp"
