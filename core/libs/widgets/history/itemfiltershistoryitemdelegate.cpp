/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-02
 * Description : delegate for custom painting of used filters view
 *
 * SPDX-FileCopyrightText: 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemfiltershistoryitemdelegate.h"

// Qt includes

#include <QPainter>
#include <QBrush>
#include <QGradient>
#include <QLinearGradient>
#include <QApplication>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dimgfiltermanager.h"

namespace Digikam
{

ItemFiltersHistoryItemDelegate::ItemFiltersHistoryItemDelegate(QObject* const parent)
    : QStyledItemDelegate(parent)
{
}

QSize ItemFiltersHistoryItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // Add padding of 6px

    QSize size = QStyledItemDelegate::sizeHint(option, index);

    if (!size.isNull())
    {
        return QSize(size.width(), size.height() + 12);
    }

    return size;
}

void ItemFiltersHistoryItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!(option.state & QStyle::State_Enabled))
    {
        // If disabled, use italic font

        QStyleOptionViewItem o(option);
        QFont font(o.font);
        font.setItalic(true);
        o.font = font;
        QStyledItemDelegate::paint(painter, o, index);
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }

    // draw 1px border

    QPen oldPen = painter->pen();
    QPen pen(option.palette.windowText(), 0);
    painter->setPen(pen);
    painter->drawRect(option.rect);
    painter->setPen(oldPen);

/*
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    KColorScheme sysColors(QPalette::Normal);

    QPen thinLinePen;
    thinLinePen.setWidth(0);
    thinLinePen.setCosmetic(true);
    thinLinePen.setBrush(sysColors.foreground());

    Qt::ItemFlags flags = index.flags();

    if (!index.model()->parent(index).isValid())
    {
        painter->setPen(thinLinePen);
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->drawRect(option.rect);
        painter->setRenderHint(QPainter::Antialiasing, true);

        QPixmap icon = QIcon::fromTheme(index.data(Qt::DecorationRole).toString()).pixmap(22,
                                        (flags & Qt::ItemIsEnabled) ? QIcon::Normal
                                                                    : QIcon::Disabled
                                       );
        painter->drawPixmap(option.rect.left()+4, option.rect.top()+5, icon);
    }

    QRect textRect = option.rect;
    textRect.setLeft(textRect.left() + 32);

    if (!flags & Qt::ItemIsEnabled)
    {
        QFont disabledFont(QApplication::font());
        disabledFont.setItalic(true);

        QPalette disabledColor;

        painter->setFont(disabledFont);
        painter->setPen(disabledColor.color(QPalette::Disabled, QPalette::WindowText));
    }

    if (!index.data(Qt::DisplayRole).toString().isEmpty())
    {
        painter->drawText(textRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());
    }
    else
    {
        //infoIcon.setToolTip(i18n("This filter's name is unknown, so you see only its identifier"));    //FIXME: better string?
        painter->drawPixmap(option.rect.right() - 22, option.rect.top() + 8,
                            QIcon::fromTheme(QLatin1String("dialog-information")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));
        painter->drawText(textRect, Qt::AlignVCenter, index.data(Qt::DecorationRole).toString());
    }

    if (entryDisabled)
    {
        //painter->fillRect(option.rect, QColor(200,200,200,160));
    }

    painter->restore();
*/
}

} //namespace Digikam

#include "moc_itemfiltershistoryitemdelegate.cpp"
