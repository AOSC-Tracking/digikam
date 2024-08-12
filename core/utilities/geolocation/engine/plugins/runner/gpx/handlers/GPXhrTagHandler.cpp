/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "GPXhrTagHandler.h"

// Qt includes

#include <QVariant>

// Local includes

#include "GPXElementDictionary.h"
#include "GeoParser.h"
#include "GeoDataExtendedData.h"
#include "GeoDataTrack.h"
#include "GeoDataSimpleArrayData.h"
#include "digikam_debug.h"

namespace Marble
{

namespace gpx
{

GPX_DEFINE_TAG_HANDLER_GARMIN_TRACKPOINTEXT1(hr)

GeoNode* GPXhrTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(gpxTag_hr)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataTrack>())
    {
        GeoDataSimpleArrayData* arrayData = parentItem.nodeAs<GeoDataTrack>()
                                            ->extendedData().simpleArrayData(QStringLiteral("heartrate"));

        if (!arrayData)
        {
            arrayData = new GeoDataSimpleArrayData();
            QString name = parser.attribute("name").trimmed();
            parentItem.nodeAs<GeoDataTrack>()->extendedData().setSimpleArrayData(QStringLiteral("heartrate"), arrayData);
        }

        QVariant value(parser.readElementText().toInt());
        arrayData->append(value);
        return nullptr;
    }

    return nullptr;
}

} // namespace gpx

} // namespace Marble
