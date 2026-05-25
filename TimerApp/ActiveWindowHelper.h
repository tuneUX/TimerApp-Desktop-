#pragma once
#include <QString>
#include <QStringList>
#include <windows.h>

class ActiveWindowHelper {
public:
    // Foreground window versions
    static QString getActiveWindowTitle();
    static QString getActiveProcessName();

    // Overloads for specific HWND
    static QString getActiveWindowTitle(HWND hwnd);
    static QString getActiveProcessName(HWND hwnd);

    // Enumerate all open processes/windows
    static QStringList getAllOpenProcesses();
};
