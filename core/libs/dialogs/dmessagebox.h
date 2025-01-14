/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-01-19
 * Description : message box notification settings
 *
 * SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QListWidget>

// Local includes

#include "digikam_export.h"

class QDialog;
class QDialogButtonBox;

namespace Digikam
{

class DIGIKAM_EXPORT DMessageBox
{

public:

    /**
     * @return true if the corresponding message box should be shown.
     * @param dontShowAgainName the name that identify the message box. If
     * empty, this method return false.
     */
    static bool readMsgBoxShouldBeShown(const QString& dontShowAgainName);

    /**
     * Save the fact that the message box should not be shown again.
     * @param dontShowAgainName the name that identify the message box. If
     * empty, this method does nothing.
     * @param value the value chosen in the message box to show it again next time.
     */
    static void saveMsgBoxShouldBeShown(const QString& dontShowAgainName, bool value);

public:

    /**
     * Show List of items into an informative message box.
     */
    static void showInformationList(QMessageBox::Icon icon,
                                    QWidget* const parent,
                                    const QString& caption,
                                    const QString& text,
                                    const QStringList& items,
                                    const QString& dontShowAgainName = QString());

    /**
     * Show widget into an informative message box.
     */
    static void showInformationWidget(QMessageBox::Icon icon,
                                      QWidget* const parent,
                                      const QString& caption,
                                      const QString& text,
                                      QWidget* const listWidget,
                                      const QString& dontShowAgainName);

public:

    /**
     * Show a message box with Continue and Cancel buttons, and wait user feedback.
     * Return QMessageBox::Yes or QMessageBox::Cancel.
     */
    static int showContinueCancel(QMessageBox::Icon icon,
                                  QWidget* const parent,
                                  const QString& caption,
                                  const QString& text,
                                  const QString& dontAskAgainName = QString());

    /**
     * Show List of items to process into a message box with Continue and Cancel buttons,
     * and wait user feedback.
     * Return QMessageBox::Yes or QMessageBox::Cancel.
     */
    static int showContinueCancelList(QMessageBox::Icon icon,
                                      QWidget* const parent,
                                      const QString& caption,
                                      const QString& text,
                                      const QStringList& items,
                                      const QString& dontAskAgainName = QString());

    /**
     * Show widget into a message box with Continue and Cancel buttons,
     * and wait user feedback.
     * Return QMessageBox::Yes or QMessageBox::Cancel.
     */
    static int showContinueCancelWidget(QMessageBox::Icon icon,
                                        QWidget* const parent,
                                        const QString& caption,
                                        const QString& text,
                                        QWidget* const listWidget,
                                        const QString& dontAskAgainName);

public:

    /**
     * Show a message box with Yes and No buttons, and wait user feedback.
     * Return QMessageBox::Yes or QMessageBox::No.
     */
    static int showYesNo(QMessageBox::Icon icon,
                         QWidget* const parent,
                         const QString& caption,
                         const QString& text,
                         const QString& dontAskAgainName = QString());

    /**
     * Show List of items to process into a message box with Yes and No buttons,
     * and wait user feedback.
     * Return QMessageBox::Yes or QMessageBox::No.
     */
    static int showYesNoList(QMessageBox::Icon icon,
                             QWidget* const parent,
                             const QString& caption,
                             const QString& text,
                             const QStringList& items,
                             const QString& dontAskAgainName = QString());

    /**
     * Show widget into a message box with Yes and No buttons,
     * and wait user feedback.
     * Return QMessageBox::Yes or QMessageBox::No.
     */
    static int showYesNoWidget(QMessageBox::Icon icon,
                               QWidget* const parent,
                               const QString& caption,
                               const QString& text,
                               QWidget* const listWidget,
                               const QString& dontAskAgainName = QString());

private:

    static int createMessageBox(QDialog* const dialog,
                                QDialogButtonBox* const buttons,
                                const QIcon& icon,
                                const QString& text,
                                QWidget* const listWidget,
                                const QString& ask,
                                bool* checkboxReturn);

    static QIcon createIcon(QMessageBox::Icon icon);

    static QListWidget* createWidgetList(const QStringList& items);
};

} // namespace Digikam
