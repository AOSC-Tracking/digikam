/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-12-27
 * Description : a tool to reduce lens distortions to an image.
 *
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "lensdistortiontool.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QBrush>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QIcon>

// KDE includes

#include <ksharedconfig.h>
#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "dnuminput.h"
#include "editortoolsettings.h"
#include "imageiface.h"
#include "imageguidewidget.h"
#include "lensdistortionfilter.h"

namespace DigikamEditorLensDistortionToolPlugin
{

class Q_DECL_HIDDEN LensDistortionTool::Private
{
public:

    Private() = default;

    const QString configGroupName               = QLatin1String("lensdistortion Tool");
    const QString config2ndOrderDistortionEntry = QLatin1String("2nd Order Distortion");
    const QString config4thOrderDistortionEntry = QLatin1String("4th Order Distortion");
    const QString configZoomFactorEntry         = QLatin1String("Zoom Factor");
    const QString configBrightenEntry           = QLatin1String("Brighten");

    QLabel*              maskPreviewLabel       = nullptr;

    DDoubleNumInput*     mainInput              = nullptr;
    DDoubleNumInput*     edgeInput              = nullptr;
    DDoubleNumInput*     rescaleInput           = nullptr;
    DDoubleNumInput*     brightenInput          = nullptr;

    DImg                 previewRasterImage;

    ImageGuideWidget*    previewWidget          = nullptr;
    EditorToolSettings*  gboxSettings           = nullptr;
};

// --------------------------------------------------------

LensDistortionTool::LensDistortionTool(QObject* const parent)
    : EditorToolThreaded(parent),
      d                 (new Private)
{
    setObjectName(QLatin1String("lensdistortion"));

    d->previewWidget = new ImageGuideWidget(nullptr, true, ImageGuideWidget::HVGuideMode);
    setToolView(d->previewWidget);

    // -------------------------------------------------------------

    d->gboxSettings = new EditorToolSettings(nullptr);
    d->gboxSettings->setTools(EditorToolSettings::ColorGuide);

    QGridLayout* const gridSettings = new QGridLayout(d->gboxSettings->plainPage());

    d->maskPreviewLabel = new QLabel(d->gboxSettings->plainPage());
    d->maskPreviewLabel->setAlignment ( Qt::AlignHCenter | Qt::AlignVCenter );
    d->maskPreviewLabel->setWhatsThis( i18n("You can see here a thumbnail preview of the "
                                            "distortion correction applied to a cross pattern.") );

    // -------------------------------------------------------------

    QLabel* const label1 = new QLabel(i18nc("value for amount of distortion", "Main:"), d->gboxSettings->plainPage());

    d->mainInput = new DDoubleNumInput(d->gboxSettings->plainPage());
    d->mainInput->setDecimals(1);
    d->mainInput->setRange(-100.0, 100.0, 0.1);
    d->mainInput->setDefaultValue(0.0);
    d->mainInput->setWhatsThis( i18n("This value controls the amount of distortion. Negative values "
                                     "correct lens barrel distortion, while positive values correct lens "
                                     "pincushion distortion."));

    // -------------------------------------------------------------

    QLabel* const label2 = new QLabel(i18n("Edge:"), d->gboxSettings->plainPage());

    d->edgeInput = new DDoubleNumInput(d->gboxSettings->plainPage());
    d->edgeInput->setDecimals(1);
    d->edgeInput->setRange(-100.0, 100.0, 0.1);
    d->edgeInput->setDefaultValue(0.0);
    d->edgeInput->setWhatsThis( i18n("This value controls in the same manner as the Main control, "
                                     "but has more effect at the edges of the image than at the center."));

    // -------------------------------------------------------------

    QLabel* const label3 = new QLabel(i18n("Zoom:"), d->gboxSettings->plainPage());

    d->rescaleInput = new DDoubleNumInput(d->gboxSettings->plainPage());
    d->rescaleInput->setDecimals(1);
    d->rescaleInput->setRange(-100.0, 100.0, 0.1);
    d->rescaleInput->setDefaultValue(0.0);
    d->rescaleInput->setWhatsThis( i18n("This value rescales the overall image size."));

    // -------------------------------------------------------------

    QLabel* const label4 = new QLabel(i18n("Brighten:"), d->gboxSettings->plainPage());

    d->brightenInput = new DDoubleNumInput(d->gboxSettings->plainPage());
    d->brightenInput->setDecimals(1);
    d->brightenInput->setRange(-100.0, 100.0, 0.1);
    d->brightenInput->setDefaultValue(0.0);
    d->brightenInput->setWhatsThis( i18n("This value adjusts the brightness in image corners."));

    // -------------------------------------------------------------

    const int spacing = d->gboxSettings->spacingHint();

    gridSettings->addWidget(d->maskPreviewLabel, 0, 0, 1, 2);
    gridSettings->addWidget(label1,              1, 0, 1, 2);
    gridSettings->addWidget(d->mainInput,        2, 0, 1, 2);
    gridSettings->addWidget(label2,              3, 0, 1, 2);
    gridSettings->addWidget(d->edgeInput,        4, 0, 1, 2);
    gridSettings->addWidget(label3,              5, 0, 1, 2);
    gridSettings->addWidget(d->rescaleInput,     6, 0, 1, 2);
    gridSettings->addWidget(label4,              7, 0, 1, 2);
    gridSettings->addWidget(d->brightenInput,    8, 0, 1, 2);
    gridSettings->setRowStretch(9, 10);
    gridSettings->setContentsMargins(spacing, spacing, spacing, spacing);
    gridSettings->setSpacing(spacing);

    setToolSettings(d->gboxSettings);

    // -------------------------------------------------------------

    connect(d->mainInput, SIGNAL(valueChanged(double)),
            this, SLOT(slotTimer()));

    connect(d->edgeInput, SIGNAL(valueChanged(double)),
            this, SLOT(slotTimer()));

    connect(d->rescaleInput, SIGNAL(valueChanged(double)),
            this, SLOT(slotTimer()));

    connect(d->brightenInput, SIGNAL(valueChanged(double)),
            this, SLOT(slotTimer()));

    connect(d->gboxSettings, SIGNAL(signalColorGuideChanged()),
            this, SLOT(slotColorGuideChanged()));

    // -------------------------------------------------------------

    /* Calc transform preview.
       We would like a checkered area to demonstrate the effect.
       We do not have any drawing support in DImg, so we let Qt draw.
       First we create a white QImage. We convert this to a QPixmap,
       on which we can draw. Then we convert back to QImage,
       convert the QImage to a DImg which we only need to create once, here.
       Later, we apply the effect on a copy and convert the DImg to QPixmap.
       Longing for Qt4 where we can paint directly on the QImage...
    */

    QPixmap pix(120, 120);
    pix.fill(Qt::white);
    QPainter pt(&pix);
    pt.setPen(QPen(Qt::black, 1));
    pt.fillRect(0, 0, pix.width(), pix.height(), QBrush(Qt::black, Qt::CrossPattern));
    pt.drawRect(0, 0, pix.width(), pix.height());
    pt.end();
    QImage preview       = pix.toImage();
    d->previewRasterImage = DImg(preview.width(), preview.height(), false, false, preview.bits());
}

LensDistortionTool::~LensDistortionTool()
{
    delete d;
}

void LensDistortionTool::slotColorGuideChanged()
{
    d->previewWidget->slotChangeGuideColor(d->gboxSettings->guideColor());
    d->previewWidget->slotChangeGuideSize(d->gboxSettings->guideSize());
}

void LensDistortionTool::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    blockWidgetSignals(true);

    d->mainInput->setValue(group.readEntry(d->config2ndOrderDistortionEntry, d->mainInput->defaultValue()));
    d->edgeInput->setValue(group.readEntry(d->config4thOrderDistortionEntry, d->edgeInput->defaultValue()));
    d->rescaleInput->setValue(group.readEntry(d->configZoomFactorEntry,      d->rescaleInput->defaultValue()));
    d->brightenInput->setValue(group.readEntry(d->configBrightenEntry,       d->brightenInput->defaultValue()));

    blockWidgetSignals(false);

    slotColorGuideChanged();
    slotPreview();
}

void LensDistortionTool::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    group.writeEntry(d->config2ndOrderDistortionEntry, d->mainInput->value());
    group.writeEntry(d->config4thOrderDistortionEntry, d->edgeInput->value());
    group.writeEntry(d->configZoomFactorEntry,         d->rescaleInput->value());
    group.writeEntry(d->configBrightenEntry,           d->brightenInput->value());

    config->sync();
}

void LensDistortionTool::slotResetSettings()
{
    blockWidgetSignals(true);

    d->mainInput->slotReset();
    d->edgeInput->slotReset();
    d->rescaleInput->slotReset();
    d->brightenInput->slotReset();

    blockWidgetSignals(false);

    slotPreview();
}

void LensDistortionTool::preparePreview()
{
    double m = d->mainInput->value();
    double e = d->edgeInput->value();
    double r = d->rescaleInput->value();
    double b = d->brightenInput->value();

    LensDistortionFilter transformPreview(&d->previewRasterImage, nullptr, m, e, r, b, 0, 0);
    transformPreview.startFilterDirectly();
    d->maskPreviewLabel->setPixmap(transformPreview.getTargetImage().convertToPixmap());

    ImageIface* const iface = d->previewWidget->imageIface();

    setFilter(new LensDistortionFilter(iface->original(), this, m, e, r, b, 0, 0));
}

void LensDistortionTool::prepareFinal()
{
    double m = d->mainInput->value();
    double e = d->edgeInput->value();
    double r = d->rescaleInput->value();
    double b = d->brightenInput->value();

    ImageIface iface;
    setFilter(new LensDistortionFilter(iface.original(), this, m, e, r, b, 0, 0));
}

void LensDistortionTool::setPreviewImage()
{
    ImageIface* const iface = d->previewWidget->imageIface();
    DImg imDest             = filter()->getTargetImage().smoothScale(iface->previewSize());
    iface->setPreview(imDest);

    d->previewWidget->updatePreview();
}

void LensDistortionTool::setFinalImage()
{
    ImageIface iface;
    iface.setOriginal(i18n("Lens Distortion"), filter()->filterAction(), filter()->getTargetImage());
}

void LensDistortionTool::blockWidgetSignals(bool b)
{
    d->mainInput->blockSignals(b);
    d->edgeInput->blockSignals(b);
    d->rescaleInput->blockSignals(b);
    d->brightenInput->blockSignals(b);
}

} // namespace DigikamEditorLensDistortionToolPlugin

#include "moc_lensdistortiontool.cpp"
