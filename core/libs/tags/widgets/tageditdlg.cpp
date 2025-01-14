/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-07-01
 * Description : dialog to edit and create digiKam Tags
 *
 * SPDX-FileCopyrightText: 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tageditdlg.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QPointer>
#include <QTreeWidget>
#include <QApplication>
#include <QStyle>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRegularExpression>

// KDE includes

#include <klocalizedstring.h>
#include <kkeysequencewidget.h>

#ifdef HAVE_KICONTHEMES
#   include <kicondialog.h>
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "album.h"
#include "syncjob.h"
#include "searchtextbar.h"
#include "tagsactionmngr.h"
#include "coredbconstants.h"
#include "dxmlguiwindow.h"
#include "dexpanderbox.h"
#include "dlayoutbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN TagsListCreationErrorDialog : public QDialog
{
    Q_OBJECT

public:

    TagsListCreationErrorDialog(QWidget* const parent, const QMap<QString, QString>& errMap);
    ~TagsListCreationErrorDialog() override {};
};

// ------------------------------------------------------------------------------

class Q_DECL_HIDDEN TagEditDlg::Private
{
public:

    Private() = default;

    bool                create          = false;

    QLabel*             topLabel        = nullptr;

    QString             icon;

    QPushButton*        iconButton      = nullptr;
    QPushButton*        resetIconButton = nullptr;

    QDialogButtonBox*   buttons         = nullptr;

    KKeySequenceWidget* keySeqWidget    = nullptr;

    TAlbum*             mainRootAlbum   = nullptr;
    SearchTextBar*      titleEdit       = nullptr;
};

TagEditDlg::TagEditDlg(QWidget* const parent, TAlbum* const album, bool create)
    : QDialog(parent),
      d      (new Private)
{
    setModal(true);

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    if (create)
    {
        setWindowTitle(i18nc("@title:window", "New Tag"));
    }
    else
    {
        setWindowTitle(i18nc("@title:window", "Edit Tag"));
    }

    d->mainRootAlbum    = album;
    d->create           = create;
    QWidget* const page = new QWidget(this);

    // --------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(page);
    QLabel* const logo      = new QLabel(page);
    logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48,48)));

    d->topLabel             = new QLabel(page);
    d->topLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->topLabel->setWordWrap(false);

    DLineWidget* const line = new DLineWidget(Qt::Horizontal, page);

    // --------------------------------------------------------

    QLabel* const titleLabel = new QLabel(page);
    titleLabel->setText(i18nc("@label: tag properties", "&Title:"));

    d->titleEdit             = new SearchTextBar(page, QLatin1String("TagEditDlgTitleEdit"), i18n("Enter tag name here..."));
    d->titleEdit->setCaseSensitive(false);
    titleLabel->setBuddy(d->titleEdit);

    QLabel* const tipLabel   = new QLabel(page);
    tipLabel->setTextFormat(Qt::RichText);
    tipLabel->setWordWrap(true);
    tipLabel->setText(i18n("<p>To create new tags, you can use the following rules:</p>"
                           "<p><ul><li>'/' can be used to create a tags hierarchy.<br/>"
                           "Ex.: <i>\"Country/City/Paris\"</i></li>"
                           "<li>',' can be used to create more than one tags hierarchy at the same time.<br/>"
                           "Ex.: <i>\"City/Paris, Monument/Notre-Dame\"</i></li>"
                           "<li>If a tag hierarchy starts with '/', root tag album is used as parent.</li></ul></p>"
                          ));

    if (d->create)
    {
        QStringList tagPaths;
        AlbumList tList = AlbumManager::instance()->allTAlbums();

        for (AlbumList::const_iterator it = tList.constBegin() ; it != tList.constEnd() ; ++it)
        {
            TAlbum* const tag = static_cast<TAlbum*>(*it);

            if (tag && !tag->isInternalTag())
            {
                tagPaths << tag->tagPath();
            }
        }

        d->titleEdit->completerModel()->setList(tagPaths);
    }
    else
    {
        d->titleEdit->setText(d->mainRootAlbum->title());
        tipLabel->hide();
    }

    // --------------------------------------------------------

    QLabel* const iconTextLabel = new QLabel(page);
    iconTextLabel->setText(i18n("&Icon:"));

    d->iconButton               = new QPushButton(page);
    d->iconButton->setFixedSize(40, 40);
    d->iconButton->setIcon(SyncJob::getTagThumbnail(album));
    iconTextLabel->setBuddy(d->iconButton);

    // In create mode, by default assign the icon of the parent (if not root) to this new tag.

    d->icon = album->icon();

    d->resetIconButton = new QPushButton(QIcon::fromTheme(QLatin1String("view-refresh")), i18n("Reset"), page);

#ifndef HAVE_KICONTHEMES

    iconTextLabel->hide();
    d->iconButton->hide();
    d->resetIconButton->hide();

#endif

    // --------------------------------------------------------

    QLabel* const kscTextLabel = new QLabel(page);
    kscTextLabel->setText(i18n("&Shortcut:"));

    d->keySeqWidget = new KKeySequenceWidget(page);
    kscTextLabel->setBuddy(d->keySeqWidget);

    if (!create)
    {
        QString Seq = album->property(TagPropertyName::tagKeyboardShortcut());
        d->keySeqWidget->setKeySequence(Seq);
    }
    else
    {
        // Do not inherit tag shortcut, it creates a conflict shortcut, see bug 309558.

        d->keySeqWidget->setCheckActionCollections(TagsActionMngr::defaultManager()->actionCollections());
    }

    QLabel* const tipLabel2 = new QLabel(page);
    tipLabel2->setTextFormat(Qt::RichText);
    tipLabel2->setWordWrap(true);
    tipLabel2->setText(i18n("<p><b>Note</b>: this shortcut can be used to assign or unassign tag to items.</p>"));

    // --------------------------------------------------------

    const int spacing = layoutSpacing();
    const int cmargin = layoutMargin();

    grid->addWidget(logo,               0, 0, 1, 1);
    grid->addWidget(d->topLabel,        0, 1, 1, 4);
    grid->addWidget(line,               1, 0, 1, 4);
    grid->addWidget(tipLabel,           2, 0, 1, 4);
    grid->addWidget(titleLabel,         3, 0, 1, 1);
    grid->addWidget(d->titleEdit,       3, 1, 1, 3);
    grid->addWidget(iconTextLabel,      4, 0, 1, 1);
    grid->addWidget(d->iconButton,      4, 1, 1, 1);
    grid->addWidget(d->resetIconButton, 4, 2, 1, 1);
    grid->addWidget(kscTextLabel,       5, 0, 1, 1);
    grid->addWidget(d->keySeqWidget,    5, 1, 1, 3);
    grid->addWidget(tipLabel2,          6, 0, 1, 4);
    grid->setRowStretch(7, 10);
    grid->setColumnStretch(3, 10);
    grid->setContentsMargins(cmargin, cmargin, cmargin, cmargin);
    grid->setSpacing(spacing);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------

    connect(d->iconButton, SIGNAL(clicked()),
            this, SLOT(slotIconChanged()));

    connect(d->resetIconButton, SIGNAL(clicked()),
            this, SLOT(slotIconResetClicked()));

    connect(d->titleEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotTitleChanged(QString)));

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    // --------------------------------------------------------

    slotTitleChanged(d->titleEdit->text());
    d->titleEdit->setFocus();
    adjustSize();
}

TagEditDlg::~TagEditDlg()
{
    delete d;
}

QString TagEditDlg::title() const
{
    return d->titleEdit->text();
}

QString TagEditDlg::icon() const
{
    return d->icon;
}

QKeySequence TagEditDlg::shortcut() const
{
    return d->keySeqWidget->keySequence();
}

void TagEditDlg::slotIconResetClicked()
{
    d->icon = d->mainRootAlbum->standardIconName();
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));
}

void TagEditDlg::slotIconChanged()
{

#ifdef HAVE_KICONTHEMES

    QPointer<KIconDialog> dlg = new KIconDialog(this);
    dlg->setup(KIconLoader::NoGroup, KIconLoader::Application, false, 20, false, false, false);
    QString icon              = dlg->openDialog();
    delete dlg;

    if (icon.isEmpty() || (icon == d->icon))
    {
        return;
    }

    d->icon                   = icon;
    d->iconButton->setIcon(QIcon::fromTheme(d->icon));

#endif

}

void TagEditDlg::slotTitleChanged(const QString& newtitle)
{
    QString tagName = d->mainRootAlbum->tagPath();

    if (tagName.endsWith(QLatin1Char('/')) && !d->mainRootAlbum->isRoot())
    {
        tagName.truncate(tagName.length()-1);
    }

    if (d->create)
    {
        if (d->titleEdit->text().startsWith(QLatin1Char('/')))
        {
            d->topLabel->setText(i18n("<b>Create New Tag</b>"));
        }
        else
        {
            d->topLabel->setText(i18n("<b>Create New Tag in<br/>"
                                      "\"%1\"</b>", tagName));
        }
    }
    else
    {
        d->topLabel->setText(i18n("<b>Properties of Tag<br/>"
                                  "\"%1\"</b>", tagName));
    }

    QRegularExpression emptyTitle(QRegularExpression::anchoredPattern(QLatin1String("^\\s*$")));
    bool enable = (!emptyTitle.match(newtitle).hasMatch() && !newtitle.isEmpty());
    d->buttons->button(QDialogButtonBox::Ok)->setEnabled(enable);
}

bool TagEditDlg::tagEdit(QWidget* const parent, TAlbum* const album, QString& title, QString& icon, QKeySequence& ks)
{
    QPointer<TagEditDlg> dlg = new TagEditDlg(parent, album);
    bool valRet              = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        title = dlg->title();
        icon  = dlg->icon();
        ks    = dlg->shortcut();
    }

    delete dlg;

    return valRet;
}

bool TagEditDlg::tagCreate(QWidget* const parent, TAlbum* const album, QString& title, QString& icon, QKeySequence& ks)
{
    QPointer<TagEditDlg> dlg = new TagEditDlg(parent, album, true);

    bool valRet = dlg->exec();

    if (valRet == QDialog::Accepted)
    {
        title = dlg->title();
        icon  = dlg->icon();
        ks    = dlg->shortcut();
    }

    delete dlg;

    return valRet;
}

AlbumList TagEditDlg::createTAlbum(TAlbum* const mainRootAlbum, const QString& tagStr, const QString& icon,
                                   const QKeySequence& ks, QMap<QString, QString>& errMap)
{
    errMap.clear();
    AlbumList createdTagsList;
    TAlbum* root = nullptr;

    // Check if new tags are include in a list of tags hierarchy separated by ','.
    // Ex: /Country/France/people,/City/France/Paris

    const QStringList tagsHierarchies = tagStr.split(QLatin1Char(','), Qt::SkipEmptyParts);

    if (tagsHierarchies.isEmpty())
    {
        return createdTagsList;
    }

    for (QStringList::const_iterator it = tagsHierarchies.constBegin() ;
         it != tagsHierarchies.constEnd() ; ++it)
    {
        QString hierarchy = (*it).trimmed();

        if (!hierarchy.isEmpty())
        {
            // Check if new tags is a hierarchy of tags separated by '/'.

            root = nullptr;

            if (hierarchy.startsWith(QLatin1Char('/')) || !mainRootAlbum)
            {
                root = AlbumManager::instance()->findTAlbum(0);
            }
            else
            {
                root = mainRootAlbum;
            }

            QStringList tagsList = hierarchy.split(QLatin1Char('/'), Qt::SkipEmptyParts);
            qCDebug(DIGIKAM_GENERAL_LOG) << tagsList;

            if (!tagsList.isEmpty())
            {
                for (QStringList::const_iterator it2 = tagsList.constBegin() ;
                     it2 != tagsList.constEnd() ; ++it2)
                {
                    QString tagPath, errMsg;
                    QString tag = (*it2).trimmed();

                    if (root->isRoot())
                    {
                        tagPath = QString::fromUtf8("/%1").arg(tag);
                    }
                    else
                    {
                        tagPath = QString::fromUtf8("%1/%2").arg(root->tagPath()).arg(tag);
                    }

                    qCDebug(DIGIKAM_GENERAL_LOG) << tag << " :: " << tagPath;

                    if (!tag.isEmpty())
                    {
                        // Tag already exist ?

                        TAlbum* const album = AlbumManager::instance()->findTAlbum(tagPath);

                        if (!album)
                        {
                            root = AlbumManager::instance()->createTAlbum(root, tag, icon, errMsg);
                        }
                        else
                        {
                            root = album;

                            if (*it2 == tagsList.last())
                            {
                                errMap.insert(tagPath, i18n("Tag name already exists"));
                            }
                        }

                        if (root)
                        {
                            createdTagsList.append(root);
                        }
                    }

                    // Sanity check if tag creation failed.

                    if (!root)
                    {
                        errMap.insert(tagPath, errMsg);
                        break;
                    }
                }
            }
        }
    }

    // Assign the keyboard shortcut to the last tag created from the hierarchy.

    if (root && !ks.isEmpty())
    {
        TagsActionMngr::defaultManager()->updateTagShortcut(root->id(), ks);
    }

    return createdTagsList;
}

void TagEditDlg::showtagsListCreationError(QWidget* const parent, const QMap<QString, QString>& errMap)
{
    if (!errMap.isEmpty())
    {
        QPointer<TagsListCreationErrorDialog> dlg = new TagsListCreationErrorDialog(parent, errMap);
        dlg->exec();
        delete dlg;
    }
}

void TagEditDlg::slotHelp()
{
    openOnlineDocumentation(QLatin1String("main_window"), QLatin1String("tags_view"), QLatin1String("managing-tags"));
}

// ------------------------------------------------------------------------------

TagsListCreationErrorDialog::TagsListCreationErrorDialog(QWidget* const parent, const QMap<QString, QString>& errMap)
    : QDialog(parent)
{
    setModal(true);
    setWindowTitle(i18nc("@title:window", "Tag Creation Error"));

    const int spacing = layoutSpacing();
    const int cmargin = qMin(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                             qMin(style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                  qMin(style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                       style()->pixelMetric(QStyle::PM_LayoutBottomMargin))));

    QDialogButtonBox* const buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    QWidget* const page             = new QWidget(this);
    QVBoxLayout* const vLay         = new QVBoxLayout(page);

    QLabel* const label             = new QLabel(i18n("An error occurred during tag creation:"), page);
    QTreeWidget* const listView     = new QTreeWidget(page);
    listView->setHeaderLabels(QStringList() << i18n("Tag Path") << i18n("Error"));
    listView->setRootIsDecorated(false);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);
    listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    vLay->addWidget(label);
    vLay->addWidget(listView);
    vLay->setContentsMargins(cmargin, cmargin, cmargin, cmargin);
    vLay->setSpacing(spacing);

    for (QMap<QString, QString>::const_iterator it = errMap.constBegin() ;
         it != errMap.constEnd() ; ++it)
    {
        new QTreeWidgetItem(listView, QStringList() << it.key() << it.value());
    }

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(buttons);
    setLayout(vbx);

    connect(buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(accept()));

    adjustSize();
}

} // namespace Digikam

#include "tageditdlg.moc"

#include "moc_tageditdlg.cpp"
