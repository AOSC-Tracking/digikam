/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-15
 * Description : Anti-vignetting batch tool.
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "batchtool.h"
#include "antivignettingsettings.h"

using namespace Digikam;

namespace DigikamBqmAntiVignettingPlugin
{

class AntiVignetting : public BatchTool
{
    Q_OBJECT

public:

    explicit AntiVignetting(QObject* const parent = nullptr);
    ~AntiVignetting()                                       override = default;

    BatchToolSettings defaultSettings()                     override;

    BatchTool* clone(QObject* const parent = nullptr) const override;

    void registerSettingsWidget()                           override;

private:

    bool toolOperations()                                   override;

private Q_SLOTS:

    void slotAssignSettings2Widget()                        override;
    void slotSettingsChanged()                              override;

private:

    AntiVignettingSettings* m_settingsView = nullptr;
};

} // namespace DigikamBqmAntiVignettingPlugin
