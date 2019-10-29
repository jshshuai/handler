
//
// Created by jshh on 19-10-20.
//

#include <chrono>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
#include "Handler.h"
#include "Message.h"
#include <sys/time.h>
#include <sys/syscall.h>


#define LOGENTER (std::cout << "This is FUNCTION " << __func__<<  std::endl)

using  namespace std;


#if 0
long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif



static Handler* pHandler = NULL;


/*
*这个只作为全局使用，postDealy这种处理，如果需要sendmessage能力，
*请自行创建实例，不要使用静态对象.
*可使用类似异步处理执行方法，异步延时处理方法，删除未执行的方法等.
*/
Handler* Handler::getInstance(){
    if(NULL == pHandler){
        static Handler handler;
        pHandler = &handler;
    }
    return pHandler;
}

Handler::Handler():Handler(NULL){};

Handler::Handler(MessageCallback cb):stop(false),stopWhenEmpty(false),mWaitState(TASK_WAIT_STATE_NONE_E){
    msgCb = cb;
    looper = std::thread(
        [this](){
            cout << "handler thread in pid:" << getpid() << " tid:"  << pthread_self() << endl;
            //  cout << "pthread_self id : " << pthread_self() << endl; ===>POSI threadid 内部转换过的
            // cout << "gettid id : " << syscall(SYS_gettid) << endl; 
            // ====>对接linux内核外部显示的id，比如top 查看对应的id
            
            for(;;)
            {
                Message msg;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    if(this->msg_Q.empty()){
                    cout << "msg_Q empty , wait..." << endl;
                    this->condition.wait(lock, [this]{  return (this->stop || this->stopWhenEmpty || !this->msg_Q.empty());});
                    mWaitState = TASK_WAIT_STATE_REFRESH_E;
//                        cout << "msg_Q empty , wait success " << endl;
//                        this->condition.wait(lock, [this]{
//                            bool bBreak = (this->stop || this->stopWhenEmpty || !this->msg_Q.empty());
//                            return bBreak;
//                        });
                    }else{
//                        chrono::system_clock::time_point beginTime = std::chrono::system_clock::now();
//                        cout << "msg_Q is not empty, now clock : " << std::chrono::system_clock::to_time_t(beginTime) << endl;
//                        cout << "msg_Q is not empty, msg_Q.back().when : " << std::chrono::system_clock::to_time_t(this->msg_Q.back().when) << endl;
                          this->condition.wait_until(lock, this->msg_Q.back().when, [this]{ return (this->stop || this->stopWhenEmpty || this->mWaitState != TASK_WAIT_STATE_NONE_E);/*|| !this->msg_Q.empty();*/ });
//                        cout << "msg_Q  wait_until success" << endl;
//                        this->condition.wait_until(lock, this->msg_Q.back().when, [this]{
//                            bool bBreak = (this->stop || this->stopWhenEmpty || this->mWaitState != TASK_WAIT_STATE_NONE_E);
//                            return bBreak;
//                        });
 //                       chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
 //                       cout << "wait_until success , now clock " << std::chrono::system_clock::to_time_t(endTime) << endl;
 //                       cout << "msg when time : " << std::chrono::system_clock::to_time_t(this->msg_Q.back().when) << endl;
                    }

                    if(this->stopWhenEmpty && this->msg_Q.empty()) {
                        cout << " handle thread stopWhenEmpty , end" << endl;
                        return;
                    }

                    if(stop){
                        msg_Q.clear();
                        cout << " handle thread stop , end" << endl;
                        return;
                    }

                    if(mWaitState == TASK_WAIT_STATE_REFRESH_E){
                        mWaitState = TASK_WAIT_STATE_NONE_E;
                        cout << "msg_Q refreh continue check" << endl;
                        continue;
                    }
                    mWaitState = TASK_WAIT_STATE_NONE_E;
                    msg = std::move(msg_Q.back());
                    msg_Q.pop_back();
                }
                this->dispatchMessage(msg);
            }
        });
}
Handler::~Handler(){
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	looper.join();
	msg_Q.clear();
}

bool Handler::setMsgCallback(MessageCallback cb){
    msgCb = cb;
    return true;
}

void Handler::dumpMsgQueue(){

    std::unique_lock<std::mutex> lock(queue_mutex);

    if(msg_Q.empty()){
        cout << "msg_Q is empty" << endl;
        return;
    }

    cout << "msg_Q size is " << msg_Q.size() << endl;

    for(std::vector<Message>::iterator i = msg_Q.begin(); i != msg_Q.end();i++){
        cout << " <" ;
        cout << "what:" << i->m_what;
        cout << " timeout:" << std::chrono::system_clock::to_time_t(i->when);
        cout << " funtion : " << (i->task == nullptr ? "null" : "valid");
        cout << ">";
    }
    cout << endl;
}

bool Handler::__syncMoidfyMsgQueue(bool bAdd, Message msg, int timeout){
    std::unique_lock<std::mutex> lock(queue_mutex);
    if(bAdd){
        msg.setWhen(timeout);
//这个其实可以去掉，检测post类型的msg同函数指针情况下进行replace
//        std::vector<Message>::iterator i = find(msg_Q.begin(),msg_Q.end(),msg);
//        if (i != msg_Q.end()){
//            msg_Q.erase(i);
//        }
        if(msg_Q.empty() || msg <= msg_Q.back()){
            if(timeout == 0) {
                mWaitState = TASK_WAIT_STATE_EXIT_E;
            }else{
                cout << "modify refresh msg.what: " << msg.m_what << endl;
                mWaitState = TASK_WAIT_STATE_REFRESH_E;
            }
            msg_Q.push_back(msg);
            condition.notify_one();
        }else{
            //这种情况下不需要通知线程重新wait timeout,仍然维持until的时间即可.
            msg_Q.push_back(msg);
            std::sort(msg_Q.begin(), msg_Q.end(),std::greater<Message>());
        }
    }else{
        bool bRefresh = false;

        if(msg.m_what == msg_Q.back().m_what){
            bRefresh = true;
        }
        while(1) {
            auto i = find(msg_Q.begin(), msg_Q.end(), msg);
            if (i != msg_Q.end()) {
                cout << "remove msg.what : " << msg.m_what << endl;
                msg_Q.erase(i);
            } else {
                break;
            }
        }
        if(bRefresh) {
            mWaitState = TASK_WAIT_STATE_REFRESH_E;
            condition.notify_one();
        }
    }
    return true;
}


void Handler::handleMessage(Message& msg){
    std::cout << "IN Handler " << __func__<< " what:" << msg.m_what <<  std::endl;
    if(msgCb != NULL)
    {
        msgCb(msg);
    }
}

bool Handler::sendMessageAtTime(Message& msg, long uptimeMillis){
    if(uptimeMillis < 0 )
        return false;
    __syncMoidfyMsgQueue(true, msg, uptimeMillis);
    return true;
}

bool Handler::sendMessage(Message& msg){
    __syncMoidfyMsgQueue(true, msg, 0);
    return true;
}

bool Handler::sendEmptyMessage(int what){
    return sendEmptyMessage(what ,0);
}

bool Handler::sendEmptyMessage(int what,long uptimeMillis){
    if(what < 0 || uptimeMillis < 0)
        return false;

    Message msg(what);
    __syncMoidfyMsgQueue(true, msg, uptimeMillis);
    return true;
}




bool Handler::post(Message::Function f){
    return postAtTime(f, NULL, 0);
}

//void* args会通过回调函数回传给使用者
bool Handler::post(Message::Function f, void* args){
	return postAtTime(f, args, 0);
}

bool Handler::postAtTime(Message::Function f, long uptimeMillis){
	return postAtTime(f, NULL, uptimeMillis);
}

/*
 * f : 需要异步回调的函数
 * args : 需要回传的参数,目前暂时设计为指针类型 可自由转换
 * uptimeMillis: 超时时间　单位毫秒
 */
bool Handler::postAtTime(Message::Function f, void* args, long uptimeMillis){
	if(!f || uptimeMillis < 0){
		return false;
	}
	Message msg(0, (long)args);
	msg.setFunction(f);
	__syncMoidfyMsgQueue(true, msg, uptimeMillis);
	return true;
}


void Handler::removeMessages(int what){
    if(what < 0)
        return;
    
    Message msg(what);
    __syncMoidfyMsgQueue(false, msg, 0);
}

void Handler::removePostFuntion(Message::Function f){
    if(f == NULL){
        return;
    }

    Message msg;
    msg.setFunction(f);
    __syncMoidfyMsgQueue(false, msg, 0);
}

void Handler::removeCallbackAndMessages(){
	std::unique_lock<std::mutex> lock(queue_mutex);
	msg_Q.clear();
}


//根据目前的实现安全退出，未完成的指令会快速执行到，不会有timeout的过程. 2019-10-22
void Handler::stopSafty(bool stopSafty){
    std::unique_lock<std::mutex> lock(queue_mutex);
    if(stopSafty){
        stopWhenEmpty = true;
    }else{
        stop = true;
    }
}


bool Handler::isQuiting(){
    std::unique_lock<std::mutex> lock(queue_mutex);
    if(stop || stopWhenEmpty)
        return true;
    return false;
}

void Handler::dispatchMessage(Message& msg){
    if(msg.task != NULL){
        msg.task((void*)msg.m_arg1);
    }else{
        if(msg.m_what < 0)
            return;
        handleMessage(msg);
    }
}
