/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-02
 * Description : Sample implementations for drag and drop handling
 *
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QAbstractItemView>

// Local includes

#include "digikam_export.h"
#include "abstractitemdragdrophandler.h"

namespace Digikam
{

class DIGIKAM_EXPORT DragDropModelImplementation
{
public:

    /**
     * A class providing a sample implementation for a QAbstractItemModel
     * redirecting drag-and-drop support to a handler.
     * Include the macro DECLARE_Model_DRAG_DROP_METHODS in your derived QAbstractItemModel class.
     */
    DragDropModelImplementation() = default;
    virtual ~DragDropModelImplementation();

    /**
     * Implements the relevant QAbstractItemModel methods for drag and drop.
     * All functionality is redirected to the handler.
     * dropMimeData() always returns false, leaving implementation to the view.
     */
    Qt::DropActions supportedDropActions()                          const;
    QStringList     mimeTypes()                                     const;
    bool            dropMimeData(const QMimeData*,
                                 Qt::DropAction,
                                 int,
                                 int,
                                 const QModelIndex&);
    QMimeData*      mimeData(const QModelIndexList& indexes)        const;

    /**
     * Call from your flags() method, adding the relevant drag drop flags.
     * Default implementation enables both drag and drop on the index
     * if a drag drop handler is set.
     * Reimplement to fine-tune. Note: There is an alternative below.
     */
    virtual Qt::ItemFlags dragDropFlags(const QModelIndex& index)   const;

    /**
     * This is an alternative approach to dragDropFlags().
     * dragDropFlagsV2 calls the virtual methods isDragEnabled()
     * and isDropEnabled() which you then reimplement.
     * Use simple dragDropFlags() if you need not customization,
     * or reimplement dragDropFlags() if you fine-tune it yourself.
     */
    Qt::ItemFlags dragDropFlagsV2(const QModelIndex& index)         const;
    virtual bool  isDragEnabled(const QModelIndex& index)           const;
    virtual bool  isDropEnabled(const QModelIndex& index)           const;

    /**
     * Set a drag drop handler.
     */
    void setDragDropHandler(AbstractItemDragDropHandler* handler);
    AbstractItemDragDropHandler* dragDropHandler()                  const;

#define DECLARE_MODEL_DRAG_DROP_METHODS                                            \
    virtual Qt::DropActions supportedDropActions()                  const override \
        { return DragDropModelImplementation::supportedDropActions();      }       \
    virtual QStringList mimeTypes()                                 const override \
        { return DragDropModelImplementation::mimeTypes();                 }       \
    virtual bool dropMimeData(const QMimeData* d, Qt::DropAction a,                \
                              int r, int c, const QModelIndex& p)         override \
        { return DragDropModelImplementation::dropMimeData(d, a, r, c, p); }       \
    virtual QMimeData* mimeData(const QModelIndexList& indexes)     const override \
        { return DragDropModelImplementation::mimeData(indexes);           }

protected:

    AbstractItemDragDropHandler* m_dragDropHandler = nullptr;

private:

    Q_DISABLE_COPY(DragDropModelImplementation)
};

// --------------------------------------------------------------------------------------------

class DIGIKAM_EXPORT DragDropViewImplementation
{
public:

    DragDropViewImplementation()          = default;
    virtual ~DragDropViewImplementation() = default;

    virtual void cut();
    virtual void copy();
    virtual void paste();

protected:

    /**
     * This one is implemented by DECLARE_VIEW_DRAG_DROP_METHODS.
     */
    virtual QAbstractItemView* asView()                                        = 0;

    /**
     * You need to implement these three methods
     * Returns the drag drop handler.
     */
    virtual AbstractItemDragDropHandler* dragDropHandler()               const = 0;

    /**
     * Maps the given index of the view's model to an index of the handler's model,
     * which can be a source model of the view's model.
     */
    virtual QModelIndex mapIndexForDragDrop(const QModelIndex& index)    const = 0;

    /**
     * Creates a pixmap for dragging the given indexes.
     */
    virtual QPixmap     pixmapForDrag(const QList<QModelIndex>& indexes) const = 0;

    /**
     * Implements the relevant QAbstractItemView methods for drag and drop.
     */
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);
    void startDrag(Qt::DropActions supportedActions);

#define DECLARE_VIEW_DRAG_DROP_METHODS(ParentViewClass)                           \
    virtual QAbstractItemView* asView()                 override                  \
        { return this;                                             }              \
    void dragEnterEvent(QDragEnterEvent* e)             override                  \
        { DragDropViewImplementation::dragEnterEvent(e);           }              \
    void dragMoveEvent(QDragMoveEvent* e)               override                  \
        { ParentViewClass::dragMoveEvent(e);                                      \
          DragDropViewImplementation::dragMoveEvent(e);            }              \
    void dropEvent(QDropEvent* e)                       override                  \
        { ParentViewClass::dropEvent(e);                                          \
          DragDropViewImplementation::dropEvent(e);                }              \
    void startDrag(Qt::DropActions supportedActions)    override                  \
        { DragDropViewImplementation::startDrag(supportedActions); }

    void encodeIsCutSelection(QMimeData* mime, bool isCutSelection);
    bool decodeIsCutSelection(const QMimeData* mimeData);

private:

    Q_DISABLE_COPY(DragDropViewImplementation)
};

} // namespace Digikam
