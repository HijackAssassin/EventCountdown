/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_2;
    QGridLayout *tileGridWidget;
    QFrame *createFrame;
    QHBoxLayout *horizontalLayout;
    QLineEdit *titleInput;
    QComboBox *yearCombo;
    QComboBox *monthCombo;
    QComboBox *dayCombo;
    QComboBox *hourCombo;
    QPushButton *chooseImageButton;
    QPushButton *createButton;
    QPushButton *plusButton;
    QPushButton *editModeButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1920, 1080);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setEnabled(true);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1900, 975));
        gridLayout_2 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_2->setObjectName("gridLayout_2");
        tileGridWidget = new QGridLayout();
        tileGridWidget->setObjectName("tileGridWidget");
        tileGridWidget->setSizeConstraint(QLayout::SetDefaultConstraint);

        gridLayout_2->addLayout(tileGridWidget, 0, 0, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        createFrame = new QFrame(centralwidget);
        createFrame->setObjectName("createFrame");
        createFrame->setVisible(false);
        createFrame->setFrameShape(QFrame::StyledPanel);
        horizontalLayout = new QHBoxLayout(createFrame);
        horizontalLayout->setObjectName("horizontalLayout");
        titleInput = new QLineEdit(createFrame);
        titleInput->setObjectName("titleInput");

        horizontalLayout->addWidget(titleInput);

        yearCombo = new QComboBox(createFrame);
        yearCombo->setObjectName("yearCombo");

        horizontalLayout->addWidget(yearCombo);

        monthCombo = new QComboBox(createFrame);
        monthCombo->setObjectName("monthCombo");

        horizontalLayout->addWidget(monthCombo);

        dayCombo = new QComboBox(createFrame);
        dayCombo->setObjectName("dayCombo");

        horizontalLayout->addWidget(dayCombo);

        hourCombo = new QComboBox(createFrame);
        hourCombo->setObjectName("hourCombo");

        horizontalLayout->addWidget(hourCombo);

        chooseImageButton = new QPushButton(createFrame);
        chooseImageButton->setObjectName("chooseImageButton");

        horizontalLayout->addWidget(chooseImageButton);

        createButton = new QPushButton(createFrame);
        createButton->setObjectName("createButton");

        horizontalLayout->addWidget(createButton);


        verticalLayout->addWidget(createFrame);

        plusButton = new QPushButton(centralwidget);
        plusButton->setObjectName("plusButton");
        plusButton->setStyleSheet(QString::fromUtf8("border-radius: 30px; background-color: #0078D4; color: white; font-size: 28pt; font-weight: bold;"));

        verticalLayout->addWidget(plusButton);

        editModeButton = new QPushButton(centralwidget);
        editModeButton->setObjectName("editModeButton");

        verticalLayout->addWidget(editModeButton);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1920, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Event Countdown Manager", nullptr));
        titleInput->setPlaceholderText(QCoreApplication::translate("MainWindow", "Enter Event Title", nullptr));
        chooseImageButton->setText(QCoreApplication::translate("MainWindow", "Select Image", nullptr));
        createButton->setText(QCoreApplication::translate("MainWindow", "Create Event", nullptr));
        plusButton->setText(QCoreApplication::translate("MainWindow", "+", nullptr));
        editModeButton->setText(QCoreApplication::translate("MainWindow", "Edit Mode", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
