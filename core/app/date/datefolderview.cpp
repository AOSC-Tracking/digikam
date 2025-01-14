/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-27
 * Description : a folder view for date albums.
 *
 * SPDX-FileCopyrightText: 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2014      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "datefolderview.h"

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"
#include "album.h"
#include "coredb.h"
#include "applicationsettings.h"
#include "datetreeview.h"
#include "monthwidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN DateFolderView::Private
{
public:

    Private() = default;

    bool          active        = false;

    DateTreeView* dateTreeView  = nullptr;
    MonthWidget*  monthview     = nullptr;
};

DateFolderView::DateFolderView(QWidget* const parent, DateAlbumModel* const dateAlbumModel)
    : DVBox            (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("DateFolderView"));

    d->dateTreeView = new DateTreeView(this);
    d->dateTreeView->setAlbumModel(dateAlbumModel);
    d->dateTreeView->setAlbumManagerCurrentAlbum(true);
    d->monthview    = new MonthWidget(this);

    connect(d->dateTreeView, SIGNAL(currentAlbumChanged(Album*)),
            this, SLOT(slotSelectionChanged(Album*)));

    // Loading of DAlbums may take longer that setting up the gui. Therefore
    // the first call to setActive may not set the current album in the album
    // manager as it is not yet loaded. To achieve this, we wait for loading
    // DAlbums and set the active album in the album manager if this tab is
    // active

    connect(AlbumManager::instance(), SIGNAL(signalAllDAlbumsLoaded()),
            this, SLOT(slotAllAlbumsLoaded()));
}

DateFolderView::~DateFolderView()
{
    saveState();
}

void DateFolderView::setItemModel(ItemFilterModel* const model)
{
    d->monthview->setItemModel(model);
}

void DateFolderView::setActive(const bool val)
{
    if (d->active == val)
    {
        return;
    }

    d->active = val;

    if (d->active)
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>()
                                                   << d->dateTreeView->currentAlbum());
        slotSelectionChanged(d->dateTreeView->currentAlbum());
    }
    else
    {
        d->monthview->setActive(false);
    }
}

void DateFolderView::slotSelectionChanged(Album* selectedAlbum)
{
    if (!d->active)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Not active, returning without action";
        return;
    }

    d->monthview->setActive(false);

    DAlbum* const dalbum = dynamic_cast<DAlbum*> (selectedAlbum);

    if (!dalbum)
    {
        return;
    }

    if (dalbum->range() == DAlbum::Month)
    {
        QDate date = dalbum->date();
        d->monthview->setActive(true);
        d->monthview->setYearMonth(date.year(), date.month());
    }

    if (d->active)
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << dalbum);
    }
}

void DateFolderView::slotAllAlbumsLoaded()
{
    if (d->active)
    {
        QList<Album*> albums({d->dateTreeView->currentAlbum()});

        if (AlbumManager::instance()->currentAlbums() != albums)
        {
            AlbumManager::instance()->setCurrentAlbums(albums);
            slotSelectionChanged(d->dateTreeView->currentAlbum());
        }
    }

    // Workaround for bug 447874

    d->dateTreeView->setSortingEnabled(true);
}

void DateFolderView::setConfigGroup(const KConfigGroup& group)
{
    StateSavingObject::setConfigGroup(group);
    d->dateTreeView->setConfigGroup(group);
}

void DateFolderView::doLoadState()
{
    d->dateTreeView->loadState();
}

void DateFolderView::doSaveState()
{
    d->dateTreeView->saveState();
}

void DateFolderView::gotoDate(const QDate& dt)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Going to date " << dt;

    QModelIndex dateIndex = d->dateTreeView->albumModel()->monthIndexForDate(dt);

    if (!dateIndex.isValid())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot find an album for date " << dt;

        return;
    }

    DAlbum* const dateAlbum = d->dateTreeView->albumModel()->albumForIndex(dateIndex);

    if (!dateAlbum)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Could not retrieve an album for index " << dateIndex;

        return;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Got date album " << dateAlbum;

    d->dateTreeView->setCurrentAlbums(QList<Album*>() << dateAlbum);

}

void DateFolderView::changeAlbumFromHistory(DAlbum* const album)
{
    d->dateTreeView->setCurrentAlbums(QList<Album*>() << album);
}

AlbumPointer<DAlbum> DateFolderView::currentAlbum() const
{
    return AlbumPointer<DAlbum>(d->dateTreeView->currentAlbum());
}

} // namespace Digikam

#include "moc_datefolderview.cpp"
