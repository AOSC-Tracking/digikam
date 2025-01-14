/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-13
 * Description : tabbed queue items list.
 *
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "queuepool.h"

// Qt includes

#include <QApplication>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dmessagebox.h"
#include "applicationsettings.h"
#include "iccsettings.h"
#include "metaenginesettings.h"
#include "ddragobjects.h"
#include "queuelist.h"
#include "workflowmanager.h"
#include "workflowdlg.h"
#include "queuesettings.h"
#include "loadingcacheinterface.h"

namespace Digikam
{

QueuePoolBar::QueuePoolBar(QWidget* const parent)
    : QTabBar(parent)
{
  setAcceptDrops(true);
  setMouseTracking(true);
}

void QueuePoolBar::dragEnterEvent(QDragEnterEvent* e)
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    int tab = tabAt(e->position().toPoint());

#else

    int tab = tabAt(e->pos());

#endif

    if (tab != -1)
    {
        bool accept = false;

        // The receivers of the testCanDecode() signal has to adjust 'accept' accordingly.

        Q_EMIT signalTestCanDecode(e, accept);

        e->setAccepted(accept);

        return;
    }

    QTabBar::dragEnterEvent(e);
}

void QueuePoolBar::dragMoveEvent(QDragMoveEvent* e)
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    int tab = tabAt(e->position().toPoint());

#else

    int tab = tabAt(e->pos());

#endif

    if (tab != -1)
    {
        bool accept = false;

        // The receivers of the testCanDecode() signal has to adjust 'accept' accordingly.

        Q_EMIT signalTestCanDecode(e, accept);

        e->setAccepted(accept);

        return;
    }

    QTabBar::dragMoveEvent(e);
}

// --------------------------------------------------------------------------------------------

QueuePool::QueuePool(QWidget* const parent)
    : QTabWidget(parent)
{
    setTabBar(new QueuePoolBar(this));
    setTabsClosable(false);
    setAcceptDrops(true);
    slotAddQueue();

    connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(slotQueueSelected(int)));

    connect(this, SIGNAL(tabCloseRequested(int)),
            this, SLOT(slotCloseQueueRequest(int)));

    connect(tabBar(), SIGNAL(signalTestCanDecode(const QDragMoveEvent*,bool&)),
            this, SLOT(slotTestCanDecode(const QDragMoveEvent*,bool&)));

    // -- FileWatch connections ------------------------------

    LoadingCacheInterface::connectToSignalFileChanged(this, SLOT(slotFileChanged(QString)));
}

void QueuePool::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete)
    {
        slotRemoveSelectedItems();
    }
    else
    {
        QTabWidget::keyPressEvent(event);
    }
}

void QueuePool::setBusy(bool b)
{
    tabBar()->setEnabled(!b);

    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = dynamic_cast<QueueListView*>(widget(i));

        if (queue)
        {
            queue->viewport()->setEnabled(!b);
        }
    }
}

QueueListView* QueuePool::currentQueue() const
{
    return (dynamic_cast<QueueListView*>(currentWidget()));
}

QString QueuePool::currentTitle() const
{
    return queueTitle(currentIndex());
}

QueueListView* QueuePool::findQueueByItemId(qlonglong id) const
{
    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = dynamic_cast<QueueListView*>(widget(i));

        if (queue && queue->findItemById(id))
        {
            return queue;
        }
    }

    return nullptr;
}

void QueuePool::setItemBusy(qlonglong id)
{
    QueueListView* const queue = findQueueByItemId(id);

    if (queue)
    {
        queue->setItemBusy(id);
    }
}

QueueListView* QueuePool::findQueueByIndex(int index) const
{
    return (dynamic_cast<QueueListView*>(widget(index)));
}

QMap<int, QString> QueuePool::queuesMap() const
{
    QMap<int, QString> map;

    for (int i = 0 ; i < count() ; ++i)
    {
        map.insert(i, queueTitle(i));
    }

    return map;
}

QString QueuePool::queueTitle(int index) const
{
    // NOTE: clean up tab title. With QTabWidget, it sound like mistake is added, as '&' and space.
    // NOTE update, & is an usability helper to allow keyboard access -teemu

    return (tabText(index).remove(QLatin1Char('&')).remove(QLatin1Char(' ')));
}

void QueuePool::slotAddQueue()
{
    QueueListView* const queue = new QueueListView(this);

    if (!queue)
    {
        return;
    }

    int index = addTab(queue, QIcon::fromTheme(QLatin1String("run-build")), QString::fromUtf8("#%1").arg(count() + 1));

    connect(queue, SIGNAL(signalQueueContentsChanged()),
            this, SIGNAL(signalQueueContentsChanged()));

    connect(queue, SIGNAL(itemSelectionChanged()),
            this, SIGNAL(signalItemSelectionChanged()));

    Q_EMIT signalQueuePoolChanged();

    setCurrentIndex(index);
}

QueuePoolItemsList QueuePool::itemsList(int index, int type) const
{
    QueuePoolItemsList qpool;

    QueueListView* const queue = dynamic_cast<QueueListView*>(widget(index));

    if (queue)
    {
        ItemInfoList list = queue->itemsList((QueueListView::ItemListType)type);

        for (ItemInfoList::const_iterator it = list.constBegin() ; it != list.constEnd() ; ++it)
        {
            ItemInfo info = *it;
            ItemInfoSet set(index, info);
            qpool.append(set);
        }
    }

    return qpool;
}

int QueuePool::totalPendingItems() const
{
    int items = 0;

    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = dynamic_cast<QueueListView*>(widget(i));

        if (queue)
        {
            items += queue->pendingItemsCount();
        }
    }

    return items;
}

int QueuePool::totalPendingTasks() const
{
    int tasks = 0;

    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = dynamic_cast<QueueListView*>(widget(i));

        if (queue)
        {
            tasks += queue->pendingTasksCount();
        }
    }

    return tasks;
}

void QueuePool::slotRemoveCurrentQueue()
{
    QueueListView* const queue = currentQueue();

    if (!queue)
    {
        return;
    }

    removeTab(indexOf(queue));

    if (count() == 0)
    {
        slotAddQueue();
    }
    else
    {
        for (int i = 0; i < count(); ++i)
        {
            setTabText(i, QString::fromUtf8("#%1").arg(i + 1));
        }
    }

    Q_EMIT signalQueuePoolChanged();
}

bool QueuePool::saveWorkflow() const
{
    QueueListView* const queue = currentQueue();

    if (queue)
    {
        WorkflowManager* const mngr = WorkflowManager::instance();
        Workflow wf;
        wf.qSettings = queue->settings();
        wf.aTools    = queue->assignedTools().m_toolsList;

        if (WorkflowDlg::createNew(wf))
        {
            mngr->insert(wf);
            mngr->save();

            return true;
        }
    }

    return false;
}

void QueuePool::slotClearList()
{
    QueueListView* const queue = currentQueue();

    if (queue)
    {
        queue->slotClearList();
    }
}

void QueuePool::slotRemoveSelectedItems()
{
    QueueListView* const queue = currentQueue();

    if (queue)
    {
        queue->slotRemoveSelectedItems();
    }
}

void QueuePool::slotRemoveItemsDone()
{
    QueueListView* const queue = currentQueue();

    if (queue)
    {
        queue->slotRemoveItemsDone();
    }
}

void QueuePool::slotAddItems(const ItemInfoList& list, int queueId)
{
    QueueListView* const queue = findQueueByIndex(queueId);

    if (queue)
    {
        queue->slotAddItems(list);
    }
}

void QueuePool::slotAssignedToolsChanged(const AssignedBatchTools& tools4Item)
{
    QueueListView* const queue = currentQueue();

    if (queue)
    {
        queue->slotAssignedToolsChanged(tools4Item);
    }
}

void QueuePool::slotQueueSelected(int index)
{
    QueueListView* const queue = dynamic_cast<QueueListView*>(widget(index));

    if (queue)
    {
        Q_EMIT signalItemSelectionChanged();
        Q_EMIT signalQueueSelected(index, queue->settings(), queue->assignedTools());
    }
}

void QueuePool::slotCloseQueueRequest(int index)
{
    removeTab(index);

    if (count() == 0)
    {
        slotAddQueue();
    }

    Q_EMIT signalQueuePoolChanged();
}

void QueuePool::removeTab(int index)
{
    QueueListView* const queue = dynamic_cast<QueueListView*>(widget(index));

    if (!queue)
    {
        return;
    }

    int count = queue->pendingItemsCount();

    if (count > 0)
    {
        int ret = QMessageBox::question(this, qApp->applicationName(),
                                        i18np("There is still 1 unprocessed item in \"%2\".\nDo you want to close this queue?",
                                              "There are still %1 unprocessed items in \"%2\".\nDo you want to close this queue?",
                                              count, queueTitle(index)),
                                        QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::No)
        {
            return;
        }
    }

    QTabWidget::removeTab(index);
}

void QueuePool::slotTestCanDecode(const QDragMoveEvent* e, bool& accept)
{
    int              albumID;
    QList<int>       albumIDs;
    QList<qlonglong> imageIDs;
    QList<QUrl>      urls;

    if (DItemDrag::decode(e->mimeData(), urls, albumIDs, imageIDs) ||
        DAlbumDrag::decode(e->mimeData(), urls, albumID)           ||
        DTagListDrag::canDecode(e->mimeData()))
    {
        accept = true;

        return;
    }

    accept = false;
}

void QueuePool::slotSettingsChanged(const QueueSettings& settings)
{
    QueueListView* const queue = currentQueue();

    if (queue)
    {
        queue->setSettings(settings);
    }
}

bool QueuePool::customRenamingRulesAreValid() const
{
    QStringList list;

    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = findQueueByIndex(i);

        if (queue)
        {
            if (queue->settings().renamingRule == QueueSettings::CUSTOMIZE &&
                queue->settings().renamingParser.isEmpty())
            {
                list.append(queueTitle(i));
            }
        }
    }

    if (!list.isEmpty())
    {
        DMessageBox::showInformationList(QMessageBox::Critical,
                                         qApp->activeWindow(),
                                         qApp->applicationName(),
                                         i18n("Custom renaming rules are invalid for Queues listed below. "
                                              "Please fix them."),
                                         list);
        return false;
    }

    return true;
}

bool QueuePool::assignedBatchToolsListsAreValid() const
{
    QStringList list;

    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = findQueueByIndex(i);

        if (queue)
        {
            if (queue->assignedTools().m_toolsList.isEmpty())
            {
                list.append(queueTitle(i));
            }
        }
    }

    if (!list.isEmpty())
    {
        DMessageBox::showInformationList(QMessageBox::Critical,
                                         qApp->activeWindow(),
                                         qApp->applicationName(),
                                         i18n("Assigned batch tools list is empty for Queues listed below. "
                                              "Please assign tools."),
                                         list);
        return false;
    }

    return true;
}

void QueuePool::slotFileChanged(const QString& filePath)
{
    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = dynamic_cast<QueueListView*>(widget(i));

        if (queue)
        {
            queue->reloadThumbs(QUrl::fromLocalFile(filePath));
        }
    }
}

void QueuePool::applySettings()
{
    for (int i = 0 ; i < count() ; ++i)
    {
        QueueListView* const queue = dynamic_cast<QueueListView*>(widget(i));

        if (queue)
        {
            // Show/hide tool-tips settings.

            queue->setEnableToolTips(ApplicationSettings::instance()->getShowToolTips());

            // Reset Exif Orientation settings.

            QueueSettings prm = queue->settings();
            prm.exifSetOrientation = MetaEngineSettings::instance()->settings().exifRotate;

            // Apply Color Management rules to RAW images decoding settings

            // If digiKam Color Management is enable, no need to correct color of decoded RAW image,
            // else, sRGB color workspace will be used.

            ICCSettingsContainer ICCSettings = IccSettings::instance()->settings();

            if (ICCSettings.enableCM)
            {
                if ((ICCSettings.defaultUncalibratedBehavior & ICCSettingsContainer::AskUser) ||
                    (ICCSettings.defaultUncalibratedBehavior & ICCSettingsContainer::AutomaticColors))
                {
                    prm.rawDecodingSettings.outputColorSpace = DRawDecoderSettings::CUSTOMOUTPUTCS;
                    prm.rawDecodingSettings.outputProfile    = ICCSettings.workspaceProfile;
                }
                else
                {
                    prm.rawDecodingSettings.outputColorSpace = DRawDecoderSettings::RAWCOLOR;
                }
            }
            else
            {
                prm.rawDecodingSettings.outputColorSpace = DRawDecoderSettings::SRGB;
            }

            queue->setSettings(prm);
        }
    }
}

} // namespace Digikam

#include "moc_queuepool.cpp"
