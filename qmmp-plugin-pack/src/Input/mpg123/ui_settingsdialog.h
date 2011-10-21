/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created: Fri Oct 21 22:50:53 2011
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label_15_2;
    QComboBox *firstTagComboBox;
    QLabel *label_15_3;
    QComboBox *secondTagComboBox;
    QLabel *label_15_4;
    QComboBox *thirdTagComboBox;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QLabel *label_17_2_2;
    QComboBox *id3v1EncComboBox;
    QLabel *label_18_2_2;
    QComboBox *id3v2EncComboBox;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
        SettingsDialog->resize(238, 243);
        gridLayout = new QGridLayout(SettingsDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(6, -1, 6, 6);
        groupBox_2 = new QGroupBox(SettingsDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_15_2 = new QLabel(groupBox_2);
        label_15_2->setObjectName(QString::fromUtf8("label_15_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_15_2->sizePolicy().hasHeightForWidth());
        label_15_2->setSizePolicy(sizePolicy);
        label_15_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_15_2, 0, 0, 1, 1);

        firstTagComboBox = new QComboBox(groupBox_2);
        firstTagComboBox->setObjectName(QString::fromUtf8("firstTagComboBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(firstTagComboBox->sizePolicy().hasHeightForWidth());
        firstTagComboBox->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(firstTagComboBox, 0, 1, 1, 2);

        label_15_3 = new QLabel(groupBox_2);
        label_15_3->setObjectName(QString::fromUtf8("label_15_3"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_15_3->sizePolicy().hasHeightForWidth());
        label_15_3->setSizePolicy(sizePolicy2);
        label_15_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_15_3, 1, 0, 1, 1);

        secondTagComboBox = new QComboBox(groupBox_2);
        secondTagComboBox->setObjectName(QString::fromUtf8("secondTagComboBox"));
        sizePolicy1.setHeightForWidth(secondTagComboBox->sizePolicy().hasHeightForWidth());
        secondTagComboBox->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(secondTagComboBox, 1, 1, 1, 2);

        label_15_4 = new QLabel(groupBox_2);
        label_15_4->setObjectName(QString::fromUtf8("label_15_4"));
        sizePolicy.setHeightForWidth(label_15_4->sizePolicy().hasHeightForWidth());
        label_15_4->setSizePolicy(sizePolicy);
        label_15_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_2->addWidget(label_15_4, 2, 0, 1, 1);

        thirdTagComboBox = new QComboBox(groupBox_2);
        thirdTagComboBox->setObjectName(QString::fromUtf8("thirdTagComboBox"));
        sizePolicy1.setHeightForWidth(thirdTagComboBox->sizePolicy().hasHeightForWidth());
        thirdTagComboBox->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(thirdTagComboBox, 2, 1, 1, 2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 3, 3, 1);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 2);

        groupBox = new QGroupBox(SettingsDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_17_2_2 = new QLabel(groupBox);
        label_17_2_2->setObjectName(QString::fromUtf8("label_17_2_2"));
        sizePolicy2.setHeightForWidth(label_17_2_2->sizePolicy().hasHeightForWidth());
        label_17_2_2->setSizePolicy(sizePolicy2);
        label_17_2_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_17_2_2, 0, 0, 1, 1);

        id3v1EncComboBox = new QComboBox(groupBox);
        id3v1EncComboBox->setObjectName(QString::fromUtf8("id3v1EncComboBox"));
        sizePolicy1.setHeightForWidth(id3v1EncComboBox->sizePolicy().hasHeightForWidth());
        id3v1EncComboBox->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(id3v1EncComboBox, 0, 1, 1, 1);

        label_18_2_2 = new QLabel(groupBox);
        label_18_2_2->setObjectName(QString::fromUtf8("label_18_2_2"));
        sizePolicy2.setHeightForWidth(label_18_2_2->sizePolicy().hasHeightForWidth());
        label_18_2_2->setSizePolicy(sizePolicy2);
        label_18_2_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_18_2_2, 1, 0, 1, 1);

        id3v2EncComboBox = new QComboBox(groupBox);
        id3v2EncComboBox->setObjectName(QString::fromUtf8("id3v2EncComboBox"));
        sizePolicy1.setHeightForWidth(id3v2EncComboBox->sizePolicy().hasHeightForWidth());
        id3v2EncComboBox->setSizePolicy(sizePolicy1);

        gridLayout_3->addWidget(id3v2EncComboBox, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_2, 0, 2, 2, 1);


        gridLayout->addWidget(groupBox, 1, 0, 1, 2);

        spacerItem = new QSpacerItem(125, 18, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 2, 0, 1, 1);

        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 1, 1, 1);


        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));

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
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
