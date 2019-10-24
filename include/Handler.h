//
// Created by jshh on 19-10-20.
//


#pragma once
#ifndef HANDLER_H
#define HANDLER_H

#include <chrono>
#include <map>
#include <mutex>
#include <vector>
#include <thread>


#include "Message.h"

/*
 * Handler will run in it's own thread, you don't want to care about it.
 * Message will be proccess by the Handler. Two ways to add your task to the Handler.
 * 1. send message to the handler
 * 2. post the task(Function) to handler
 */

class Handler{

public:
	typedef enum TaskWiatStates{
		TASK_WAIT_STATE_NONE_E = 0,
		TASK_WAIT_STATE_REFRESH_E,   //refresh
		TASK_WAIT_STATE_EXIT_E
	}TASK_WAIT_STATE_E;

    typedef std::function<void(Message& msg)> MessageCallback;

public:
	Handler();
    Handler(MessageCallback cb);
	virtual ~Handler();

	bool sendMessageAtTime(Message& msg, long uptimeMillis);
	bool sendMessage(Message& msg);
	bool sendEmptyMessage(int what);
	bool sendEmptyMessage(int what, long uptimeMillis);

	bool post(Message::Function f);
	bool postAtTime(Message::Function f, long uptimeMillis);

	void removeMessages(int what);
	void removeCallbackAndMessages();

	void stopSafty(bool stopSafty);

	bool isQuiting();

	virtual void handleMessage(Message& msg);

	void dispatchMessage(Message& msg);

    bool setMsgCallback(MessageCallback cb);
    
	void dumpMsgQueue();

	/*
	 * for msgQueue sorted when insert,
	 * ascending order
	 */
	template<class T>
	class ValComp {
	public:
		bool operator()(const T& t1,const T& t2) const {
			return (t1 < t2);
		}

	};


private:
	bool __syncMoidfyMsgQueue(bool bAdd, Message msg, int timeout);

private:
	std::vector<Message> msg_Q;

	std::mutex queue_mutex;
	std::condition_variable condition;
	std::thread looper;
	bool stop;
	bool stopWhenEmpty;
	TASK_WAIT_STATE_E mWaitState;
    MessageCallback msgCb;
};

#endif
