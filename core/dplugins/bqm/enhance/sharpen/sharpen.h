/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-05-02
 * Description : sharpen image batch tool.
 *
 * SPDX-FileCopyrightText: 2009      by Matthias Welwarsky <matze at welwarsky dot de>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "batchtool.h"
#include "sharpsettings.h"

using namespace Digikam;

namespace DigikamBqmSharpenPlugin
{

class Sharpen : public BatchTool
{
    Q_OBJECT

public:

    explicit Sharpen(QObject* const parent = nullptr);
    ~Sharpen()                                              override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    SharpSettings* m_settingsView = nullptr;
};

} // namespace DigikamBqmSharpenPlugin
