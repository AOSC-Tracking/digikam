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

#pragma once

// Qt includes

#include <QString>

// Local includes

#include "GeoTagWriter.h"

namespace Marble
{

// No registration for this writer, ColorStyle is an abstract kml element
class KmlFeatureTagWriter: public GeoTagWriter
{
public:

    explicit KmlFeatureTagWriter(const QString& elementName);

    bool write(const GeoNode* node, GeoWriter& writer) const override;

protected:

    virtual bool writeMid(const GeoNode* node, GeoWriter& writer) const = 0;

private:

    QString const m_elementName;
};

} // namespace Marble
