#pragma once
#include <QString>
#include <QFileInfo>

// Normalize site name from window title
inline QString extractSiteName(const QString& title) {
    QString lower = title.toLower();

    lower.replace(QChar(0x200B), ""); 
    lower = lower.trimmed();

    // Keyword-based normalization
    if (lower.contains("youtube"))   return "youtube";
    if (lower.contains("gmail"))     return "gmail";
    if (lower.contains("facebook"))  return "facebook";
    if (lower.contains("outlook"))   return "outlook";
    if (lower.contains("twitter"))   return "twitter";
    if (lower.contains("tiktok"))    return "tiktok";


    lower.remove(QRegularExpression("^\\(\\d+\\)\\s*"));

    int dashIndex = lower.indexOf(" - "); 
    if (dashIndex != -1) { 
        
        lower = lower.left(dashIndex); 
    }
 
    return lower;
}



// Build a normalized key: process + site
inline QString makeKey(const QString& processPath, const QString& windowTitle) {
    // Always normalize process name the same way
    QString processName = QFileInfo(processPath).baseName().toLower();
    QString site = extractSiteName(windowTitle);
    return processName + ":" + site;
}
