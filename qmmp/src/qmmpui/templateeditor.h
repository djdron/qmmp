/***************************************************************************
 *   Copyright (C) 2010 by Ilya Kotov                                      *
 *   forkotov02@hotmail.ru                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TEMPLATEEDITOR_H
#define TEMPLATEEDITOR_H

#include <QDialog>


namespace Ui {
    class TemplateEditor;
}
class QAction;


/*!
 * @author Ilya Kotov <forkotov02@hotmail.ru>
 */
class TemplateEditor : public QDialog
{
Q_OBJECT
public:
    explicit TemplateEditor(QWidget *parent = 0);

    QString currentTemplate() const;
    void setTemplate(const QString &text = QString());
    void setDefaultTemplate(const QString &text);
    static QString getTemplate (QWidget *parent, const QString &title, const QString &text = QString(),
                                const QString &default_template = QString(), bool *ok = 0);

private slots:
    void insertExpression(QAction *a);
    void on_resetButton_clicked();

private:
    void createMenu();
    Ui::TemplateEditor *m_ui;
    QString m_defaultTemplate;

};

#endif // TEMPLATEEDITOR_H
