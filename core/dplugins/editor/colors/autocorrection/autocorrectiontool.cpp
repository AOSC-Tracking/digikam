/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-31
 * Description : Auto-Color correction tool.
 *
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autocorrectiontool.h"

// Qt includes

#include <QGridLayout>
#include <QListWidget>
#include <QIcon>

// KDE includes

#include <ksharedconfig.h>
#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "autolevelsfilter.h"
#include "equalizefilter.h"
#include "stretchfilter.h"
#include "normalizefilter.h"
#include "autoexpofilter.h"
#include "editortoolsettings.h"
#include "histogramwidget.h"
#include "imageiface.h"
#include "imageregionwidget.h"
#include "previewlist.h"

namespace DigikamEditorAutoCorrectionToolPlugin
{

class Q_DECL_HIDDEN AutoCorrectionTool::Private
{
public:

    enum AutoCorrectionType
    {
        AutoLevelsCorrection = 0,
        NormalizeCorrection,
        EqualizeCorrection,
        StretchContrastCorrection,
        AutoExposureCorrection
    };

public:

    Private() = default;

    const QString configGroupName                   = QLatin1String("autocorrection Tool");
    const QString configHistogramChannelEntry       = QLatin1String("Histogram Channel");
    const QString configHistogramScaleEntry         = QLatin1String("Histogram Scale");
    const QString configAutoCorrectionFilterEntry   = QLatin1String("Auto Correction Filter");

    PreviewList*         correctionTools            = nullptr;

    ImageRegionWidget*   previewWidget              = nullptr;
    EditorToolSettings*  gboxSettings               = nullptr;
};

// --------------------------------------------------------

AutoCorrectionTool::AutoCorrectionTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d                 (new Private)
{
    setObjectName(QLatin1String("autocorrection"));
    setToolVersion(1);
    setToolHelp(QLatin1String("autocolorcorrectiontool.anchor"));
    setToolCategory(FilterAction::ReproducibleFilter);

    // -------------------------------------------------------------

    d->previewWidget = new ImageRegionWidget;
    setToolView(d->previewWidget);
    setPreviewModeMask(PreviewToolBar::AllPreviewModes);

    // -------------------------------------------------------------

    ImageIface iface;
    DImg thumbImage       = iface.original()->smoothScale(128, 128, Qt::KeepAspectRatio);
    PreviewListItem* item = nullptr;
    (void)item; // to prevent cppcheck warnings.
    d->gboxSettings       = new EditorToolSettings(nullptr);
    d->gboxSettings->setTools(EditorToolSettings::Histogram);
    d->gboxSettings->setHistogramType(LRGBC);
    d->gboxSettings->setButtons(EditorToolSettings::Default|
                                EditorToolSettings::Ok|
                                EditorToolSettings::Cancel);
    //                                EditorToolSettings::Try);

    // -------------------------------------------------------------

    d->correctionTools = new PreviewList;

    item = d->correctionTools->addItem(new AutoLevelsFilter(&thumbImage, iface.original()),
                                       i18n("Auto Levels"), Private::AutoLevelsCorrection);
    item->setWhatsThis(i18n("<b>Auto Levels</b>:"
                            "<p>This option maximizes the tonal range in the Red, "
                            "Green, and Blue channels. It searches the image shadow and highlight "
                            "limit values and adjusts the Red, Green, and Blue channels "
                            "to a full histogram range.</p>"));

    item = d->correctionTools->addItem(new NormalizeFilter(&thumbImage, iface.original()),
                                       i18n("Normalize"), Private::NormalizeCorrection);
    item->setWhatsThis(i18n("<b>Normalize</b>:"
                            "<p>This option scales brightness values across the active "
                            "image so that the darkest point becomes black, and the "
                            "brightest point becomes as bright as possible without "
                            "altering its hue. This is often a \"magic fix\" for "
                            "images that are dim or washed out.</p>"));


    item = d->correctionTools->addItem(new EqualizeFilter(&thumbImage, iface.original()),
                                       i18n("Equalize"), Private::EqualizeCorrection);
    item->setWhatsThis(i18n("<b>Equalize</b>:"
                            "<p>This option adjusts the brightness of colors across the "
                            "active image so that the histogram for the value channel "
                            "is as nearly as possible flat, that is, so that each possible "
                            "brightness value appears at about the same number of pixels "
                            "as each other value. Sometimes Equalize works wonderfully at "
                            "enhancing the contrasts in an image. Other times it gives "
                            "garbage. It is a very powerful operation, which can either work "
                            "miracles on an image or destroy it.</p>"));

    item = d->correctionTools->addItem(new StretchFilter(&thumbImage, iface.original()),
                                       i18n("Stretch Contrast"), Private::StretchContrastCorrection);
    item->setWhatsThis(i18n("<b>Stretch Contrast</b>:"
                            "<p>This option enhances the contrast and brightness "
                            "of the RGB values of an image by stretching the lowest "
                            "and highest values to their fullest range, adjusting "
                            "everything in between.</p>"));

    item = d->correctionTools->addItem(new AutoExpoFilter(&thumbImage, iface.original()),
                                       i18n("Auto Exposure"), Private::AutoExposureCorrection);
    item->setWhatsThis(i18n("<b>Auto Exposure</b>:"
                            "<p>This option enhances the contrast and brightness "
                            "of the RGB values of an image to calculate optimal "
                            "exposition and black level using image histogram "
                            "properties.</p>"));

    d->correctionTools->setFocus();

    // -------------------------------------------------------------

    QGridLayout* const mainLayout = new QGridLayout();
    mainLayout->addWidget(d->correctionTools, 0, 0, 1, 1);
    mainLayout->setRowStretch(0, 10);
    mainLayout->setContentsMargins(QMargins());
    mainLayout->setSpacing(d->gboxSettings->spacingHint());
    d->gboxSettings->plainPage()->setLayout(mainLayout);

    // -------------------------------------------------------------

    setToolSettings(d->gboxSettings);

    // -------------------------------------------------------------

    connect(d->correctionTools, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotPreview()));
}

AutoCorrectionTool::~AutoCorrectionTool()
{
    delete d->correctionTools;
    delete d;
}

void AutoCorrectionTool::slotInit()
{
    EditorToolThreaded::slotInit();
    d->correctionTools->startFilters();
}

void AutoCorrectionTool::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    d->gboxSettings->histogramBox()->setChannel((ChannelType)group.readEntry(d->configHistogramChannelEntry, (int)LuminosityChannel));
    d->gboxSettings->histogramBox()->setScale((HistogramScale)group.readEntry(d->configHistogramScaleEntry, (int)LogScaleHistogram));
    d->correctionTools->setCurrentId(group.readEntry(d->configAutoCorrectionFilterEntry, (int)Private::AutoLevelsCorrection));
}

void AutoCorrectionTool::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    group.writeEntry(d->configHistogramChannelEntry,     (int)d->gboxSettings->histogramBox()->channel());
    group.writeEntry(d->configHistogramScaleEntry,       (int)d->gboxSettings->histogramBox()->scale());
    group.writeEntry(d->configAutoCorrectionFilterEntry, d->correctionTools->currentId());
    config->sync();
}

void AutoCorrectionTool::slotResetSettings()
{
    d->correctionTools->blockSignals(true);
    d->correctionTools->setCurrentId(Private::AutoLevelsCorrection);
    d->correctionTools->blockSignals(false);

    slotPreview();
}

void AutoCorrectionTool::preparePreview()
{
    d->gboxSettings->histogramBox()->histogram()->stopHistogramComputation();

    ImageIface iface;
    DImg preview = d->previewWidget->getOriginalRegionImage(true);

    autoCorrection(&preview, iface.original(), d->correctionTools->currentId());
}

void AutoCorrectionTool::setPreviewImage()
{
    DImg preview = filter()->getTargetImage();
    d->previewWidget->setPreviewImage(preview);

    // Update histogram.

    d->gboxSettings->histogramBox()->histogram()->updateData(preview.copy(), DImg(), false);
}

void AutoCorrectionTool::prepareFinal()
{
    int type = d->correctionTools->currentId();
    ImageIface iface;
    autoCorrection(iface.original(), iface.original(), type);
}

void AutoCorrectionTool::setFinalImage()
{
    int type = d->correctionTools->currentId();
    QString name;

    switch (type)
    {
        case Private::AutoLevelsCorrection:
        {
            name = i18n("Auto Levels");
            break;
        }

        case Private::NormalizeCorrection:
        {
            name = i18n("Normalize");
            break;
        }

        case Private::EqualizeCorrection:
        {
            name = i18n("Equalize");
            break;
        }

        case Private::StretchContrastCorrection:
        {
            name = i18n("Stretch Contrast");
            break;
        }

        case Private::AutoExposureCorrection:
        {
            name = i18n("Auto Exposure");
            break;
        }
    }

    ImageIface iface;
    iface.setOriginal(name, filter()->filterAction(), filter()->getTargetImage());
}

void AutoCorrectionTool::autoCorrection(DImg* const img, DImg* const ref, int type)
{
    switch (type)
    {
        case Private::AutoLevelsCorrection:
        {
            setFilter(new AutoLevelsFilter(img, ref, this));
            break;
        }

        case Private::NormalizeCorrection:
        {
            setFilter(new NormalizeFilter(img, ref, this));
            break;
        }

        case Private::EqualizeCorrection:
        {
            setFilter(new EqualizeFilter(img, ref, this));
            break;
        }

        case Private::StretchContrastCorrection:
        {
            setFilter(new StretchFilter(img, ref, this));
            break;
        }

        case Private::AutoExposureCorrection:
        {
            setFilter(new AutoExpoFilter(img, ref, this));
            break;
        }
    }
}

} // namespace DigikamEditorAutoCorrectionToolPlugin

#include "moc_autocorrectiontool.cpp"
