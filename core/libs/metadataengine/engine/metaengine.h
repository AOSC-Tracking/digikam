/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface
 *               Exiv2: https://www.exiv2.org
 *               Exif : https://www.exif.org/Exif2-2.PDF
 *               Iptc : https://www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf
 *               Xmp  : https://www.adobe.com/devnet/xmp/pdfs/xmp_specification.pdf
 *                      https://www.iptc.org/std/Iptc4xmpCore/1.0/specification/Iptc4xmpCore_1.0-spec-XMPSchema_8.pdf
 *               Paper: www.metadataworkinggroup.com/pdf/mwg_guidance.pdf
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// QT includes

#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QSharedDataPointer>
#include <QStringList>
#include <QVariant>
#include <QRegularExpression>
#include <QUrl>
#include <QImage>

// Local includes

#include "metaengine_data.h"
#include "digikam_export.h"

namespace Digikam
{

// TODO: merge with DMetadata class.

class DIGIKAM_EXPORT MetaEngine
{

public:

    /**
     * The item metadata writing mode, between item file metadata and XMP sidecar file, depending on the context.
     * @sa MetadataWritingMode(), metadataWritingMode()
     */
    enum MetadataWritingMode
    {
        /// Write metadata to item file only.
        WRITE_TO_FILE_ONLY                        = 0,

        /// Write metadata to sidecar file only.
        WRITE_TO_SIDECAR_ONLY                     = 1,

        /// Write metadata to item and sidecar files.
        WRITE_TO_SIDECAR_AND_FILE                 = 2,

        /// Write metadata to sidecar file only for read only items such as RAW files for example.
        WRITE_TO_SIDECAR_ONLY_FOR_READ_ONLY_FILES = 3
    };

    /**
     * The item color workspace values given by Exif metadata.
     */
    enum ImageColorWorkSpace
    {
        WORKSPACE_UNSPECIFIED  = 0,
        WORKSPACE_SRGB         = 1,
        WORKSPACE_ADOBERGB     = 2,
        WORKSPACE_UNCALIBRATED = 65535
    };

    /**
     * The item orientation values given by Exif metadata.
     */
    enum ImageOrientation
    {
        ORIENTATION_UNSPECIFIED  = 0,
        ORIENTATION_NORMAL       = 1,
        ORIENTATION_HFLIP        = 2,
        ORIENTATION_ROT_180      = 3,
        ORIENTATION_VFLIP        = 4,
        ORIENTATION_ROT_90_HFLIP = 5,
        ORIENTATION_ROT_90       = 6,
        ORIENTATION_ROT_90_VFLIP = 7,
        ORIENTATION_ROT_270      = 8
    };

    /**
     * Xmp tag types, used by setXmpTag, only first three types are used
     */
    enum XmpTagType
    {
        NormalTag    = 0,
        ArrayBagTag  = 1,
        StructureTag = 2,
        ArrayLangTag = 3,
        ArraySeqTag  = 4
    };

    /**
     * Metadata Backend used to populate information.
     */
    enum Backend
    {
        Exiv2Backend       = 0,   ///< Default backend used by MetaEngine.
        LibRawBackend,            ///< DMetadata only.
        LibHeifBackend,           ///< DMetadata only.
        ImageMagickBackend,       ///< DMetadata only.
        FFMpegBackend,            ///< DMetadata only.
        ExifToolBackend,          ///< DMetadata only.
        VideoMergeBackend,        ///< DMetadata only.
        NoBackend                 ///< No backend used (aka file cannot be read).
    };

    /**
     * A map used to store Tags Key and Tags Value.
     */
    typedef QMap<QString, QString> MetaDataMap;

    /**
     * A map used to store a list of Alternative Language values.
     * The map key is the language code following RFC3066 notation
     * (like "fr-FR" for French), and the map value the text.
     */
    typedef QMap<QString, QString> AltLangMap;

    /**
     * A map used to store Tags Key and a list of Tags properties :
     *  - name,
     *  - title,
     *  - description.
     */
    typedef QMap<QString, QStringList> TagsMap;

public:

    /**
     * Standard constructor.
     */
    MetaEngine();

    /**
     * Constructor to load from parsed data.
     */
    explicit MetaEngine(const MetaEngineData& data);

    /**
     * Constructor to Load Metadata from item file.
     */
    explicit MetaEngine(const QString& filePath);

    /**
     * Standard destructor
     */
    virtual ~MetaEngine();

public:

    //-----------------------------------------------------------------
    /// @name Static methods
    //@{

    /**
     * Return true if Exiv2 library initialization is done properly.
     * This method must be called before using libMetaEngine with multithreading.
     * It initialize several non re-entrancy code from Adobe XMP SDK, and register
     * a function to cleanup automatically all XMP SDK memory allocation.
     * See Bug #166424 for details.
     */
    static bool initializeExiv2();

    /**
     * Return true if Exiv2 library is compiled with Xmp metadata support.
     */
    static bool supportXmp();

    /**
     * Return true if Exiv2 library is compiled with JpegXL metadata support.
     */
    static bool supportJpegXL();

    /**
     * Return true if library support Base Media File Format (aka CR3, HEIF, HEIC, and AVIF).
     * Note: use this function only after to call initializeExiv2(), else false will always returned.
     * The function return true only if Exiv2 >= 0.27.4 compiled with BMFF support.
     */
    static bool supportBmff();

    /**
     * Return true if library can write metadata to typeMime file format.
     */
    static bool supportMetadataWriting(const QString& typeMime);

    /**
     * Return a string version of Exiv2 release in format "major.minor.patch"
     */
    static QString Exiv2Version();

    //@}

    //-----------------------------------------------------------------
    /// @name General methods
    //@{

    MetaEngineData data() const;
    void setData(const MetaEngineData& data);

    /**
     * Load all metadata (Exif, Iptc, Xmp, and JFIF Comments) from a byte array.
     * Return true if metadata have been loaded successfully from item data.
     */
    bool loadFromData(const QByteArray& imgData);

    /**
     * Load and merge metadata (Exif, Iptc and Xmp) from a byte array.
     * Use 'exclude' to remove Exif tags from the 'imgData' that will not be merged.
     * Return true if metadata have been loaded and merged successfully from item data.
     */
    bool loadFromDataAndMerge(const QByteArray& imgData, const QStringList& exclude = QStringList());

    /**
     * Return 'true' if metadata container in memory as no Comments, Exif, Iptc, and Xmp.
     */
    bool isEmpty() const;

    /**
     * Returns the pixel size of the current item. This information is read from the file,
     * not from the metadata. The returned QSize is valid if the MetaEngine object was _constructed_
     * by reading a file or item data; the information is not available when the object
     * was created from MetaEngineData.
     * Note that in the Exif or XMP metadata, there may be fields describing the item size.
     * These fields are not accessed by this method.
     * When replacing the metadata with setData(), the metadata may change; this information
     * always keeps referring to the file it was initially read from.
     */
    QSize getPixelSize() const;

    /**
     * Returns the mime type of this item. The information is read from the file;
     * see the docs for getPixelSize() to know when it is available.
     */
    QString getMimeType() const;

    /**
     * Enable or disable writing metadata operations with ExifTool.
     */
    void setWriteWithExifTool(const bool on);

    /**
     * Return true if writing metadata operations with ExifTool is enabled.
     */
    bool writeWithExifTool() const;

    /**
     * Enable or disable writing metadata operations to RAW files.
     * By default RAW files are untouched.
     */
    void setWriteRawFiles(const bool on);

    /**
     * Return true if writing metadata operations on RAW files is enabled.
     */
    bool writeRawFiles() const;

    /**
     * Enable or disable writing metadata operations to DNG files.
     */
    void setWriteDngFiles(const bool on);

    /**
     * Return true if writing metadata operations on DNG files is enabled.
     */
    bool writeDngFiles() const;

    /**
     * Enable or disable using XMP sidecar for reading metadata.
     */
    void setUseXMPSidecar4Reading(const bool on);

    /**
     * Return true if using XMP sidecar for reading metadata is enabled.
     */
    bool useXMPSidecar4Reading() const;

    /**
     * Enable or disable using compatible file name for sidecar files.
     */
    void setUseCompatibleFileName(const bool on);

    /**
     * Return true if using compatible file name for sidecar files.
     */
    bool useCompatibleFileName() const;

    /**
     * Set metadata writing mode.
     * @param mode Metadata writing mode as defined by the #MetadataWritingMode enum.
     * @sa MetadataWritingMode, metadataWritingMode()
     */
    void setMetadataWritingMode(const int mode);

    /**
     * Return the metadata writing mode.
     * @returns Metadata writing mode as defined by the #MetadataWritingMode enum.
     * @sa MetadataWritingMode, setMetadataWritingMode()
     */
    int metadataWritingMode() const;

    /**
     * Enable or disable file timestamp updating when metadata are saved.
     * By default files timestamp are untouched.
     */
    void setUpdateFileTimeStamp(bool on);

    /**
     * Return true if file timestamp is updated when metadata are saved.
     */
    bool updateFileTimeStamp() const;

    //@}

    //-------------------------------------------------------------------
    /// @name File I/O methods
    //@{

    /**
     * Set the file path of current item.
     */
    void setFilePath(const QString& path);

    /**
     * Return the file path of current item.
     */
    QString getFilePath() const;

    /**
     * Return the XMP Sidecar file path for a item file path.
     * If item file path do not include a file name or is empty, this function return a null string.
     */
    static QString sidecarFilePathForFile(const QString& path);

    /**
     * Like sidecarFilePathForFile(), but works for local file path.
     */
    static QString sidecarPath(const QString& path);

    /**
     * Like sidecarFilePathForFile(), but works for remote URLs.
     */
    static QUrl sidecarUrl(const QUrl& url);

    /**
     * Gives a file url for a local path.
     */
    static QUrl sidecarUrl(const QString& path);

    /**
     * Performs a QFileInfo based check if the given local file has a sidecar.
     */
    static bool hasSidecar(const QString& path);

    /**
     * Return a string of backend name used to parse metadata from file.
     * See Backend enum for details.
     */
    static QString backendName(Backend t);

    /**
     * Load all metadata (Exif, Iptc, Xmp, and JFIF Comments) from a picture (JPEG, RAW, TIFF, PNG,
     * DNG, etc...). Return true if metadata have been loaded successfully from file.
     * If backend is non null, return the backend used to populate metadata (Exiv2).
     * See Backend enum for details.
     */
    bool load(const QString& filePath, Backend* backend = nullptr);

    /**
     * Load metadata from a sidecar file and merge.
     * Return true if metadata have been loaded successfully from file.
     */
    bool loadFromSidecarAndMerge(const QString& filePath);

    /**
     * Save all metadata to a file. This one can be different than original picture to perform
     * transfer operation Return true if metadata have been saved into file.
     */
    bool save(const QString& filePath, bool setVersion = false) const;

    /**
     * The same than save() method, but it apply on current item. Return true if metadata
     * have been saved into file.
     */
    bool applyChanges(bool setVersion = false) const;

    /**
     *  Export metadata to a temporary EXV file container.
     * 'exvTmpFile' is the path to the temporary EXV container to create.
     */
    bool exportChanges(const QString& exvTmpFile) const;

    //@}

    //-------------------------------------------------------------------
    /// @name Metadata item information manipulation methods
    //@{

    /**
     * Set Program name and program version in Exif and Iptc Metadata. Return true if information
     * have been changed in metadata.
     */
    bool setItemProgramId(const QString& program, const QString& version) const;

    /**
     * Return the size of item in pixels using Exif tags. Return a null dimension if size cannot
     * be found.
     */
    QSize getItemDimensions() const;

    /**
     * Set the size of item in pixels in Exif tags. Return true if size have been changed
     * in metadata.
     */
    bool setItemDimensions(const QSize& size) const;

    /**
     * Return the item orientation set in Exif metadata. The makernotes of item are also parsed to
     * get this information. See ImageOrientation values for details.
     */
    MetaEngine::ImageOrientation getItemOrientation() const;

    /**
     * Set the Exif orientation tag of item. See ImageOrientation values for details
     * Return true if orientation have been changed in metadata.
     */
    bool setItemOrientation(ImageOrientation orientation) const;

    /**
     * Return the item color-space set in Exif metadata. The makernotes of item are also parsed to
     * get this information. See ImageColorWorkSpace values for details.
     */
    MetaEngine::ImageColorWorkSpace getItemColorWorkSpace() const;

    /**
     * Set the Exif color-space tag of item. See ImageColorWorkSpace values for details
     * Return true if work-space have been changed in metadata.
     */
    bool setItemColorWorkSpace(ImageColorWorkSpace workspace) const;

    /**
     * Return the time stamp of item. Exif information are check in first, IPTC in second
     * if item don't have Exif information. If no time stamp is found, a null date is returned.
     */
    QDateTime getItemDateTime() const;

    /**
     * Set the Exif and Iptc time stamp. If 'setDateTimeDigitized' parameter is true, the 'Digitalized'
     * time stamp is set, else only 'Created' time stamp is set.
     */
    bool setImageDateTime(const QDateTime& dateTime, bool setDateTimeDigitized = false) const;

    /**
     * Return the digitization time stamp of the item. First Exif information is checked, then IPTC.
     * If no digitization time stamp is found, getItemDateTime() is called if fallbackToCreationTime
     * is true, or a null QDateTime is returned if fallbackToCreationTime is false.
     */
    QDateTime getDigitizationDateTime(bool fallbackToCreationTime = false) const;

    /**
     * Return a QImage copy of Iptc preview image. Return a null item if preview cannot
     * be found.
     */
    bool getItemPreview(QImage& preview) const;

    /**
     * Set the Iptc preview image. The thumbnail item must have the right size before (64Kb max
     * with JPEG file, else 256Kb). Look Iptc specification for details. Return true if preview
     * have been changed in metadata.
     * Re-implement this method if you want to use another item file format than JPEG to
     * save preview.
     */
    bool setItemPreview(const QImage& preview) const;

    //@}

    //-----------------------------------------------------------------
    /// @name Comments manipulation methods
    //@{

    /**
     * Return 'true' if Comments can be written in file.
     */
    static bool canWriteComment(const QString& filePath);

    /**
     * Return 'true' if metadata container in memory as Comments.
     */
    bool hasComments() const;

    /**
     * Clear the Comments metadata container in memory.
     */
    bool clearComments() const;

    /**
     * Return a Qt byte array copy of Comments container get from current item.
     * Comments are JFIF section of JPEG images. Look Exiv2 API for more information.
     * Return a null Qt byte array if there is no Comments metadata in memory.
     */
    QByteArray getComments() const;

    /**
     * Return a Qt string object of Comments from current item decoded using
     * the 'detectEncodingAndDecode()' method. Return a null string if there is no
     * Comments metadata available.
     */
    QString getCommentsDecoded() const;

    /**
     * Set the Comments data using a Qt byte array. Return true if Comments metadata
     * have been changed in memory.
     */
    bool setComments(const QByteArray& data) const;

    /**
     * Language Alternative autodetection. Return a QString without language alternative
     * header. Header is saved into 'lang'. If no language alternative is founf, value is returned
     * as well and 'lang' is set to a null string.
     */
    static QString detectLanguageAlt(const QString& value, QString& lang);

    //@}

    //-----------------------------------------------------------------
    /// @name Exif manipulation methods
    //@{

    /**
     * Return a map of all standard Exif tags supported by Exiv2.
     */
    TagsMap getStdExifTagsList() const;

    /**
     * Return a map of all non-standard Exif tags (makernotes) supported by Exiv2.
     */
    TagsMap getMakernoteTagsList() const;

    /**
     * Return 'true' if Exif can be written in file.
     */
    static bool canWriteExif(const QString& filePath);

    /**
     * Return 'true' if metadata container in memory as Exif.
     */
    bool hasExif() const;

    /**
     * Clear the Exif metadata container in memory.
     */
    bool clearExif() const;

    /**
     * Returns the exif data encoded to a QByteArray in a form suitable
     * for storage in a JPEG image.
     * Note that this encoding is a lossy operation.
     *
     * Set true 'addExifHeader' parameter to add an Exif header to Exif metadata.
     * Returns a null Qt byte array if there is no Exif metadata in memory.
     */
    QByteArray getExifEncoded(bool addExifHeader = false) const;

    /**
     * Set the Exif data using a Qt byte array. Return true if Exif metadata
     * have been changed in memory.
     */
    bool setExif(const QByteArray& data) const;

    /**
     * Return a QImage copy of Exif thumbnail image. Return a null image if thumbnail cannot
     * be found. The 'fixOrientation' parameter will rotate automatically the thumbnail if Exif
     * orientation tags information are attached with thumbnail.
     */
    QImage getExifThumbnail(bool fixOrientation) const;

    /**
     * Fix orientation of a QImage image accordingly with Exif orientation tag.
     * Return true if image is rotated, else false.
     */
    bool rotateExifQImage(QImage& image, ImageOrientation orientation) const;

    /**
     * Set the Exif Thumbnail image. The thumbnail image must have the right dimensions before.
     * Look Exif specification for details. Return true if thumbnail have been changed in metadata.
     */
    bool setExifThumbnail(const QImage& thumb) const;

    /**
     * Remove the Exif Thumbnail from the item
     */
    bool removeExifThumbnail() const;

    /**
     * Adds a JPEG thumbnail to a TIFF images. Use this instead of setExifThumbnail for TIFF images.
     */
    bool setTiffThumbnail(const QImage& thumb) const;

    /**
     * Return a QString copy of Exif user comments. Return a null string if user comments cannot
     * be found.
     */
    QString getExifComment(bool readDescription = true) const;

    /**
     * Return a Exif tag comment like a string. Return a null string if user comments cannot
     * be found.
     */
    QString getExifTagComment(const char* exifTagName) const;

    /**
     * Set the Exif user comments from item. Look Exif specification for more details about this tag.
     * Return true if Exif user comments have been changed in metadata.
     */
    bool setExifComment(const QString& comment, bool writeDescription = true) const;

    /**
     * Get an Exif tags content like a string. If 'escapeCR' parameter is true, the CR characters
     * will be removed. If Exif tag cannot be found a null string is returned.
     */
    QString getExifTagString(const char* exifTagName, bool escapeCR = true) const;

    /**
     * Set an Exif tag content using a string. Return true if tag is set successfully.
     */
    bool setExifTagString(const char* exifTagName, const QString& value) const;

    /**
     * Get an Exif tag content like a long value. Return true if Exif tag be found.
     */
    bool getExifTagLong(const char* exifTagName, long &val) const;

    /**
     * Get an Exif tag content like a long value. Return true if Exif tag be found.
     */
    bool getExifTagLong(const char* exifTagName, long &val, int component) const;

    /**
     * Set an Exif tag content using a long value. Return true if tag is set successfully.
     */
    bool setExifTagLong(const char* exifTagName, long val) const;

    /**
     * Set an Exif tag content using a unsigned short value. Return true if tag is set successfully.
     */
    bool setExifTagUShort(const char* exifTagName, unsigned int val) const;

    /**
     * Get the 'component' index of an Exif tags content like a rational value.
     * 'num' and 'den' are the numerator and the denominator of the rational value.
     * Return true if Exif tag be found.
     */
    bool getExifTagRational(const char* exifTagName, long int& num, long int& den, int component = 0) const;

    /**
     * Set an Exif tag content using a rational value.
     * 'num' and 'den' are the numerator and the denominator of the rational value.
     * Return true if tag is set successfully.
     */
    bool setExifTagRational(const char* exifTagName, long int num, long int den) const;

    /**
     * Set an Exif tag content using a unsigned rational value.
     * 'num' and 'den' are the numerator and the denominator of the unsigned rational value.
     * Return true if tag is set successfully.
     */
    bool setExifTagURational(const char* exifTagName, unsigned long int num, unsigned long int den) const;

    /**
     * Get an Exif tag content like a bytes array. Return an empty bytes array if Exif
     * tag cannot be found.
     */
    QByteArray getExifTagData(const char* exifTagName) const;

    /**
     * Set an Exif tag content using a bytes array. Return true if tag is set successfully.
     */
    bool setExifTagData(const char* exifTagName, const QByteArray& data) const;

    /**
     * Get an Exif tags content as a QVariant. Returns a null QVariant if the Exif
     * tag cannot be found.
     * For string and integer values the matching QVariant types will be used,
     * for date and time values QVariant::DateTime.
     * Rationals will be returned as QVariant::List with two integer QVariants (numerator, denominator)
     * if rationalAsListOfInts is true, as double if rationalAsListOfInts is false.
     * An exif tag of numerical type may contain more than one value; set component to the desired index.
     */
    QVariant getExifTagVariant(const char* exifTagName, bool rationalAsListOfInts = true,
                               bool escapeCR = true, int component = 0) const;

    /**
     * Set an Exif tag content using a QVariant. Returns true if tag is set successfully.
     * All types described for the getExifTagVariant() method are supported.
     * Calling with a QVariant of type ByteArray is equivalent to calling setExifTagData.
     * For the meaning of rationalWantSmallDenominator, see the documentation of the convertToRational methods.
     * Setting a value with multiple components is currently not supported.
     */
    bool setExifTagVariant(const char* exifTagName, const QVariant& data,
                           bool rationalWantSmallDenominator = true) const;

    /**
     * Remove the Exif tag 'exifTagName' from Exif metadata. Return true if tag is
     * removed successfully or if no tag was present.
     */
    bool removeExifTag(const char* exifTagName) const;

    /**
     * Return the Exif Tag title or a null string.
     */
    QString getExifTagTitle(const char* exifTagName);

    /**
     * Return the Exif Tag description or a null string.
     */
    QString getExifTagDescription(const char* exifTagName);

    /**
     * Takes a QVariant value as it could have been retrieved by getExifTagVariant with the given exifTagName,
     * and returns its value properly converted to a string (including translations from Exiv2).
     * This is equivalent to calling getExifTagString directly.
     * If escapeCR is true CR characters will be removed from the result.
     */
    QString createExifUserStringFromValue(const char* exifTagName, const QVariant& val, bool escapeCR = true);

    /**
     * Return a map of Exif tags name/value found in metadata sorted by
     * Exif keys given by 'exifKeysFilter'.
     *
     * 'exifKeysFilter' is a QStringList of Exif keys.
     * For example, if you use the string list given below:
     *
     * "Iop"
     * "Thumbnail"
     * "Image"
     * "Photo"
     *
     * List can be empty to not filter output.
     *
     * ... this method will return a map of all Exif tags which :
     *
     * - include "Iop", or "Thumbnail", or "Image", or "Photo" in the Exif tag keys
     *   if 'inverSelection' is false.
     * - not include "Iop", or "Thumbnail", or "Image", or "Photo" in the Exif tag keys
     * if 'inverSelection' is true.
     * if 'extractBinary" is true, tags with undefined types of data are extracted (default),
     * else contents is replaced by "Binary data ... bytes". Take a care as large binary data as
     * original RAW data from DNG container can be huge and listing Exif tags from GUI can take a while.
     */
    MetaEngine::MetaDataMap getExifTagsDataList(const QStringList& exifKeysFilter = QStringList(),
                                                bool invertSelection = false,
                                                bool extractBinary = true) const;

    //@}

    //-------------------------------------------------------------
    /// @name IPTC manipulation methods
    //@{

    /**
     * Return a map of all standard Iptc tags supported by Exiv2.
     */
    MetaEngine::TagsMap getIptcTagsList() const;

    /**
     * Return 'true' if Iptc can be written in file.
     */
    static bool canWriteIptc(const QString& filePath);

    /**
     * Return 'true' if metadata container in memory as Iptc.
     */
    bool hasIptc() const;

    /**
     * Clear the Iptc metadata container in memory.
     */
    bool clearIptc() const;

    /**
     * Return a Qt byte array copy of Iptc container get from current item.
     * Set true 'addIrbHeader' parameter to add an Irb header to Iptc metadata.
     * Return a null Qt byte array if there is no Iptc metadata in memory.
     */
    QByteArray  getIptc(bool addIrbHeader = false) const;

    /**
     * Set the Iptc data using a Qt byte array. Return true if Iptc metadata
     * have been changed in memory.
     */
    bool setIptc(const QByteArray& data) const;

    /**
     * Get an Iptc tag content like a string. If 'escapeCR' parameter is true, the CR characters
     * will be removed. If Iptc tag cannot be found a null string is returned.
     */
    QString getIptcTagString(const char* iptcTagName, bool escapeCR = true) const;

    /**
     * Set an Iptc tag content using a string. Return true if tag is set successfully.
     */
    bool setIptcTagString(const char* iptcTagName, const QString& value) const;

    /**
     * Returns a strings list with of multiple Iptc tags from the item. Return an empty list if no tag is found.
     * Get the values of all IPTC tags with the given tag name in a string list.
     * (In Iptc, there can be multiple tags with the same name)
     * If the 'escapeCR' parameter is true, the CR characters
     * will be removed.
     * If no tag can be found an empty list is returned.
     */
    QStringList getIptcTagsStringList(const char* iptcTagName, bool escapeCR = true) const;

    /**
     * Set multiple Iptc tags contents using a strings list. 'maxSize' is the max characters size
     * of one entry. Return true if all tags have been set successfully.
     */
    bool setIptcTagsStringList(const char* iptcTagName, int maxSize,
                               const QStringList& oldValues, const QStringList& newValues) const;

    /**
     * Get an Iptc tag content as a bytes array. Return an empty bytes array if Iptc
     * tag cannot be found.
     */
    QByteArray getIptcTagData(const char* iptcTagName) const;

    /**
     * Set an Iptc tag content using a bytes array. Return true if tag is set successfully.
     */
    bool setIptcTagData(const char* iptcTagName, const QByteArray& data) const;

    /**
     * Remove the all instance of Iptc tags 'iptcTagName' from Iptc metadata. Return true if all
     * tags have been removed successfully (or none were present).
     */
    bool removeIptcTag(const char* iptcTagName) const;

    /**
     * Return the Iptc Tag title or a null string.
     */
    QString getIptcTagTitle(const char* iptcTagName);

    /**
     * Return the Iptc Tag description or a null string.
     */
    QString getIptcTagDescription(const char* iptcTagName);

    /**
     * Return a map of Iptc tags name/value found in metadata sorted by
     * Iptc keys given by 'iptcKeysFilter'.
     *
     * 'iptcKeysFilter' is a QStringList of Iptc keys.
     * For example, if you use the string list given below:
     *
     * "Envelope"
     * "Application2"
     *
     * List can be empty to not filter output.
     *
     * ... this method will return a map of all Iptc tags which :
     *
     * - include "Envelope", or "Application2" in the Iptc tag keys
     *   if 'inverSelection' is false.
     * - not include "Envelope", or "Application2" in the Iptc tag keys
     *   if 'inverSelection' is true.
     */
    MetaEngine::MetaDataMap getIptcTagsDataList(const QStringList& iptcKeysFilter = QStringList(),
                                                bool invertSelection = false) const;

    /**
     * Return a strings list of Iptc keywords from item. Return an empty list if no keyword are set.
     */
    QStringList getIptcKeywords() const;

    /**
     * Set Iptc keywords using a list of strings defined by 'newKeywords' parameter. Use 'getImageKeywords()'
     * method to set 'oldKeywords' parameter with existing keywords from item. The method will compare
     * all new keywords with all old keywords to prevent duplicate entries in item. Return true if keywords
     * have been changed in metadata.
     */
    bool setIptcKeywords(const QStringList& oldKeywords, const QStringList& newKeywords) const;

    /**
     * Return a strings list of Iptc subjects from item. Return an empty list if no subject are set.
     */
    QStringList getIptcSubjects() const;

    /**
     * Set Iptc subjects using a list of strings defined by 'newSubjects' parameter. Use 'getImageSubjects()'
     * method to set 'oldSubjects' parameter with existing subjects from item. The method will compare
     * all new subjects with all old subjects to prevent duplicate entries in item. Return true if subjects
     * have been changed in metadata.
     */
    bool setIptcSubjects(const QStringList& oldSubjects, const QStringList& newSubjects) const;

    /**
     * Return a strings list of Iptc sub-categories from item. Return an empty list if no sub-category
     * are set.
     */
    QStringList getIptcSubCategories() const;

    /**
     * Set Iptc sub-categories using a list of strings defined by 'newSubCategories' parameter. Use
     * 'getImageSubCategories()' method to set 'oldSubCategories' parameter with existing sub-categories
     * from item. The method will compare all new sub-categories with all old sub-categories to prevent
     * duplicate entries in item. Return true if sub-categories have been changed in metadata.
     */
    bool setIptcSubCategories(const QStringList& oldSubCategories, const QStringList& newSubCategories) const;

    //@}

    //------------------------------------------------------------
    /// @name XMP manipulation methods
    //@{

    /**
     * Return a map of all standard Xmp tags supported by Exiv2.
     */
    MetaEngine::TagsMap getXmpTagsList() const;

    /**
     * Return 'true' if Xmp can be written in file.
     */
    static bool canWriteXmp(const QString& filePath);

    /**
     * Return 'true' if metadata container in memory as Xmp.
     */
    bool hasXmp() const;

    /**
     * Clear the Xmp metadata container in memory.
     */
    bool clearXmp() const;

    /**
     * Return a Qt byte array copy of XMp container get from current item.
     * Return a null Qt byte array if there is no Xmp metadata in memory.
     */
    QByteArray getXmp() const;

    /**
     * Set the Xmp data using a Qt byte array. Return true if Xmp metadata
     * have been changed in memory.
     */
    bool setXmp(const QByteArray& data) const;

    /**
     * Get a Xmp tag content like a string. If 'escapeCR' parameter is true, the CR characters
     * will be removed. If Xmp tag cannot be found a null string is returned.
     */
    QString getXmpTagString(const char* xmpTagName, bool escapeCR = true) const;

    /**
     * Set a Xmp tag content using a string. Return true if tag is set successfully.
     */
    bool setXmpTagString(const char* xmpTagName, const QString& value) const;

    /**
     * Set a Xmp tag with a specific type. Return true if tag is set successfully.
     * This method only accept NormalTag, ArrayBagTag and StructureTag.
     * Other XmpTagTypes do nothing
     */
    bool setXmpTagString(const char* xmpTagName, const QString& value,
                         XmpTagType type) const;

    /**
     * Return the Xmp Tag title or a null string.
     */
    QString getXmpTagTitle(const char* xmpTagName);

    /**
     * Return the Xmp Tag description or a null string.
     */
    QString getXmpTagDescription(const char* xmpTagName);

    /**
     * Return a map of Xmp tags name/value found in metadata sorted by
     * Xmp keys given by 'xmpKeysFilter'.
     *
     * 'xmpKeysFilter' is a QStringList of Xmp keys.
     * For example, if you use the string list given below:
     *
     * "dc"           // Dubling Core schema.
     * "xmp"          // Standard Xmp schema.
     *
     * List can be empty to not filter output.
     *
     * ... this method will return a map of all Xmp tags which :
     *
     * - include "dc", or "xmp" in the Xmp tag keys
     *   if 'inverSelection' is false.
     * - not include "dc", or "xmp" in the Xmp tag keys
     *   if 'inverSelection' is true.
     */
    MetaEngine::MetaDataMap getXmpTagsDataList(const QStringList& xmpKeysFilter = QStringList(),
                                               bool invertSelection = false) const;

    /**
     * Get all redondant Alternative Language Xmp tags content like a map.
     * See AltLangMap class description for details.
     * If 'escapeCR' parameter is true, the CR characters will be removed from strings.
     * If Xmp tag cannot be found a null string list is returned.
     */
    MetaEngine::AltLangMap getXmpTagStringListLangAlt(const char* xmpTagName,
                                                      bool escapeCR = true) const;

    /**
     * Set an Alternative Language Xmp tag content using a map. See AltLangMap class
     * description for details. If tag already exist, it will be removed before.
     * Return true if tag is set successfully.
     */
    bool setXmpTagStringListLangAlt(const char* xmpTagName, const MetaEngine::AltLangMap& values) const;

    /**
     * Get a Xmp tag content like a string set with an alternative language
     * header 'langAlt' (like "fr-FR" for French - RFC3066 notation)
     * If 'escapeCR' parameter is true, the CR characters will be removed.
     * If Xmp tag cannot be found a null string is returned.
     */
    QString getXmpTagStringLangAlt(const char* xmpTagName, const QString& langAlt, bool escapeCR) const;

    /**
     * Set a Xmp tag content using a string with an alternative language header. 'langAlt' contain the
     * language alternative information (like "fr-FR" for French - RFC3066 notation) or is null to
     * set alternative language to default settings ("x-default").
     * Return true if tag is set successfully.
     */
    bool setXmpTagStringLangAlt(const char* xmpTagName, const QString& value,
                                const QString& langAlt) const;

    /**
     * Get a Xmp tag content like a sequence of strings. If 'escapeCR' parameter is true, the CR characters
     * will be removed from strings. If Xmp tag cannot be found a null string list is returned.
     */
    QStringList getXmpTagStringSeq(const char* xmpTagName, bool escapeCR = true) const;

    /**
     * Set a Xmp tag content using the sequence of strings 'seq'.
     * Return true if tag is set successfully.
     */
    bool setXmpTagStringSeq(const char* xmpTagName, const QStringList& seq) const;

    /**
     * Get a Xmp tag content like a bag of strings. If 'escapeCR' parameter is true, the CR characters
     * will be removed from strings. If Xmp tag cannot be found a null string list is returned.
     */
    QStringList getXmpTagStringBag(const char* xmpTagName, bool escapeCR) const;

    /**
     * Set a Xmp tag content using the bag of strings 'bag'.
     * Return true if tag is set successfully.
     */
    bool setXmpTagStringBag(const char* xmpTagName, const QStringList& bag) const;

    /**
     * Set an Xmp tag content using a list of strings defined by the 'entriesToAdd' parameter.
     * The existing entries are preserved. The method will compare
     * all new with all already existing entries to prevent duplicates in the item.
     * Return true if the entries have been added to metadata.
     */
    bool addToXmpTagStringBag(const char* xmpTagName, const QStringList& entriesToAdd) const;

    /**
     * Remove those Xmp tag entries that are listed in entriesToRemove from the entries in metadata.
     * Return true if tag entries are no longer contained in metadata.
     * All other entries are preserved.
     */
    bool removeFromXmpTagStringBag(const char* xmpTagName, const QStringList& entriesToRemove) const;

    /**
     * Get an Xmp tag content as a QVariant. Returns a null QVariant if the Xmp
     * tag cannot be found.
     * For string and integer values the matching QVariant types will be used,
     * for date and time values QVariant::DateTime.
     * Rationals will be returned as QVariant::List with two integer QVariants (numerator, denominator)
     * if rationalAsListOfInts is true, as double if rationalAsListOfInts is false.
     * Arrays (ordered, unordered, alternative) are returned as type StringList.
     * LangAlt values will have type Map (QMap<QString, QVariant>) with the language
     * code as key and the contents as value, of type String.
     */
    QVariant getXmpTagVariant(const char* xmpTagName, bool rationalAsListOfInts = true, bool stringEscapeCR = true) const;

    /**
     * Return a strings list of Xmp keywords from item. Return an empty list if no keyword are set.
     */
    QStringList getXmpKeywords() const;

    /**
     * Set Xmp keywords using a list of strings defined by 'newKeywords' parameter.
     * The existing keywords from item are preserved. The method will compare
     * all new keywords with all already existing keywords to prevent duplicate entries in item.
     * Return true if keywords have been changed in metadata.
     */
    bool setXmpKeywords(const QStringList& newKeywords) const;

    /**
     * Remove those Xmp keywords that are listed in keywordsToRemove from the keywords in metadata.
     * Return true if keywords are no longer contained in metadata.
     */
    bool removeXmpKeywords(const QStringList& keywordsToRemove);

    /**
     * Return a strings list of Xmp subjects from item. Return an empty list if no subject are set.
     */
    QStringList getXmpSubjects() const;

    /**
     * Set Xmp subjects using a list of strings defined by 'newSubjects' parameter.
     * The existing subjects from item are preserved. The method will compare
     * all new subject with all already existing subject to prevent duplicate entries in item.
     * Return true if subjects have been changed in metadata.
     */
    bool setXmpSubjects(const QStringList& newSubjects) const;

    /**
     * Remove those Xmp subjects that are listed in subjectsToRemove from the subjects in metadata.
     * Return true if subjects are no longer contained in metadata.
     */
    bool removeXmpSubjects(const QStringList& subjectsToRemove);

    /**
     * Return a strings list of Xmp sub-categories from item. Return an empty list if no sub-category
     * are set.
     */
    QStringList getXmpSubCategories() const;

    /**
     * Set Xmp sub-categories using a list of strings defined by 'newSubCategories' parameter.
     * The existing sub-categories from item are preserved. The method will compare
     * all new sub-categories with all already existing sub-categories to prevent duplicate entries in item.
     * Return true if sub-categories have been changed in metadata.
     */
    bool setXmpSubCategories(const QStringList& newSubCategories) const;

    /**
     * Remove those Xmp sub-categories that are listed in categoriesToRemove from the sub-categories in metadata.
     * Return true if subjects are no longer contained in metadata.
     */
    bool removeXmpSubCategories(const QStringList& categoriesToRemove);

    /**
     * Remove the Xmp tag 'xmpTagName' from Xmp metadata. Return true if tag is
     * removed successfully or if no tag was present.
     */
    bool removeXmpTag(const char* xmpTagName, bool family = false) const;

    /**
     * Register a namespace which Exiv2 doesn't know yet. This is only needed
     * when new Xmp properties are added manually. 'uri' is the namespace url and 'prefix' the
     * string used to construct new Xmp key (ex. "Xmp.digiKam.tagList").
     * NOTE: If the Xmp metadata is read from an item, namespaces are decoded and registered
     * by Exiv2 at the same time.
     */
    static bool registerXmpNameSpace(const QString& uri, const QString& prefix);

    /**
     * Unregister a previously registered custom namespace
     */
    static bool unregisterXmpNameSpace(const QString& uri);

    //@}

    //------------------------------------------------------------
    /// @name GPS manipulation methods
    //@{

    /**
     * Make sure all static required GPS EXIF and XMP tags exist
     */
    bool initializeGPSInfo();

    /**
     * Get all GPS location information set in item. Return true if all information can be found.
     */
    bool getGPSInfo(double& altitude, double& latitude, double& longitude) const;

    /**
     * Get GPS location information set in the item, in the GPSCoordinate format
     * as described in the XMP specification. Returns a null string in the information cannot be found.
     */
    QString getGPSLatitudeString()  const;
    QString getGPSLongitudeString() const;

    /**
     * Get GPS location information set in the item, as a double floating point number as in degrees
     * where the sign determines the direction ref (North + / South - ; East + / West -).
     * Returns true if the information is available.
     */
    bool getGPSLatitudeNumber(double* const latitude)   const;
    bool getGPSLongitudeNumber(double* const longitude) const;

    /**
     * Get GPS altitude information, in meters, relative to sea level (positive sign above sea level)
     */
    bool getGPSAltitude(double* const altitude) const;

    /**
     * Set all GPS location information into item. Return true if all information have been
     * changed in metadata.
     */
    bool setGPSInfo(const double altitude, const double latitude, const double longitude);

    /**
     * Set all GPS location information into item. Return true if all information have been
     * changed in metadata. If you do not want altitude to be set, pass a null pointer.
     */
    bool setGPSInfo(const double* const altitude, const double latitude, const double longitude);

    /**
     * Set all GPS location information into item. Return true if all information have been
     * changed in metadata.
     */
    bool setGPSInfo(const double altitude, const QString& latitude, const QString& longitude);

    /**
     * Remove all Exif tags relevant of GPS location information. Return true if all tags have been
     * removed successfully in metadata.
     */
    bool removeGPSInfo();

    /**
     * This method converts 'number' to a rational value, returned in the 'numerator' and
     * 'denominator' parameters. Set the precision using 'rounding' parameter.
     * Use this method if you want to retrieve a most exact rational for a number
     * without further properties, without any requirements to the denominator.
     */
    static void convertToRational(const double number,
                                  long int* const numerator,
                                  long int* const denominator,
                                  const int rounding);

    /**
     * This method convert a 'number' to a rational value, returned in 'numerator' and
     * 'denominator' parameters.
     * This method will be able to retrieve a rational number from a double - if you
     * constructed your double with 1.0 / 4786.0, this method will retrieve 1 / 4786.
     * If your number is not expected to be rational, use the method above which is just as
     * exact with rounding = 4 and more exact with rounding > 4.
     */
    static void convertToRationalSmallDenominator(const double number,
                                                  long int* const numerator,
                                                  long int* const denominator);

    /**
     * Converts degrees values as a double representation. This code take a care about hemisphere position.
     */
    static double convertDegreeAngleToDouble(double degrees, double minutes, double seconds);


    /**
     * Converts a GPS position stored as rationals in Exif to the form described
     * as GPSCoordinate in the XMP specification, either in the from "256,45,34N" or "256,45.566667N"
     */
    static QString convertToGPSCoordinateString(const long int numeratorDegrees,
                                                const long int denominatorDegrees,
                                                const long int numeratorMinutes,
                                                const long int denominatorMinutes,
                                                const long int numeratorSeconds,
                                                const long int denominatorSeconds,
                                                const char directionReference);

    /**
     * Converts a GPS position stored as double floating point number in degrees to the form described
     * as GPSCoordinate in the XMP specification.
     */
    static QString convertToGPSCoordinateString(const bool isLatitude,
                                                double coordinate);

    /**
     * Converts a GPSCoordinate string as defined by XMP to three rationals and the direction reference.
     * Returns true if the conversion was successful.
     * If minutes is given in the fractional form, a denominator of 1000000 for the minutes will be used.
     */
    static bool convertFromGPSCoordinateString(const QString& coordinate,
                                               long int* const numeratorDegrees,
                                               long int* const denominatorDegrees,
                                               long int* const numeratorMinutes,
                                               long int* const denominatorMinutes,
                                               long int* const numeratorSeconds,
                                               long int* const denominatorSeconds,
                                               char* const directionReference);

    /**
     * Convert a GPSCoordinate string as defined by XMP to a double floating point number in degrees
     * where the sign determines the direction ref (North + / South - ; East + / West -).
     * Returns true if the conversion was successful.
     */
    static bool convertFromGPSCoordinateString(const QString& gpsString,
                                               double* const coordinate);

    /**
     * Converts a GPSCoordinate string to user presentable numbers, integer degrees and minutes and
     * double floating point seconds, and a direction reference ('N' or 'S', 'E' or 'W')
     */
    static bool convertToUserPresentableNumbers(const QString& coordinate,
                                                int* const degrees,
                                                int* const minutes,
                                                double* const seconds,
                                                char* const directionReference);

    /**
     * Converts a double floating point number to user presentable numbers, integer degrees and minutes and
     * double floating point seconds, and a direction reference ('N' or 'S', 'E' or 'W').
     * The method needs to know for the direction reference
     * if the latitude or the longitude is meant by the double parameter.
     */
    static void convertToUserPresentableNumbers(const bool isLatitude,
                                                double coordinate,
                                                int* const degrees,
                                                int* const minutes,
                                                double* const seconds,
                                                char* const directionReference);

    //@}

protected:

    /**
     * Set the Program Name and Program Version
     * information in Exif and Iptc metadata
     */
    bool setProgramId() const;

private:

    // Disable copy constructor and operator to prevent potential slicing with this class, reported by Clazy static analyzer.
    // https://github.com/KDE/clazy/blob/master/docs/checks/README-copyable-polymorphic.md
    // This methods was implemented to be able to pass this class or a derived version to signals and slots. This is very
    // Dangerous as virtual methods are present in this polymorphic class and is copyable.
    // Instead to use this class in signals and slots, use MetaEngineData container.

    // TODO: remove legacy implementations for these methods later if no side effect.

    MetaEngine(const MetaEngine& metadata);
    MetaEngine& operator=(const MetaEngine& metadata);

private:

    /**
     * Internal container to store private members. Used to improve binary compatibility
     */
    class Private;
    Private* const d = nullptr;

    friend class MetaEnginePreviews;
};

} // namespace Digikam
