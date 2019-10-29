//
// Created by jshh on 19-10-22.
//

#include <iostream>
#include "Message.h"
#include "Handler.h"
#include "Message.h"
#include <unistd.h>
#include <stdio.h>

using namespace std;

void testFuntion1(void * args){
    static int cnt = 0;

    cout << "testFunction1 in cnt : "  << ++cnt << endl;
}

void testFuntion2(void * args){

    cout << "testFunction2 in " << endl;
}

void testFuntion3(void * args){

    cout << "testFunction3 in  args " << (long)args << endl;
}

void testFuntion4(void * args){

    cout << "testFunction4 in " << endl;
}



void postFuntion(void * args){

    cout << "postFuntion in " << endl;
}

void postFuntionTimeout(void * args){

    cout << "postFuntionTimeout in " << endl;
}

void emptyMsgFunction(void * args){

    cout << "emptyMsgFunction in " << endl;
}

void emptyDelayMsgFunction1(void * args){

    cout << "emptyDelayMsgFunction1 in " << endl;
}

void emptyDelayMsgFunction2(void * args){

    cout << "emptyDelayMsgFunction2 in " << endl;
}

void  handleMessge(Message& msg){

    cout << "handleMessage : " << msg.m_what << endl;

}


int main(int argc, char* argv[]){

    cout << "main in" << endl;

//    Manager manager;
//    manager.init();
//
    Handler handler;
    Handler handler2;
    Handler::getInstance();
    sleep(5);
    
//
//    Message msg1(1);
//    handler.sendMessageAtTime(msg1, 5000);
    handler.sendEmptyMessage(2, 4000);
    handler.sendEmptyMessage(2, 5000);
    handler.sendEmptyMessage(2, 6000);
    handler.sendEmptyMessage(3, 2000);
    handler.sendEmptyMessage(5, 6000);
    handler.sendEmptyMessage(6, 7000);
    handler.sendEmptyMessage(7, 8000);
    handler.removeMessages(6);
    handler.removeMessages(2);
    handler.postAtTime(testFuntion1, 5000);
    handler.post(testFuntion2);
    handler.postAtTime(testFuntion2, 6000);
  //  handler.removePostFuntion(testFuntion1);
  //  handler.removePostFuntion(testFuntion2);
    Handler::getInstance()->post(testFuntion3, (void*) 3456);
    handler.post(testFuntion3, (void *)12345);

    handler2.sendEmptyMessage(100, 20*1000);
    Handler::getInstance()->sendEmptyMessage(200, 30*1000);
    

//    handler.dumpMsgQueue();
//    handler.stopSafty(true);

//    handler.postAtTime(testFuntion1, 10000);
//    sleep(3);
//    handler.postAtTime(testFuntion1, 10000);
//    sleep(2);
//    handler.postAtTime(testFuntion1, 10000);
//    sleep(4);
//    handler.postAtTime(testFuntion1, 10000);
//    sleep(5);
//    handler.postAtTime(testFuntion1, 10000);
//    sleep(7);
//    handler.postAtTime(testFuntion1, 10000);
//    sleep(8);
//    handler.postAtTime(testFuntion1, 10000);

//    int cnt = 10;
//
//    while(cnt){
//        sleep(1);
//        handler.postAtTime(testFuntion2, 10000);
//    }

//    sleep(1);
//    cout << "postAtTime testFuntion3 now\n" << endl;
//    handler.post(testFuntion3);
//
//
//    sleep(10);
//    cout << "post testFuntion4\n" << endl;
//    handler.post(testFuntion4);
//
//    cout << "post testFuntion4\n" << endl;
//    handler.post(testFuntion4);



    while (1){
        sleep(1);
        //    std::this_thread::sleep_for(std::chrono::seconds(1000));
    }

    return 0;
}