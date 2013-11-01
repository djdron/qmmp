#ifndef FILESYSTEMBROWSER_H
#define FILESYSTEMBROWSER_H

#include <QWidget>

namespace  Ui {
    class FileSystemBrowser;
}

class QFileSystemModel;
class QModelIndex;

class FileSystemBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystemBrowser(QWidget *parent = 0);
    ~FileSystemBrowser();

    void readSettings();

private slots:
    void on_listView_activated(const QModelIndex &index);

private:
    void setCurrentDirectory(const QString &path);
    Ui::FileSystemBrowser *m_ui;
    QFileSystemModel *m_model;

};

#endif // FILESYSTEMBROWSER_H
