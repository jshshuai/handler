//
// Created by jshh on 19-10-20.
//


#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

#include <chrono>
#include <functional>
#include <string>
#include <iostream>
#include <condition_variable>

using namespace std;



class Message{
public:

    int m_what;
    long m_arg1;
    long m_arg2;
    string m_str;

   // typedef std::function<void()> Function;  //c++这种方式没法强制转换.用下面c的方式代替使用.
    typedef void(*Function)(void* args);

    Function task;

    std::chrono::system_clock::time_point when;

public:
    Message();
    Message(int what);
    Message(int what, long arg1);
    Message(int what, long arg1, long arg2);
    Message(int what, long arg1, long arg2, long uptimeMillis);
    Message(int what, long arg1, string str);
    Message(int what, long arg1, string str, long uptimeMillis);

    virtual ~Message();

    Message& operator=(const Message& msg);

    void setWhen(long uptimeMillis);

    void setFunction(Function f);

    bool operator > (const Message& msg) const {
        return (this->when > msg.when);
    }

    bool operator < (const Message& msg) const {
        return (this->when < msg.when);
    }

    bool operator <= (const Message& msg) const {
        return (this->when <= msg.when);
    }

    bool operator==(const Message& msg) const {
        return (this->m_what == msg.m_what && this->task == msg.task);
    }

    bool operator==(int what) const {
        return (this->m_what == what);
    }

private:

};

#endif
