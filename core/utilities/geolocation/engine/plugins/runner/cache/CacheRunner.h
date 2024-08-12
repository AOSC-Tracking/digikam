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

// Local includes

#include "ParsingRunner.h"

namespace Marble
{

class CacheRunner : public ParsingRunner
{
    Q_OBJECT

public:

    explicit CacheRunner(QObject* const parent = nullptr);
    ~CacheRunner() override;

    GeoDataDocument* parseFile(const QString& fileName, DocumentRole role, QString& error) override;
};

} // namespace Marble