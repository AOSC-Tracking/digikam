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

#include "KmldrawOrderTagHandler.h"

// Local includes

#include "KmlElementDictionary.h"
#include "GeoDataOverlay.h"
#include "GeoParser.h"

namespace Marble
{

namespace kml
{

KML_DEFINE_TAG_HANDLER(drawOrder)

GeoNode* KmldrawOrderTagHandler::parse(GeoParser& parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1String(kmlTag_drawOrder)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataOverlay>())
    {
        bool ok = false;
        int value = parser.readElementText().trimmed().toInt(&ok, 10);

        if (ok)
        {
            parentItem.nodeAs<GeoDataOverlay>()->setDrawOrder(value);
        }
    }

    return nullptr;
}

} // namespace kml

} // namespace Marble
