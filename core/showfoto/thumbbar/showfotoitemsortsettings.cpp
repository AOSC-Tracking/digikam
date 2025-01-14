/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 29-07-2013
 * Description : Sort settings for showfoto item infos
 *
 * SPDX-FileCopyrightText: 2013 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "showfotoitemsortsettings.h"

// Qt includes

#include <QRectF>

namespace ShowFoto
{

bool ShowfotoItemSortSettings::operator ==(const ShowfotoItemSortSettings& other) const
{
    return (
            (categorizationMode            == other.categorizationMode)            &&
            (categorizationSortOrder       == other.categorizationSortOrder)       &&
            (categorizationCaseSensitivity == other.categorizationCaseSensitivity) &&
            (sortRole                      == other.sortRole)                      &&
            (sortOrder                     == other.sortOrder)                     &&
            (sortCaseSensitivity           == other.sortCaseSensitivity)
           );
}

void ShowfotoItemSortSettings::setCategorizationMode(CategorizationMode mode)
{
    categorizationMode = mode;

    if (categorizationSortOrder == DefaultOrder)
    {
        currentCategorizationSortOrder = defaultSortOrderForCategorizationMode(categorizationMode);
    }
}

void ShowfotoItemSortSettings::setCategorizationSortOrder(SortOrder order)
{
    categorizationSortOrder = order;

    if (categorizationSortOrder == DefaultOrder)
    {
        currentCategorizationSortOrder = defaultSortOrderForCategorizationMode(categorizationMode);
    }
    else
    {
        currentCategorizationSortOrder = (Qt::SortOrder)categorizationSortOrder;
    }
}

void ShowfotoItemSortSettings::setSortRole(SortRole role)
{
    sortRole = role;

    if (sortOrder == DefaultOrder)
    {
        currentSortOrder = defaultSortOrderForSortRole(sortRole);
    }
}

void ShowfotoItemSortSettings::setSortOrder(SortOrder order)
{
    sortOrder = order;

    if (sortOrder == DefaultOrder)
    {
        currentSortOrder = defaultSortOrderForSortRole(sortRole);
    }
    else
    {
        currentSortOrder = (Qt::SortOrder)order;
    }
}

Qt::SortOrder ShowfotoItemSortSettings::defaultSortOrderForCategorizationMode(CategorizationMode mode)
{
    switch (mode)
    {
        case NoCategories:
        case CategoryByFolder:
        case CategoryByFormat:
        default:
        {
            return Qt::AscendingOrder;
        }
    }
}

Qt::SortOrder ShowfotoItemSortSettings::defaultSortOrderForSortRole(SortRole role)
{
    switch (role)
    {
        case SortByFileName:
        case SortByFileSize:
        {
            return Qt::DescendingOrder;
        }

        case SortByCreationDate:
        {
            return Qt::AscendingOrder;
        }

        default:
        {
            return Qt::AscendingOrder;
        }
    }
}

int ShowfotoItemSortSettings::compareCategories(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right) const
{
    switch (categorizationMode)
    {
        case NoCategories:
        case CategoryByFolder:
        {
            return naturalCompare(left.folder, right.folder, currentCategorizationSortOrder, categorizationCaseSensitivity);
        }

        case CategoryByFormat:
        {
            return naturalCompare(left.mime, right.mime, currentCategorizationSortOrder, categorizationCaseSensitivity);
        }

        default:
        {
            return 0;
        }
    }
}

bool ShowfotoItemSortSettings::lessThan(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right) const
{
    int result = compare(left, right, sortRole);

    if (result != 0)
    {
        return (result < 0);
    }

    if (left == right)
    {
        return false;
    }

    if ((result = compare(left, right, SortByFileName)) != 0)
    {
        return (result < 0);
    }

    if ((result = compare(left, right, SortByCreationDate)) != 0)
    {
        return (result < 0);
    }

    return false;
}

int ShowfotoItemSortSettings::compare(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right) const
{
    return compare(left, right, sortRole);
}

int ShowfotoItemSortSettings::compare(const ShowfotoItemInfo& left, const ShowfotoItemInfo& right, SortRole role) const
{
    switch (role)
    {
        case SortByFileName:
        {
            return naturalCompare(left.name, right.name, currentSortOrder, sortCaseSensitivity);
        }

        case SortByFileSize:
        {
            return compareByOrder(left.size, right.size, currentSortOrder);

        }

        // FIXME: Change it to creation date instead of modification date.
        // TODO : complete the needed functions

        case SortByCreationDate:
        {
            return compareByOrder((left.ctime.isValid()  ? left.ctime  : left.dtime),
                                  (right.ctime.isValid() ? right.ctime : right.dtime),
                                  currentSortOrder);
        }

        default:
        {
            return 1;
        }
    }
}

bool ShowfotoItemSortSettings::lessThan(const QVariant& left, const QVariant& right) const
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    if (left.typeId() != right.typeId())

#else

    if (left.type() != right.type())

#endif

    {
        return false;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    switch (left.typeId())

#else

    switch (left.type())

#endif

    {
        case QVariant::Int:
        {
            return compareByOrder(left.toInt(), right.toInt(), currentSortOrder);
        }

        case QVariant::UInt:
        {
            return compareByOrder(left.toUInt(), right.toUInt(), currentSortOrder);
        }

        case QVariant::LongLong:
        {
            return compareByOrder(left.toLongLong(), right.toLongLong(), currentSortOrder);
        }

        case QVariant::ULongLong:
        {
            return compareByOrder(left.toULongLong(), right.toULongLong(), currentSortOrder);
        }

        case QVariant::Double:
        {
            return compareByOrder(left.toDouble(), right.toDouble(), currentSortOrder);
        }
/*
        case QVariant::Date:
        {
            return compareByOrder(left.toDate(), right.toDate(), currentSortOrder);
        }

        case QVariant::DateTime:
        {
            return compareByOrder(left.toDateTime(), right.toDateTime(), currentSortOrder);
        }

        case QVariant::Time:
        {
            return compareByOrder(left.toTime(), right.toTime(), currentSortOrder);
        }
*/
        case QVariant::Rect:
        case QVariant::RectF:
        {
            QRectF rectLeft  = left.toRectF();
            QRectF rectRight = right.toRectF();
            int result;

            if ((result = compareByOrder(rectLeft.top(), rectRight.top(), currentSortOrder)) != 0)
            {
                return (result < 0);
            }

            if ((result = compareByOrder(rectLeft.left(), rectRight.left(), currentSortOrder)) != 0)
            {
                return (result < 0);
            }

            QSizeF sizeLeft  = rectLeft.size();
            QSizeF sizeRight = rectRight.size();

            if ((result = compareByOrder(sizeLeft.width()*sizeLeft.height(), sizeRight.width()*sizeRight.height(), currentSortOrder)) != 0)
            {
                return (result < 0);
            }

#if __GNUC__ >= 7   // krazy:exclude=cpp

            [[fallthrough]];

#endif
        }

        default:
        {
            return naturalCompare(left.toString(), right.toString(), currentSortOrder, sortCaseSensitivity);
        }
    }
}

} // namespace Showfoto
