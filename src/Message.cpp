//
// Created by jshh on 19-10-20.
//

#include "Message.h"
#include <chrono>

Message::Message():Message(0, 0, 0, 0){}

Message::Message(int what):Message(what, 0, 0, 0){}

Message::Message(int what, long arg1):Message(what, arg1, 0, 0){}

Message::Message(int what, long arg1, long arg2):Message(what, arg1, arg2, 0){}

Message::Message(int what, long arg1, long arg2,long uptimeMillis):m_what(what),m_arg1(arg1),m_arg2(arg2){
    when = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
    task = NULL;
}

Message::Message(int what, long arg1, string str):Message(what, arg1, str, 0){}
Message::Message(int what, long arg1, string str, long uptimeMillis):m_what(what),m_arg1(arg1),m_str(str){
    when = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
    task = NULL;
}

void Message::setWhen(long uptimeMillis){
    when = std::chrono::system_clock::now() + std::chrono::milliseconds(uptimeMillis);
}

void Message::setFunction(Function f){
    this->task = f;
}

Message::~Message(){

}

Message& Message::operator = (const Message& msg) {
    this->m_arg1 = msg.m_arg1;
    this->m_arg2 = msg.m_arg2;
    this->m_what = msg.m_what;
    this->m_str = msg.m_str;
    this->when = msg.when;
    this->task = msg.task;
    return *this;
}
