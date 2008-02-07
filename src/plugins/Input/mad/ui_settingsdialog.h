/********************************************************************************
** Form generated from reading ui file 'settingsdialog.ui'
**
** Created: Thu Feb 7 00:21:43 2008
**      by: Qt User Interface Compiler version 4.3.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_SettingsDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout;
    QLabel *label_15_2;
    QComboBox *firstTagComboBox;
    QHBoxLayout *hboxLayout1;
    QLabel *label_15_3;
    QComboBox *secondTagComboBox;
    QHBoxLayout *hboxLayout2;
    QLabel *label_15_4;
    QComboBox *thirdTagComboBox;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout3;
    QLabel *label_17_2_2;
    QComboBox *id3v1EncComboBox;
    QHBoxLayout *hboxLayout4;
    QLabel *label_18_2_2;
    QComboBox *id3v2EncComboBox;
    QHBoxLayout *hboxLayout5;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *SettingsDialog)
    {
    if (SettingsDialog->objectName().isEmpty())
        SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
    QSize size(242, 303);
    size = size.expandedTo(SettingsDialog->minimumSizeHint());
    SettingsDialog->resize(size);
    vboxLayout = new QVBoxLayout(SettingsDialog);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    groupBox_2 = new QGroupBox(SettingsDialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    vboxLayout1 = new QVBoxLayout(groupBox_2);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    label_15_2 = new QLabel(groupBox_2);
    label_15_2->setObjectName(QString::fromUtf8("label_15_2"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label_15_2->sizePolicy().hasHeightForWidth());
    label_15_2->setSizePolicy(sizePolicy);
    label_15_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout->addWidget(label_15_2);

    firstTagComboBox = new QComboBox(groupBox_2);
    firstTagComboBox->setObjectName(QString::fromUtf8("firstTagComboBox"));

    hboxLayout->addWidget(firstTagComboBox);


    vboxLayout1->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    label_15_3 = new QLabel(groupBox_2);
    label_15_3->setObjectName(QString::fromUtf8("label_15_3"));
    sizePolicy.setHeightForWidth(label_15_3->sizePolicy().hasHeightForWidth());
    label_15_3->setSizePolicy(sizePolicy);
    label_15_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout1->addWidget(label_15_3);

    secondTagComboBox = new QComboBox(groupBox_2);
    secondTagComboBox->setObjectName(QString::fromUtf8("secondTagComboBox"));

    hboxLayout1->addWidget(secondTagComboBox);


    vboxLayout1->addLayout(hboxLayout1);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    label_15_4 = new QLabel(groupBox_2);
    label_15_4->setObjectName(QString::fromUtf8("label_15_4"));
    sizePolicy.setHeightForWidth(label_15_4->sizePolicy().hasHeightForWidth());
    label_15_4->setSizePolicy(sizePolicy);
    label_15_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout2->addWidget(label_15_4);

    thirdTagComboBox = new QComboBox(groupBox_2);
    thirdTagComboBox->setObjectName(QString::fromUtf8("thirdTagComboBox"));

    hboxLayout2->addWidget(thirdTagComboBox);


    vboxLayout1->addLayout(hboxLayout2);


    vboxLayout->addWidget(groupBox_2);

    groupBox = new QGroupBox(SettingsDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    vboxLayout2 = new QVBoxLayout(groupBox);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    hboxLayout3->setContentsMargins(0, 0, 0, 0);
    label_17_2_2 = new QLabel(groupBox);
    label_17_2_2->setObjectName(QString::fromUtf8("label_17_2_2"));
    label_17_2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout3->addWidget(label_17_2_2);

    id3v1EncComboBox = new QComboBox(groupBox);
    id3v1EncComboBox->setObjectName(QString::fromUtf8("id3v1EncComboBox"));

    hboxLayout3->addWidget(id3v1EncComboBox);


    vboxLayout2->addLayout(hboxLayout3);

    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setSpacing(6);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    hboxLayout4->setContentsMargins(0, 0, 0, 0);
    label_18_2_2 = new QLabel(groupBox);
    label_18_2_2->setObjectName(QString::fromUtf8("label_18_2_2"));
    label_18_2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout4->addWidget(label_18_2_2);

    id3v2EncComboBox = new QComboBox(groupBox);
    id3v2EncComboBox->setObjectName(QString::fromUtf8("id3v2EncComboBox"));

    hboxLayout4->addWidget(id3v2EncComboBox);


    vboxLayout2->addLayout(hboxLayout4);


    vboxLayout->addWidget(groupBox);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setSpacing(6);
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    hboxLayout5->setContentsMargins(0, 0, 0, 0);
    spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem);

    okButton = new QPushButton(SettingsDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout5->addWidget(okButton);

    cancelButton = new QPushButton(SettingsDialog);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout5->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout5);


    retranslateUi(SettingsDialog);
    QObject::connect(cancelButton, SIGNAL(clicked()), SettingsDialog, SLOT(reject()));

    firstTagComboBox->setCurrentIndex(0);
    secondTagComboBox->setCurrentIndex(0);
    thirdTagComboBox->setCurrentIndex(0);


    QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
    SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "MPEG Plugin Settings", 0, QApplication::UnicodeUTF8));
    groupBox_2->setTitle(QApplication::translate("SettingsDialog", "Tag Priority", 0, QApplication::UnicodeUTF8));
    label_15_2->setText(QApplication::translate("SettingsDialog", "First:", 0, QApplication::UnicodeUTF8));
    firstTagComboBox->clear();
    firstTagComboBox->insertItems(0, QStringList()
     << QApplication::translate("SettingsDialog", "ID3v1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "ID3v2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "APE", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "Disabled", 0, QApplication::UnicodeUTF8)
    );
    label_15_3->setText(QApplication::translate("SettingsDialog", "Second:", 0, QApplication::UnicodeUTF8));
    secondTagComboBox->clear();
    secondTagComboBox->insertItems(0, QStringList()
     << QApplication::translate("SettingsDialog", "ID3v1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "ID3v2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "APE", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "Disabled", 0, QApplication::UnicodeUTF8)
    );
    label_15_4->setText(QApplication::translate("SettingsDialog", "Third:", 0, QApplication::UnicodeUTF8));
    thirdTagComboBox->clear();
    thirdTagComboBox->insertItems(0, QStringList()
     << QApplication::translate("SettingsDialog", "ID3v1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "ID3v2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "APE", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("SettingsDialog", "Disabled", 0, QApplication::UnicodeUTF8)
    );
    groupBox->setTitle(QApplication::translate("SettingsDialog", "Encodings", 0, QApplication::UnicodeUTF8));
    label_17_2_2->setText(QApplication::translate("SettingsDialog", "ID3v1 encoding:", 0, QApplication::UnicodeUTF8));
    label_18_2_2->setText(QApplication::translate("SettingsDialog", "ID3v2 encoding:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("SettingsDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("SettingsDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SettingsDialog);
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

#endif // UI_SETTINGSDIALOG_H
