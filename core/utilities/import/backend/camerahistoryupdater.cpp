/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-10-16
 * Description : history updater thread for importui
 *
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "camerahistoryupdater.h"

// Qt includes

#include <QList>
#include <QMutex>
#include <QVariant>
#include <QWaitCondition>
#include <QWidget>

// Local includes

#include "digikam_debug.h"
#include "coredbdownloadhistory.h"

namespace Digikam
{

class Q_DECL_HIDDEN CameraHistoryUpdater::Private
{

public:

    typedef QList<CHUpdateItem> CHUpdateItemsList;

public:

    Private() = default;

public:

    bool              canceled = false;
    bool              running  = false;

    QMutex            mutex;
    QWaitCondition    condVar;
    CHUpdateItemsList updateItems;
};

// --------------------------------------------------------

CameraHistoryUpdater::CameraHistoryUpdater(QWidget* const parent)
    : QThread(parent),
      d      (new Private)
{
    qRegisterMetaType<CHUpdateItemMap>("CHUpdateItemMap");
}

CameraHistoryUpdater::~CameraHistoryUpdater()
{
    // clear updateItems, stop processing

    slotCancel();

    // stop thread
    {
        QMutexLocker lock(&d->mutex);
        d->running = false;
        d->condVar.wakeAll();
    }

    wait();

    delete d;
}

void CameraHistoryUpdater::slotCancel()
{
    d->canceled = true;
    QMutexLocker lock(&d->mutex);
    d->updateItems.clear();
}

void CameraHistoryUpdater::run()
{
    while (d->running)
    {
        CHUpdateItem item;

        QMutexLocker lock(&d->mutex);

        if (!d->updateItems.isEmpty())
        {
            item = d->updateItems.takeFirst();
            sendBusy(true);
            proccessMap(item.first, item.second);
        }
        else
        {
            sendBusy(false);
            d->condVar.wait(&d->mutex);
            continue;
        }
    }

    sendBusy(false);
}

void CameraHistoryUpdater::sendBusy(bool val)
{
    Q_EMIT signalBusy(val);
}

void CameraHistoryUpdater::addItems(const QByteArray& id, CHUpdateItemMap& map)
{
    if (map.isEmpty())
    {
        return;
    }

    qCDebug(DIGIKAM_IMPORTUI_LOG) << "Check download state from DB for " << map.count() << " items";

    QMutexLocker lock(&d->mutex);
    d->running  = true;
    d->canceled = false;
    d->updateItems << CHUpdateItem(id, map);

    if (!isRunning())
    {
        start(LowPriority);
    }

    d->condVar.wakeAll();
}

void CameraHistoryUpdater::proccessMap(const QByteArray& id, CHUpdateItemMap& map)
{
    CHUpdateItemMap::iterator it = map.begin();

    do
    {
        // We query database to check if (*it).have been already downloaded from camera.

        switch (CoreDbDownloadHistory::status(QString::fromUtf8(id), (*it).name, (*it).size, (*it).ctime))
        {
            case CoreDbDownloadHistory::NotDownloaded:
            {
                (*it).downloaded = CamItemInfo::NewPicture;
                break;
            }

            case CoreDbDownloadHistory::Downloaded:
            {
                (*it).downloaded = CamItemInfo::DownloadedYes;
                break;
            }

            default: // CoreDbDownloadHistory::StatusUnknown
            {
                (*it).downloaded = CamItemInfo::DownloadUnknown;
                break;
            }
        }

        ++it;
    }
    while ((it != map.end()) && !d->canceled);

    Q_EMIT signalHistoryMap(map);
}

} // namespace Digikam

#include "moc_camerahistoryupdater.cpp"
