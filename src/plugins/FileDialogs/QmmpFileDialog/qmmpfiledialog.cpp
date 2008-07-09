/**************************************************************************
*   Copyright (C) 2008 by Ilya Kotov                                      *
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

#include <QtPlugin>
#include <QTranslator>
#include <QMessageBox>

#include "qmmpfiledialogimpl.h"
#include "qmmpfiledialog.h"



QmmpFileDialog::QmmpFileDialog()
{
    m_dialog = new QmmpFileDialogImpl();
    connect(m_dialog,SIGNAL(filesAdded(const QStringList&)),this,SIGNAL(filesAdded(const QStringList&)));
}

void QmmpFileDialog::handleSelected(/*const QStringList& s */)
{
}

bool QmmpFileDialog::modal()const
{
    return FALSE;
}

QmmpFileDialog::~QmmpFileDialog()
{
    qWarning("QmmpFileDialog::~QmmpFileDialog()");
    delete m_dialog;
}
void QmmpFileDialog::raise(const QString &dir, Mode mode, const QString &caption,
                           const QStringList &mask)
{
    m_dialog->setModeAndMask(dir, mode, mask);
    m_dialog->setWindowTitle(caption);
    m_dialog->show();
    m_dialog->raise();
}

QString QmmpFileDialog::openFileName(QWidget *parent, const QString &caption,
                                     const QString &dir, const QString &filter, QString*)
{
    QmmpFileDialogImpl *dialog = new QmmpFileDialogImpl(parent);
    dialog->setWindowTitle(caption);
    dialog->setModeAndMask(dir, FileDialog::AddFile, filter.split(";;"));
    QStringList l;
    if (dialog->exec() == QDialog::Accepted)
        l = dialog->selectedFiles();
    dialog->deleteLater();
    if (l.isEmpty())
        return QString();
    else
        return l.at(0);
}

QString QmmpFileDialog::saveFileName (QWidget *parent, const QString &caption,
                                      const QString &dir, const QString &filter, QString*)
{
    QmmpFileDialogImpl *dialog = new QmmpFileDialogImpl(parent);
    dialog->setWindowTitle(caption);
    dialog->setModeAndMask(dir, FileDialog::SaveFile, filter.split(";;"));
    QStringList l;
    if (dialog->exec() == QDialog::Accepted)
        l = dialog->selectedFiles();
    dialog->deleteLater();
    if (l.isEmpty())
        return QString();
    else
        return l.at(0);
}

FileDialog* QmmpFileDialogFactory::create()
{
    return new QmmpFileDialog();
}

const FileDialogProperties QmmpFileDialogFactory::properties() const
{
    FileDialogProperties properties;
    properties.name = tr("Qmmp File Dialog");
    properties.hasAbout =TRUE;
    return properties;
}

void QmmpFileDialogFactory::showAbout(QWidget *parent)
{
    QMessageBox::about (parent, tr("About Qmmp File Dialog"),
                        tr("Qmmp File Dialog")+"\n"+
                        tr("Writen by:\n"
                           "Vladimir Kuznetsov <vovanec@gmail.com>\n"
                           "Ilya Kotov <forkotov02@hotmail.ru>")+"\n"+
                        tr("Some code is copied from the Qt library"));

}

QTranslator *QmmpFileDialogFactory::createTranslator(QObject *parent)
{
    QTranslator *translator = new QTranslator(parent);
    QString locale = QLocale::system().name();
    translator->load(QString(":/qmmp_file_dialog_plugin_") + locale);
    return translator;
}

Q_EXPORT_PLUGIN(QmmpFileDialogFactory)
