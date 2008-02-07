/********************************************************************************
** Form generated from reading ui file 'detailsdialog.ui'
**
** Created: Thu Feb 7 00:21:43 2008
**      by: Qt User Interface Compiler version 4.3.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DETAILSDIALOG_H
#define UI_DETAILSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_DetailsDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QLabel *label_28;
    QLineEdit *pathLineEdit;
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox_3;
    QHBoxLayout *hboxLayout1;
    QRadioButton *id3v1RadioButton;
    QRadioButton *id3v2RadioButton;
    QRadioButton *apeRadioButton;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QLabel *label;
    QLabel *levelLabel;
    QLabel *label_2;
    QLabel *bitRateLabel;
    QLabel *label_3;
    QLabel *sampleRateLabel;
    QLabel *label_5;
    QLabel *fileSizeLabel;
    QLabel *label_6;
    QLabel *modeLabel;
    QLabel *label_8;
    QLabel *copyrightLabel;
    QLabel *label_9;
    QLabel *originalLabel;
    QGroupBox *tagGroupBox;
    QVBoxLayout *vboxLayout1;
    QWidget *tagsWidget;
    QGridLayout *gridLayout2;
    QLabel *label_21;
    QLineEdit *titleLineEdit;
    QLabel *label_22;
    QLineEdit *artistLineEdit;
    QLabel *label_23;
    QLineEdit *albumLineEdit;
    QLabel *label_24;
    QLineEdit *commentLineEdit;
    QLabel *label_25;
    QLineEdit *yearLineEdit;
    QLabel *label_26;
    QLineEdit *trackLineEdit;
    QLabel *label_27;
    QLineEdit *genreLineEdit;
    QHBoxLayout *hboxLayout2;
    QPushButton *createButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QSpacerItem *spacerItem;
    QPushButton *pushButton_3;

    void setupUi(QDialog *DetailsDialog)
    {
    if (DetailsDialog->objectName().isEmpty())
        DetailsDialog->setObjectName(QString::fromUtf8("DetailsDialog"));
    QSize size(593, 402);
    size = size.expandedTo(DetailsDialog->minimumSizeHint());
    DetailsDialog->resize(size);
    gridLayout = new QGridLayout(DetailsDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    label_28 = new QLabel(DetailsDialog);
    label_28->setObjectName(QString::fromUtf8("label_28"));
    label_28->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

    hboxLayout->addWidget(label_28);

    pathLineEdit = new QLineEdit(DetailsDialog);
    pathLineEdit->setObjectName(QString::fromUtf8("pathLineEdit"));
    pathLineEdit->setReadOnly(true);

    hboxLayout->addWidget(pathLineEdit);


    gridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

    vboxLayout = new QVBoxLayout();
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    groupBox_3 = new QGroupBox(DetailsDialog);
    groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
    groupBox_3->setSizePolicy(sizePolicy);
    hboxLayout1 = new QHBoxLayout(groupBox_3);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    id3v1RadioButton = new QRadioButton(groupBox_3);
    id3v1RadioButton->setObjectName(QString::fromUtf8("id3v1RadioButton"));

    hboxLayout1->addWidget(id3v1RadioButton);

    id3v2RadioButton = new QRadioButton(groupBox_3);
    id3v2RadioButton->setObjectName(QString::fromUtf8("id3v2RadioButton"));

    hboxLayout1->addWidget(id3v2RadioButton);

    apeRadioButton = new QRadioButton(groupBox_3);
    apeRadioButton->setObjectName(QString::fromUtf8("apeRadioButton"));

    hboxLayout1->addWidget(apeRadioButton);


    vboxLayout->addWidget(groupBox_3);

    groupBox = new QGroupBox(DetailsDialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setMinimumSize(QSize(200, 16));
    gridLayout1 = new QGridLayout(groupBox);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    label = new QLabel(groupBox);
    label->setObjectName(QString::fromUtf8("label"));
    label->setTextFormat(Qt::AutoText);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label, 0, 0, 1, 1);

    levelLabel = new QLabel(groupBox);
    levelLabel->setObjectName(QString::fromUtf8("levelLabel"));

    gridLayout1->addWidget(levelLabel, 0, 1, 1, 1);

    label_2 = new QLabel(groupBox);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_2, 1, 0, 1, 1);

    bitRateLabel = new QLabel(groupBox);
    bitRateLabel->setObjectName(QString::fromUtf8("bitRateLabel"));

    gridLayout1->addWidget(bitRateLabel, 1, 1, 1, 1);

    label_3 = new QLabel(groupBox);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_3, 2, 0, 1, 1);

    sampleRateLabel = new QLabel(groupBox);
    sampleRateLabel->setObjectName(QString::fromUtf8("sampleRateLabel"));

    gridLayout1->addWidget(sampleRateLabel, 2, 1, 1, 1);

    label_5 = new QLabel(groupBox);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_5, 3, 0, 1, 1);

    fileSizeLabel = new QLabel(groupBox);
    fileSizeLabel->setObjectName(QString::fromUtf8("fileSizeLabel"));

    gridLayout1->addWidget(fileSizeLabel, 3, 1, 1, 1);

    label_6 = new QLabel(groupBox);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_6, 4, 0, 1, 1);

    modeLabel = new QLabel(groupBox);
    modeLabel->setObjectName(QString::fromUtf8("modeLabel"));

    gridLayout1->addWidget(modeLabel, 4, 1, 1, 1);

    label_8 = new QLabel(groupBox);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_8, 5, 0, 1, 1);

    copyrightLabel = new QLabel(groupBox);
    copyrightLabel->setObjectName(QString::fromUtf8("copyrightLabel"));

    gridLayout1->addWidget(copyrightLabel, 5, 1, 1, 1);

    label_9 = new QLabel(groupBox);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_9, 6, 0, 1, 1);

    originalLabel = new QLabel(groupBox);
    originalLabel->setObjectName(QString::fromUtf8("originalLabel"));

    gridLayout1->addWidget(originalLabel, 6, 1, 1, 1);


    vboxLayout->addWidget(groupBox);


    gridLayout->addLayout(vboxLayout, 1, 0, 2, 1);

    tagGroupBox = new QGroupBox(DetailsDialog);
    tagGroupBox->setObjectName(QString::fromUtf8("tagGroupBox"));
    vboxLayout1 = new QVBoxLayout(tagGroupBox);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    tagsWidget = new QWidget(tagGroupBox);
    tagsWidget->setObjectName(QString::fromUtf8("tagsWidget"));
    tagsWidget->setEnabled(true);
    gridLayout2 = new QGridLayout(tagsWidget);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    label_21 = new QLabel(tagsWidget);
    label_21->setObjectName(QString::fromUtf8("label_21"));
    label_21->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_21, 0, 0, 1, 1);

    titleLineEdit = new QLineEdit(tagsWidget);
    titleLineEdit->setObjectName(QString::fromUtf8("titleLineEdit"));

    gridLayout2->addWidget(titleLineEdit, 0, 1, 1, 3);

    label_22 = new QLabel(tagsWidget);
    label_22->setObjectName(QString::fromUtf8("label_22"));
    label_22->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_22, 1, 0, 1, 1);

    artistLineEdit = new QLineEdit(tagsWidget);
    artistLineEdit->setObjectName(QString::fromUtf8("artistLineEdit"));

    gridLayout2->addWidget(artistLineEdit, 1, 1, 1, 3);

    label_23 = new QLabel(tagsWidget);
    label_23->setObjectName(QString::fromUtf8("label_23"));
    label_23->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_23, 2, 0, 1, 1);

    albumLineEdit = new QLineEdit(tagsWidget);
    albumLineEdit->setObjectName(QString::fromUtf8("albumLineEdit"));

    gridLayout2->addWidget(albumLineEdit, 2, 1, 1, 3);

    label_24 = new QLabel(tagsWidget);
    label_24->setObjectName(QString::fromUtf8("label_24"));
    label_24->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_24, 3, 0, 1, 1);

    commentLineEdit = new QLineEdit(tagsWidget);
    commentLineEdit->setObjectName(QString::fromUtf8("commentLineEdit"));

    gridLayout2->addWidget(commentLineEdit, 3, 1, 1, 3);

    label_25 = new QLabel(tagsWidget);
    label_25->setObjectName(QString::fromUtf8("label_25"));
    label_25->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_25, 4, 0, 1, 1);

    yearLineEdit = new QLineEdit(tagsWidget);
    yearLineEdit->setObjectName(QString::fromUtf8("yearLineEdit"));

    gridLayout2->addWidget(yearLineEdit, 4, 1, 1, 1);

    label_26 = new QLabel(tagsWidget);
    label_26->setObjectName(QString::fromUtf8("label_26"));
    label_26->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_26, 4, 2, 1, 1);

    trackLineEdit = new QLineEdit(tagsWidget);
    trackLineEdit->setObjectName(QString::fromUtf8("trackLineEdit"));

    gridLayout2->addWidget(trackLineEdit, 4, 3, 1, 1);

    label_27 = new QLabel(tagsWidget);
    label_27->setObjectName(QString::fromUtf8("label_27"));
    label_27->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_27, 5, 0, 1, 1);

    genreLineEdit = new QLineEdit(tagsWidget);
    genreLineEdit->setObjectName(QString::fromUtf8("genreLineEdit"));

    gridLayout2->addWidget(genreLineEdit, 5, 1, 1, 3);


    vboxLayout1->addWidget(tagsWidget);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    createButton = new QPushButton(tagGroupBox);
    createButton->setObjectName(QString::fromUtf8("createButton"));

    hboxLayout2->addWidget(createButton);

    deleteButton = new QPushButton(tagGroupBox);
    deleteButton->setObjectName(QString::fromUtf8("deleteButton"));

    hboxLayout2->addWidget(deleteButton);

    saveButton = new QPushButton(tagGroupBox);
    saveButton->setObjectName(QString::fromUtf8("saveButton"));
    saveButton->setEnabled(false);

    hboxLayout2->addWidget(saveButton);


    vboxLayout1->addLayout(hboxLayout2);


    gridLayout->addWidget(tagGroupBox, 1, 1, 1, 2);

    spacerItem = new QSpacerItem(111, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem, 2, 1, 1, 1);

    pushButton_3 = new QPushButton(DetailsDialog);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

    gridLayout->addWidget(pushButton_3, 2, 2, 1, 1);


    retranslateUi(DetailsDialog);
    QObject::connect(pushButton_3, SIGNAL(clicked()), DetailsDialog, SLOT(close()));

    QMetaObject::connectSlotsByName(DetailsDialog);
    } // setupUi

    void retranslateUi(QDialog *DetailsDialog)
    {
    DetailsDialog->setWindowTitle(QApplication::translate("DetailsDialog", "Details", 0, QApplication::UnicodeUTF8));
    label_28->setText(QApplication::translate("DetailsDialog", "File path:", 0, QApplication::UnicodeUTF8));
    groupBox_3->setTitle(QApplication::translate("DetailsDialog", "Tag Choice", 0, QApplication::UnicodeUTF8));
    id3v1RadioButton->setText(QApplication::translate("DetailsDialog", "ID3v1", 0, QApplication::UnicodeUTF8));
    id3v2RadioButton->setText(QApplication::translate("DetailsDialog", "ID3v2", 0, QApplication::UnicodeUTF8));
    apeRadioButton->setText(QApplication::translate("DetailsDialog", "APE", 0, QApplication::UnicodeUTF8));
    groupBox->setTitle(QApplication::translate("DetailsDialog", "MPEG Info", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("DetailsDialog", "MPEG level:", 0, QApplication::UnicodeUTF8));
    levelLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("DetailsDialog", "Bit rate:", 0, QApplication::UnicodeUTF8));
    bitRateLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("DetailsDialog", "Sample rate:", 0, QApplication::UnicodeUTF8));
    sampleRateLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("DetailsDialog", "File size:", 0, QApplication::UnicodeUTF8));
    fileSizeLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("DetailsDialog", "Mode:", 0, QApplication::UnicodeUTF8));
    modeLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("DetailsDialog", "Copyright:", 0, QApplication::UnicodeUTF8));
    copyrightLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("DetailsDialog", "Original:", 0, QApplication::UnicodeUTF8));
    originalLabel->setText(QApplication::translate("DetailsDialog", "-", 0, QApplication::UnicodeUTF8));
    tagGroupBox->setTitle(QApplication::translate("DetailsDialog", "ID3v1 Tag", 0, QApplication::UnicodeUTF8));
    label_21->setText(QApplication::translate("DetailsDialog", "Title:", 0, QApplication::UnicodeUTF8));
    label_22->setText(QApplication::translate("DetailsDialog", "Artist:", 0, QApplication::UnicodeUTF8));
    label_23->setText(QApplication::translate("DetailsDialog", "Album:", 0, QApplication::UnicodeUTF8));
    label_24->setText(QApplication::translate("DetailsDialog", "Comment:", 0, QApplication::UnicodeUTF8));
    label_25->setText(QApplication::translate("DetailsDialog", "Year:", 0, QApplication::UnicodeUTF8));
    label_26->setText(QApplication::translate("DetailsDialog", "Track number:", 0, QApplication::UnicodeUTF8));
    label_27->setText(QApplication::translate("DetailsDialog", "Genre:", 0, QApplication::UnicodeUTF8));
    createButton->setText(QApplication::translate("DetailsDialog", "Create", 0, QApplication::UnicodeUTF8));
    deleteButton->setText(QApplication::translate("DetailsDialog", "Delete", 0, QApplication::UnicodeUTF8));
    saveButton->setText(QApplication::translate("DetailsDialog", "Save", 0, QApplication::UnicodeUTF8));
    pushButton_3->setText(QApplication::translate("DetailsDialog", "Close", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DetailsDialog);
    } // retranslateUi

};

namespace Ui {
    class DetailsDialog: public Ui_DetailsDialog {};
} // namespace Ui

#endif // UI_DETAILSDIALOG_H
