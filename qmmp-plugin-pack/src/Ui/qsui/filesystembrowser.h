#ifndef FILESYSTEMBROWSER_H
#define FILESYSTEMBROWSER_H

#include <QListView>


class QFileSystemModel;

class FileSystemBrowser : public QListView
{
    Q_OBJECT
public:
    explicit FileSystemBrowser(QWidget *parent = 0);

    void readSettings();

private slots:
    void onDoubleClicked(const QModelIndex &ind);

private:
    QFileSystemModel *m_model;

};

#endif // FILESYSTEMBROWSER_H
