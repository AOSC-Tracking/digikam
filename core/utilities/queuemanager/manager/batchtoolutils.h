/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-24
 * Description : Batch Tool utils.
 *
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>
#include <QVariant>
#include <QMap>
#include <QList>
#include <QUrl>

// Local includes

#include "batchtool.h"

namespace Digikam
{

/**
 * A list of batch tool instances.
 */
typedef QList<BatchTool*> BatchToolsList;

// -------------------------------------------------------------------------------------------------------------

/**
 * A container of associated batch tool and settings.
 */
class BatchToolSet
{
public:

    BatchToolSet()  = default;
    ~BatchToolSet() = default;

    /**
     * Equality operator which check index, version, name, and group data. Settings member is ignored.
     */
    bool operator==(const BatchToolSet& set) const;

    /// Tool identifier data. Index is tool ID from assigned list.
    int                       index     = -1;
    int                       version   = 0;
    QString                   name;
    BatchTool::BatchToolGroup group     = BatchTool::BaseTool;

    /// Settings hosted in this container.
    BatchToolSettings         settings;
};

//! qDebug() stream operator. Writes property @a t to the debug output in a nicely formatted way.
QDebug operator<<(QDebug dbg, const BatchToolSet& s);

// -------------------------------------------------------------------------------------------------------------

/**
 * An indexed map of batch tools with settings.
 */
typedef QList<BatchToolSet> BatchSetList;

// -------------------------------------------------------------------------------------------------------------

/**
 * Container to assign Batch tools and settings to an item by Url.
 * Url is used only with ActionThread class.
 */
class AssignedBatchTools
{
public:

    AssignedBatchTools()  = default;
    ~AssignedBatchTools() = default;

    QString targetSuffix(bool* const extSet = nullptr) const;

public:

    QString      m_destFileName;
    QUrl         m_itemUrl;
    BatchSetList m_toolsList;
};

} // namespace Digikam
