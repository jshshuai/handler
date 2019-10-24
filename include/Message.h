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
    int m_arg1;
    int m_arg2;
    string m_str;
    typedef std::function<void()> Function;
    Function task;

    std::chrono::system_clock::time_point when;

public:
	Message();
	Message(int what);
	Message(int what, int arg1);
	Message(int what, int arg1, int arg2);
	Message(int what, int arg1, int arg2, long uptimeMillis);
    Message(int what, int arg1, string str);
    Message(int what, int arg1, string str, long uptimeMillis);

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
		return (this->m_what == msg.m_what);
	}

	bool operator==(int what) const {
		return (this->m_what == what);
	}

private:

};

#endif
