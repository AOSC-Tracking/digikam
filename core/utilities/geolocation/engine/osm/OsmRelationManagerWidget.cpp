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

#include "OsmRelationManagerWidget.h"
#include "OsmRelationManagerWidget_p.h"

// Qt includes

#include <QTreeWidget>
#include <QMenu>
#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ui_OsmRelationManagerWidget.h"
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "OsmPlacemarkData.h"
#include "OsmObjectManager.h"
#include "OsmRelationEditorDialog.h"
#include "digikam_debug.h"

namespace Marble
{

OsmRelationManagerWidget::OsmRelationManagerWidget(GeoDataPlacemark* placemark,
                                                   const QHash<qint64, OsmPlacemarkData>* relations,
                                                   QWidget* parent)
    : QWidget(parent),
      d(new OsmRelationManagerWidgetPrivate)
{
    d->m_placemark = placemark;
    d->m_allRelations = relations;
    d->setupUi(this);
    d->populateRelationsList();
    d->m_relationDropMenu = new QMenu(d->m_addRelation);
    d->m_currentRelations->setRootIsDecorated(false);
    d->m_currentRelations->setEditTriggers(QTreeWidget::DoubleClicked);
    d->m_currentRelations->setContextMenuPolicy(Qt::CustomContextMenu);
    d->m_currentRelations->setMinimumWidth(d->m_currentRelations->columnCount() * d->m_currentRelations->columnWidth(0) + 10);

    d->m_addRelation->setMenu(d->m_relationDropMenu);
    d->populateDropMenu();

    QObject::connect(d->m_currentRelations, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                     this, SLOT(handleDoubleClick(QTreeWidgetItem*, int)));
    QObject::connect(d->m_currentRelations, SIGNAL(customContextMenuRequested(QPoint)),
                     this, SLOT(handleRelationContextMenuRequest(QPoint)));
    QObject::connect(d->m_relationDropMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(addRelation(QAction*)));
    QObject::connect(d->m_currentRelations, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                     this, SLOT(handleItemChange(QTreeWidgetItem*, int)));
}

OsmRelationManagerWidget::~OsmRelationManagerWidget()
{
    delete d;
}

void OsmRelationManagerWidget::addRelation(QAction* relationAction)
{
    // The QAction::text() adds a '&' for some reason
    QString relationText = relationAction->text().remove(QLatin1Char('&'));

    if (relationText == i18n("New Relation"))
    {
        OsmPlacemarkData relationData;
        QPointer<OsmRelationEditorDialog> relationEditor = new OsmRelationEditorDialog(&relationData);
        const int result = relationEditor->exec();
        delete relationEditor;

        if (result == QDialog::Rejected)
        {
            return;
        }

        QTreeWidgetItem* newRelationItem = new QTreeWidgetItem();
        newRelationItem->setText(Column::Name, relationData.tagValue(QStringLiteral("name")));
        newRelationItem->setText(Column::Type, relationData.tagValue(QStringLiteral("type")));
        newRelationItem->setData(Column::Name, Qt::UserRole, relationData.id());
        d->m_currentRelations->addTopLevelItem(newRelationItem);

        // Make the user complete the role column
        newRelationItem->setFlags(newRelationItem->flags() | Qt::ItemIsEditable);
        d->m_currentRelations->editItem(newRelationItem, Column::Role);

        // This tells the annotate plugin to add the new relation to its list
        Q_EMIT relationCreated(relationData);
    }

    else
    {
        qint64 id = relationAction->data().toLongLong();
        OsmPlacemarkData relationData = d->m_allRelations->value(id);
        QTreeWidgetItem* newRelationItem = new QTreeWidgetItem();
        newRelationItem->setText(Column::Name, relationData.tagValue(QStringLiteral("name")));
        newRelationItem->setText(Column::Type, relationData.tagValue(QStringLiteral("type")));
        newRelationItem->setData(Column::Name, Qt::UserRole, relationData.id());
        d->m_currentRelations->addTopLevelItem(newRelationItem);

        // Make the user complete the role column
        newRelationItem->setFlags(newRelationItem->flags() | Qt::ItemIsEditable);
        d->m_currentRelations->editItem(newRelationItem, Column::Role);
    }
}

void OsmRelationManagerWidget::update()
{
    d->populateRelationsList();
    d->populateDropMenu();
}

void OsmRelationManagerWidget::handleDoubleClick(QTreeWidgetItem* item, int column)
{
    Qt::ItemFlags flags = item->flags();

    // Only the "role" column should be editable
    if (column == Column::Role)
    {
        item->setFlags(flags | Qt::ItemIsEditable);
        // If the double click didn't occur on the "role" column, and the item
        // is editable make it uneditable
    }

    else if (flags & Qt::ItemIsEditable)
    {
        item->setFlags(flags ^ Qt::ItemIsEditable);
    }
}

void OsmRelationManagerWidget::handleItemChange(QTreeWidgetItem* item, int column)
{
    // Only the role column should be editable
    if (column != Column::Role)
    {
        return;
    }

    QString role = item->text(Column::Role);
    qint64 id = item->data(Column::Name, Qt::UserRole).toLongLong();

    d->m_placemark->osmData().addRelation(id, OsmType::Way, role);
    update();
}

void OsmRelationManagerWidget::handleRelationContextMenuRequest(const QPoint& point)
{
    QMenu relationEditMenu;
    relationEditMenu.addAction(i18n("Remove"));
    relationEditMenu.addAction(i18n("Edit"));
    QAction* selectedItem = relationEditMenu.exec(d->m_currentRelations->mapToGlobal(point));

    if (selectedItem)
    {

        QTreeWidgetItem* requestedItem = d->m_currentRelations->itemAt(point);
        qint64 id = requestedItem->data(Column::Name, Qt::UserRole).toLongLong();

        if (selectedItem->text() == i18n("Remove"))
        {
            d->m_placemark->osmData().removeRelation(id);
            update();
        }

        else if (selectedItem->text() == i18n("Edit"))
        {
            OsmPlacemarkData relationData = d->m_allRelations->value(id);
            QPointer<OsmRelationEditorDialog> relationEditor = new OsmRelationEditorDialog(&relationData);
            const int result = relationEditor->exec();
            delete relationEditor;

            if (result == QDialog::Rejected)
            {
                return;
            }

            Q_EMIT relationCreated(relationData);
            update();
        }
    }
}

} // namespace Marble

#include "moc_OsmRelationManagerWidget.cpp"
