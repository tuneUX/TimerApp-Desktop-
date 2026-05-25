#include "TimerApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet(
    "QPushButton{background color:#3498db;color:Add8e6;border-radius:6px;padding:6px}"
    "QpushButton:hover{background-color:2980b9;}"
    "QLabel { font-size: 16px; color: #2ecc71; }"


    );
    TimerApp window;
    window.show();
    return app.exec();
}
