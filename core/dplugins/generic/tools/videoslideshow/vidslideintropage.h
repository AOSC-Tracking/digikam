/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericVideoSlideShowPlugin
{

class VidSlideIntroPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit VidSlideIntroPage(QWizard* const dialog, const QString& title);
    ~VidSlideIntroPage()    override;

    void initializePage()   override;
    bool validatePage()     override;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericVideoSlideShowPlugin