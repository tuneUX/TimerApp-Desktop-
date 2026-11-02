/********************************************************************************
** Form generated from reading UI file 'TimerApp.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMERAPP_H
#define UI_TIMERAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TimerAppClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TimerAppClass)
    {
        if (TimerAppClass->objectName().isEmpty())
            TimerAppClass->setObjectName("TimerAppClass");
        TimerAppClass->resize(600, 400);
        menuBar = new QMenuBar(TimerAppClass);
        menuBar->setObjectName("menuBar");
        TimerAppClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TimerAppClass);
        mainToolBar->setObjectName("mainToolBar");
        TimerAppClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(TimerAppClass);
        centralWidget->setObjectName("centralWidget");
        TimerAppClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(TimerAppClass);
        statusBar->setObjectName("statusBar");
        TimerAppClass->setStatusBar(statusBar);

        retranslateUi(TimerAppClass);

        QMetaObject::connectSlotsByName(TimerAppClass);
    } // setupUi

    void retranslateUi(QMainWindow *TimerAppClass)
    {
        TimerAppClass->setWindowTitle(QCoreApplication::translate("TimerAppClass", "TimerApp", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TimerAppClass: public Ui_TimerAppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMERAPP_H
