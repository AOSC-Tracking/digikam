/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-09-19
 * Description : Scanning a single item.
 *
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QFileInfo>

// Local includes

#include "dimg.h"
#include "iteminfo.h"
#include "dmetadata.h"
#include "coredbalbuminfo.h"
#include "coredbinfocontainers.h"

namespace Digikam
{

class DIGIKAM_DATABASE_EXPORT ItemScanner
{

public:

    enum ScanMode
    {
        NewScan,
        ModifiedScan,
        Rescan,
        CleanScan
    };

public:

    /**
     * Construct an ItemScanner object from an existing QFileInfo
     * and ItemScanInfo object.
     * This constructor shall be used with fileModified() or fullScan().
     */
    ItemScanner(const QFileInfo& info, const ItemScanInfo& Iteminfo);

    /**
     * Construct an ItemScanner from an existing QFileInfo object.
     * Use this constructor if you intend to call newFile().
     */
    explicit ItemScanner(const QFileInfo& info);

    /**
     * Construct an ItemScanner for an image in the database.
     * File info, Scan info and the category will be retrieved from the database.
     */
    explicit ItemScanner(qlonglong imageid);

    ~ItemScanner();

    /**
     * Inform the scanner about the category of the file.
     * Required at least for newFile() calls, recommended for calls with the
     * first constructor above as well.
     */
    void setCategory(DatabaseItem::Category category);

    /**
     * Provides access to the information retrieved by scanning.
     * The validity depends on the previously executed scan.
     */
    const ItemScanInfo& itemScanInfo() const;

    /**
     * Loads data from disk (metadata, image file properties).
     * This method is called from any of the main entry points above.
     * You can call it before if you want to control the time when it is executed.
     * Calling it a second time with data already loaded will do nothing.
     */
    void loadFromDisk();

    /**
     * Helper method to translate enum values to user presentable strings
     */
    static QString formatToString(const QString& format);

private:

    static bool hasValidField(const QVariantList& list);
    static bool lessThanForIdentity(const ItemScanInfo& a, const ItemScanInfo& b);

    // -----------------------------------------------------------------------------

    /** @name Operations with Database
     */

    //@{

public:

    /**
     * Call this when you want ItemScanner to add a new file to the database
     * and read all information into the database.
     */
    void newFile(int albumId);

    /**
     * Call this when you want ItemScanner to add a new file to the database
     * and read all information into the database. This variant will not use
     * the unique hash to establish identify with an existing entry, but
     * read all information newly from the file.
     */
    void newFileFullScan(int albumId);

    /**
     * Call this to take an existing image in the database, but re-read
     * all information from the file into the database, possibly overwriting
     * information there.
     */
    void rescan();

    /**
     * This is the same as rescan() but the database metadata
     * will be cleaned up if the corresponding metadata
     * write option is enabled.
     */
    void cleanScan();

    /**
     * Commits the scanned information to the database.
     * You must call this after scanning was done for any changes to take effect.
     * Only this method will perform write operations to the database.
     */
    void commit();

    /**
     * Returns the image id of the scanned file, if (yet) available.
     */
    qlonglong id() const;

    /**
     * Similar to newFile.
     * Call this when you want ItemScanner to add a new file to the database
     * which is a copy of another file, copying attributes from the src
     * and rescanning other attributes as appropriate.
     * Give the id of the album of the new file, and the id of the src file.
     */
    void copiedFrom(int albumId, qlonglong srcId);

    /**
     * Sort a list of infos by proximity to the given subject.
     * Infos are near if they are e.g. in the same album.
     * They are not near if they are e.g. in different collections.
     */
    static void sortByProximity(QList<ItemInfo>& infos, const ItemInfo& subject);

protected:

    bool copyFromSource(qlonglong src);
    void commitCopyImageAttributes();
    void cleanDatabaseMetadata();

    void prepareAddImage(int albumId);
    bool commitAddImage();

    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations on File Metadata
     */

    //@{

public:

    /**
     * Call this when you have detected that a file in the database has been
     * modified on disk. Only two groups of fields will be updated in the database:
     * - filesystem specific properties (those that signaled you that the file has been modified
     *   because their state on disk differed from the state in the database)
     * - image specific properties, for which a difference in the database independent from
     *   the actual file does not make sense (width/height, bit depth, color model)
     */
    void fileModified();

    /**
     * Returns File-metadata container with user-presentable information.
     * These methods provide the reverse service: Not writing into the db, but reading from the db.
     */
    static void fillCommonContainer(qlonglong imageid, ImageCommonContainer* const container);

    /**
     * Returns a suitable creation date from file system information.
     * Use this as a fallback if metadata is not available.
     */
    static QDateTime creationDateFromFilesystem(const QFileInfo& info);

protected:

    void prepareUpdateImage();
    void commitUpdateImage();

    bool scanFromIdenticalFile();
    void scanFile(ScanMode mode);

    void scanItemInformation();
    void commitItemInformation();

    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations on Photo Metadata
     */

    //@{

public:

    /**
     * Helper method to return official property name by which
     * IPTC core properties are stored in the database (ItemCopyright and ImageProperties table).
     * Allowed arguments: All MetadataInfo::Fields starting with "IptcCore..."
     */
    static QString iptcCorePropertyName(MetadataInfo::Field field);

    static MetadataFields allImageMetadataFields();

protected:

    QString detectImageFormat() const;

    void scanImageMetadata();
    void commitImageMetadata();
    void scanItemPosition();
    void commitItemPosition();
    void scanItemComments();
    void commitItemComments();
    void scanItemCopyright();
    void commitItemCopyright();
    void scanIPTCCore();
    void commitIPTCCore();
    void scanTags();
    void commitTags();
    void scanFaces();
    void commitFaces();

    bool checkRatingFromMetadata(const QVariant& ratingFromMetadata) const;
    void checkCreationDateFromMetadata(QVariant& dateFromMetadata)   const;

    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations on Video Metadata
     */

    //@{

public:

    /**
     * Returns Video container with user-presentable information.
     * These methods provide the reverse service: Not writing into the db, but reading from the db.
     */
    static void fillVideoMetadataContainer(qlonglong imageid, VideoMetadataContainer* const container);

protected:

    void scanVideoInformation();
    void scanVideoMetadata();
    void commitVideoMetadata();
    QString detectVideoFormat() const;
    QString detectAudioFormat() const;
    static MetadataFields allVideoMetadataFields();

    //@}

    // -----------------------------------------------------------------------------

    /** @name Operations on History Metadata
     */

    //@{

public:

    /**
     * Returns true if this file has been marked as needing history resolution at a later stage
     */
    bool hasHistoryToResolve() const;

    //@{
    /**
     * Resolves the image history of the image id by filling the ImageRelations table
     * for all contained referred images.
     * If needTaggingIds is given, all ids marked for needing tagging of the history graph are added.
     */
    static bool resolveImageHistory(qlonglong id, QList<qlonglong>* needTaggingIds = nullptr);
    static bool resolveImageHistory(qlonglong imageId, const QString& historyXml, QList<qlonglong>* needTaggingIds = nullptr);
    //@}

    /**
     * Takes the history graph reachable from the given image, and assigns
     * versioning tags to all entries based on history image types and graph structure
     */
    static void tagItemHistoryGraph(qlonglong id);

    /**
     * All referred images of the given history will be resolved.
     * In the returned history, the actions are the same, while each
     * referred image actually exists in the collection
     * (if mustBeAvailable is true, it is even in a currently available collection).
     * That means the number of referred images may be less or greater than initially.
     * Note that this history may have peculiar properties, like multiple Original or Current entries
     * (if the source entry resolves to multiple collection images), so this history
     * is only for internal use, not for storage.
     */
    static DImageHistory resolvedImageHistory(const DImageHistory& history, bool mustBeAvailable = false);

    /**
     * Determines if the two ids refer to the same image.
     * Does not check if such a referred image really exists.
     */
    static bool sameReferredImage(const HistoryImageId& id1, const HistoryImageId& id2);

    /**
     * Returns all image ids fulfilling the given image id.
     */
    static QList<qlonglong> resolveHistoryImageId(const HistoryImageId& historyId);

protected:

    void scanImageHistory();
    void commitImageHistory();
    void scanImageHistoryIfModified();

    QString uniqueHash() const;

    //@}

public:

    /**
     * @brief scanBalooInfo - retrieve tags, comments and rating from Baloo Desktop service.
     */
    void scanBalooInfo();

private:

    // Disable
    ItemScanner(const ItemScanner&)            = delete;
    ItemScanner& operator=(const ItemScanner&) = delete;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
