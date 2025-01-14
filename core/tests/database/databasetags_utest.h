/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-12-13
 * Description : test cases for tags tree manipulation in database
 *
 * SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QHash>
#include <QAbstractItemModel>

namespace Digikam
{
class PAlbum;
class TAlbum;
class AlbumModel;
}

class DatabaseTagsTest: public QObject
{
    Q_OBJECT

public:

    explicit DatabaseTagsTest(QObject* const parent = nullptr);
    ~DatabaseTagsTest() override;

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();
/*
    void testPAlbumModel();
    void testDisablePAlbumCount();
    void testDAlbumModel();
    void testDAlbumCount();
    void testDAlbumContainsAlbums();
    void testDAlbumSorting();
    void testTAlbumModel();
    void testSAlbumModel();
    void testStartAlbumModel();

    void deletePAlbum(Digikam::PAlbum* album);

    void setLastPAlbumCountHash(const QHash<int, int>& hash);

    // slots for ensuring signal order while scanning albums
    void slotStartModelRowsInserted(const QModelIndex& parent, int start, int end);
    void slotStartModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
*/
private:

//    void ensureItemCounts();

private:

    QString              dbPath;
    QString              tempSuffix;

    Digikam::PAlbum*     palbumRoot0;
    Digikam::PAlbum*     palbumRoot1;
    Digikam::PAlbum*     palbumRoot2;
    Digikam::PAlbum*     palbumChild0Root0;
    Digikam::PAlbum*     palbumChild1Root0;
    Digikam::PAlbum*     palbumChild2Root0;
    Digikam::PAlbum*     palbumChild0Root1;

    Digikam::TAlbum*     rootTag;
    Digikam::TAlbum*     talbumRoot0;
    Digikam::TAlbum*     talbumRoot1;
    Digikam::TAlbum*     talbumChild0Root0;
    Digikam::TAlbum*     talbumChild1Root0;
    Digikam::TAlbum*     talbumChild0Child1Root0;
    Digikam::TAlbum*     talbumChild0Root1;

    QList<int>           addedIds;

    QString              filesPath;
};
