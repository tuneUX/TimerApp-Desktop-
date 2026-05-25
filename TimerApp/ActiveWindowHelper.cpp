#include "ActiveWindowHelper.h"
#include <Psapi.h>
#include<QFileInfo>

// Foreground versions
QString ActiveWindowHelper::getActiveWindowTitle() {
    return getActiveWindowTitle(GetForegroundWindow());
}

QString ActiveWindowHelper::getActiveProcessName() {
    return getActiveProcessName(GetForegroundWindow());
}

// HWND overloads
QString ActiveWindowHelper::getActiveWindowTitle(HWND hwnd) {
    if (!hwnd) return "";
    wchar_t title[256];
    GetWindowTextW(hwnd, title, 256);
    return QString::fromWCharArray(title);
}

QString ActiveWindowHelper::getActiveProcessName(HWND hwnd) {
    if (!hwnd) return "";
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProcess) return "";

    wchar_t exeName[MAX_PATH];
    if (GetModuleFileNameExW(hProcess, NULL, exeName, MAX_PATH)) {
        CloseHandle(hProcess);
         QString fullPath = QString::fromWCharArray(exeName);
         return QFileInfo(fullPath).baseName().toLower();
    }

    CloseHandle(hProcess);
    return "";
}

// Enumerate all windows
QStringList ActiveWindowHelper::getAllOpenProcesses() {
    QStringList list;

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        QStringList* list = reinterpret_cast<QStringList*>(lParam);

        if (IsWindowVisible(hwnd)) {
            QString proc = ActiveWindowHelper::getActiveProcessName(hwnd);
            QString title = ActiveWindowHelper::getActiveWindowTitle(hwnd);

            if (!proc.isEmpty() && !title.isEmpty()) {
                list->append(proc + " > " + title);
            }
        }
        return TRUE;
        }, reinterpret_cast<LPARAM>(&list));

    return list;
}
