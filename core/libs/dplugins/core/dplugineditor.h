/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : Image Editor digiKam plugin definition.
 *
 * SPDX-FileCopyrightText: 2018-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>

// Local includes

#include "dinfointerface.h"
#include "dplugin.h"
#include "dpluginaction.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DPluginEditor : public DPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor with optional parent object
     */
    explicit DPluginEditor(QObject* const parent = nullptr);

    /**
     * Destructor
     */
    ~DPluginEditor() override;

public:

    /**
     * Holds whether the plugin can be seen in parent view.
     */
    void setVisible(bool b) override;

    /**
     * Return all plugin actions registered in setup() method with addAction() for a given parent.
     */
    QList<DPluginAction*> actions(QObject* const parent) const;

    /**
     * Return the amount of tools registered to all parents.
     */
    int count() const override;

    /**
     * Return a plugin action instance found by name in plugin action list for a given parent.
     */
    DPluginAction* findActionByName(const QString& name, QObject* const parent) const;

    /**
     * Return a list of categories as strings registered in this plugin.
     */
    QStringList categories() const override;

    /**
     * Return the plugin interface identifier.
     */
    QString ifaceIid() const override { return QLatin1String(DIGIKAM_DPLUGIN_EDITOR_IID); };

protected:

    void addAction(DPluginAction* const ac);

    /**
     * Return the info interface instance for the given action.
     */
    DInfoInterface* infoIface(QObject* const ac) const;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

Q_DECLARE_INTERFACE(Digikam::DPluginEditor, DIGIKAM_DPLUGIN_EDITOR_IID)
