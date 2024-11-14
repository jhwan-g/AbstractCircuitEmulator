#include "console.h"

cOnsole::cOnsole(QObject *parent, std::string console_name) :
    QObject(parent), name(console_name)
{}

std::string cOnsole::GetConsoleStr(){
    return console_msg;
}
void cOnsole::ClearConsole(){
    console_msg = "";
}

cOnsole& cOnsole::operator<<(std::string str){
    console_msg += str;
    emit ConsoleMsgChanged();
    return *this;
}

QString cOnsole::GetConsoleMsg(){
    std::string Msg = name + "\n" + console_msg;
    return QString(Msg.c_str());
}

cOnsole& cOnsole::operator<<(int val){
    console_msg += std::to_string(val);
    emit ConsoleMsgChanged();
    return *this;
}
