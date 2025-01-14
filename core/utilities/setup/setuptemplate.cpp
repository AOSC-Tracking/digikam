/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-07-04
 * Description : metadata template setup page.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setuptemplate.h"

// Qt includes

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "templatelist.h"
#include "templatepanel.h"
#include "altlangstredit.h"
#include "dtextedit.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupTemplate::Private
{
public:

    Private() = default;

    QPushButton*   addButton    = nullptr;
    QPushButton*   delButton    = nullptr;
    QPushButton*   repButton    = nullptr;

    DTextEdit*     titleEdit    = nullptr;

    TemplateList*  listView     = nullptr;

    TemplatePanel* tview        = nullptr;
};

SetupTemplate::SetupTemplate(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    QWidget* const panel = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    const int spacing    = layoutSpacing();

    d->listView          = new TemplateList(panel);
    d->listView->setFixedHeight(100);

    // --------------------------------------------------------

    QLabel* const label0 = new QLabel(i18n("Template Title:"), panel);
    d->titleEdit         = new DTextEdit(panel);
    d->titleEdit->setLinesVisible(1);
    d->titleEdit->setPlaceholderText(i18n("Enter the metadata template title here."));
    d->titleEdit->setWhatsThis(i18n("<p>Enter the metadata template title here. This title will be "
                                    "used to identify a template in your collection.</p>"));
    label0->setBuddy(d->titleEdit);

    // --------------------------------------------------------

    d->tview             = new TemplatePanel(panel);

    // --------------------------------------------------------

    QLabel* const note = new QLabel(i18n("<b>Note: These information are used to set "
                                   "<b><a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a></b> "
                                   "and <b><a href='https://en.wikipedia.org/wiki/IPTC'>IPTC</a></b> tag contents. "
                                   "There is no limitation with XMP, but note that IPTC text tag sizes are limited. "
                                   "Use contextual help for details.</b>"), panel);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // -------------------------------------------------------------

    d->addButton = new QPushButton(panel);
    d->delButton = new QPushButton(panel);
    d->repButton = new QPushButton(panel);

    d->addButton->setText(i18n("&Add..."));
    d->addButton->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->delButton->setText(i18n("&Remove"));
    d->delButton->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    d->repButton->setText(i18n("&Replace..."));
    d->repButton->setIcon(QIcon::fromTheme(QLatin1String("view-refresh")));
    d->delButton->setEnabled(false);
    d->repButton->setEnabled(false);

    // -------------------------------------------------------------

    QGridLayout* const grid = new QGridLayout;
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);
    grid->setAlignment(Qt::AlignTop);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(4, 10);
    grid->addWidget(d->listView,  0, 0, 4, 2);
    grid->addWidget(d->addButton, 0, 2, 1, 1);
    grid->addWidget(d->delButton, 1, 2, 1, 1);
    grid->addWidget(d->repButton, 2, 2, 1, 1);
    grid->addWidget(label0,       4, 0, 1, 1);
    grid->addWidget(d->titleEdit, 4, 1, 1, 1);
    grid->addWidget(d->tview,     5, 0, 1, 3);
    grid->addWidget(note,         6, 0, 1, 3);
    panel->setLayout(grid);

    panel->setTabOrder(d->listView,  d->addButton);
    panel->setTabOrder(d->addButton, d->delButton);
    panel->setTabOrder(d->delButton, d->repButton);
    panel->setTabOrder(d->repButton, d->titleEdit);
    panel->setTabOrder(d->titleEdit, d->tview);

    // --------------------------------------------------------

    connect(d->listView, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSelectionChanged()));

    connect(d->addButton, SIGNAL(clicked()),
            this, SLOT(slotAddTemplate()));

    connect(d->delButton, SIGNAL(clicked()),
            this, SLOT(slotDelTemplate()));

    connect(d->repButton, SIGNAL(clicked()),
            this, SLOT(slotRepTemplate()));

    // --------------------------------------------------------

    readSettings();
    d->titleEdit->setFocus();

    d->listView->sortItems(0, Qt::AscendingOrder);
    d->listView->setSortingEnabled(true);
}

SetupTemplate::~SetupTemplate()
{
    delete d;
}

void SetupTemplate::applySettings()
{
    QString title = d->titleEdit->text();

    if (!title.isEmpty())
    {
        TemplateListItem* const item = dynamic_cast<TemplateListItem*>(d->listView->currentItem());

        if (item)
        {
            d->tview->apply();

            Template t = d->tview->getTemplate();
            t.setTemplateTitle(title);
            item->setTemplate(t);
        }
    }

    d->listView->applySettings();

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));
    group.writeEntry(QLatin1String("Template Tab"), (int)(d->tview->currentIndex()));
    config->sync();
}

void SetupTemplate::readSettings()
{
    d->listView->readSettings();

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("Setup Dialog"));
    d->tview->setCurrentIndex((TemplatePanel::TemplateTab)group.readEntry(QLatin1String("Template Tab"), (int)TemplatePanel::RIGHTS));
}

void SetupTemplate::setTemplate(const Template& t)
{
    if (!t.isNull())
    {
        TemplateListItem* const item = d->listView->find(t.templateTitle());
        d->listView->setCurrentItem(item);

        return;
    }

    populateTemplate(t);
}

void SetupTemplate::slotSelectionChanged()
{
    TemplateListItem* const item = dynamic_cast<TemplateListItem*>(d->listView->currentItem());

    if (!item)
    {
        d->delButton->setEnabled(false);
        d->repButton->setEnabled(false);

        return;
    }

    d->delButton->setEnabled(true);
    d->repButton->setEnabled(true);
    populateTemplate(item->getTemplate());
}

void SetupTemplate::populateTemplate(const Template& t)
{
    d->tview->setTemplate(t);
    d->titleEdit->setText(t.templateTitle());
    d->titleEdit->setFocus();
}

void SetupTemplate::slotAddTemplate()
{
    QString title = d->titleEdit->text();

    if (title.isEmpty())
    {
        QMessageBox::critical(this, qApp->applicationName(), i18n("Cannot register new metadata template without title."));

        return;
    }

    if (d->listView->find(title))
    {
        QMessageBox::critical(this, qApp->applicationName(), i18n("A metadata template named '%1' already exists.", title));

        return;
    }

    d->tview->apply();

    Template t = d->tview->getTemplate();
    t.setTemplateTitle(d->titleEdit->text());
    new TemplateListItem(d->listView, t);
}

void SetupTemplate::slotDelTemplate()
{
    TemplateListItem* const item = dynamic_cast<TemplateListItem*>(d->listView->currentItem());
    delete item;
}

void SetupTemplate::slotRepTemplate()
{
    QString title = d->titleEdit->text();

    if (title.isEmpty())
    {
        QMessageBox::critical(this, qApp->applicationName(), i18n("Cannot register new metadata template without title."));

        return;
    }

    TemplateListItem* const item = dynamic_cast<TemplateListItem*>(d->listView->currentItem());

    if (!item)
    {
        return;
    }

    d->tview->apply();

    Template t = d->tview->getTemplate();
    t.setTemplateTitle(title);
    item->setTemplate(t);
}

} // namespace Digikam

#include "moc_setuptemplate.cpp"
