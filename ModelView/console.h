#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <QObject>

class cOnsole: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString consoleMsg READ GetConsoleMsg NOTIFY ConsoleMsgChanged)
    std::string console_msg;
    std::string name;

public:
    explicit cOnsole(QObject *parent = nullptr, std::string console_name = "default console");
    std::string GetConsoleStr();
    void ClearConsole();
    cOnsole& operator<<(std::string str);
    cOnsole& operator<<(int val);
    QString GetConsoleMsg();

signals:
    void ConsoleMsgChanged();
};

#endif // CONSOLE_H
