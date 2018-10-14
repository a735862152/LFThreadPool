/**
* @file   : task.h
* @brief  : ����Ļ����࣬������������Ľӿ�
* @version: 1.0
* @author : LWF
* @create : 2018��10��
* @date   : 2018��10��4��
*/


#include "ThreadPool.h"
#include <iostream>
#include <thread>

#include "Task.h"
#include "TaskQueue.h"

//OEThreadPool SystemThreadPool;

OEThreadPool::OEThreadPool(void)
:taskQueue_(new OETaskQueue<OETask>()), atcCurTotalThrNum_(0), atcWorking_(true) {
}

OEThreadPool::~OEThreadPool(void) {
    // @note: �����������Զ����� release �����˴���
    release();
}

// ��ʼ����Դ
int OEThreadPool::init(const ThreadPoolConfig& threadPoolConfig) {
    // ���������
    if (threadPoolConfig.dbTaskAddThreadRate < threadPoolConfig.dbTaskSubThreadRate)
        return 87;


    threadPoolConfig_.nMaxThreadsNum = threadPoolConfig.nMaxThreadsNum;
    threadPoolConfig_.nMinThreadsNum = threadPoolConfig.nMinThreadsNum;
    threadPoolConfig_.dbTaskAddThreadRate = threadPoolConfig.dbTaskAddThreadRate;
    threadPoolConfig_.dbTaskSubThreadRate = threadPoolConfig.dbTaskSubThreadRate;


    int ret = 0;
    // �����̳߳�
    if (threadPoolConfig_.nMinThreadsNum > 0)
        ret = addProThreads(threadPoolConfig_.nMinThreadsNum);


    return ret;
}

// �������
int OEThreadPool::addTask(std::shared_ptr<OETask> taskptr, bool priority) {
    const double& rate = getThreadTaskRate();
    int ret = 0;
    if (priority) {

        if (rate > 1000)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        taskQueue_->put_front(taskptr);

    }
    else {

        // �����������
        if (rate > 100) {
            taskptr->onCanceled();
            return 298;
        }


        // �������������
        taskQueue_->put_back(taskptr);

    }


    // ����Ƿ�Ҫ��չ�߳�
    if (atcCurTotalThrNum_ < threadPoolConfig_.nMaxThreadsNum
        && rate > threadPoolConfig_.dbTaskAddThreadRate)
        ret = addProThreads(1);


    return ret;
}

// ɾ�����񣨴Ӿ�������ɾ���������������û�У���ִ�ж�����û�У��еĻ�����ȡ��״̬λ��
int OEThreadPool::deleteTask(size_t nID) {
    return taskQueue_->deleteTask(nID);
}

// ɾ����������
int OEThreadPool::deleteAllTasks(void) {
    return taskQueue_->deleteAllTasks();
}

std::shared_ptr<OETask> OEThreadPool::isTaskProcessed(size_t nId) {
    return taskQueue_->isTaskProcessed(nId);
}


// �ͷ���Դ���ͷ��̳߳ء��ͷ�������У�
bool OEThreadPool::release(void) {
    // 1��ֹͣ�̳߳ء�2������������С�3���ȴ�ִ�ж���Ϊ0
    releaseThreadPool();
    taskQueue_->release();

    int i = 0;
    while (atcCurTotalThrNum_ != 0) {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // �쳣�ȴ�
        if (i++ == 10)
            exit(23);

    }

    atcCurTotalThrNum_ = 0;
    return true;
}

// ��ȡ��ǰ�߳������
double OEThreadPool::getThreadTaskRate(void) {
    // @note :����ν�̰߳�ȫ
    //std::unique_lock<std::mutex> lock(mutex_)

    if (atcCurTotalThrNum_ != 0)
        return taskQueue_->size() * 1.0 / atcCurTotalThrNum_;

    return 0;
}
// ��ǰ�߳��Ƿ���Ҫ����
bool OEThreadPool::shouldEnd(void) {

    bool bFlag = false;
    double dbThreadTaskRate = getThreadTaskRate();

    // ����߳����������
    if (!atcWorking_ || atcCurTotalThrNum_ > threadPoolConfig_.nMinThreadsNum
        && dbThreadTaskRate < threadPoolConfig_.dbTaskSubThreadRate)
        bFlag = true;

    return bFlag;
}
// ���ָ�������Ĵ����߳�
int OEThreadPool::addProThreads(int nThreadsNum) {

    try {

        for (; nThreadsNum > 0; --nThreadsNum)
            std::thread(&OEThreadPool::taskProcThread, this).detach();

    }
    catch (...) {
        return 155;
    }

    return 0;
}
// �ͷ��̳߳�
bool OEThreadPool::releaseThreadPool(void) {
    threadPoolConfig_.nMinThreadsNum = 0;
    threadPoolConfig_.dbTaskSubThreadRate = 0;
    atcWorking_ = false;
    return true;
}

// �������̺߳���
void OEThreadPool::taskProcThread(void) {
    int nTaskProcRet = 0;
    // �߳�����
    atcCurTotalThrNum_.fetch_add(1);
    std::chrono::milliseconds mills_sleep(500);


    std::shared_ptr<OETask> pTask;
    while (atcWorking_) {
        // ��ȡ����
        pTask = taskQueue_->get();
        if (pTask == nullptr) {

            if (shouldEnd())
                break;

            // ����˯�߳�
            taskQueue_->wait(mills_sleep);
            continue;

        }


        // �������ȡ��״̬
        if (pTask->isCancelRequired())
            pTask->onCanceled();
        else
            // ��������
            pTask->onCompleted(pTask->doWork());


        // ����������������Ƴ�����
        taskQueue_->onTaskFinished(pTask->getID());


        // �ж��߳��Ƿ���Ҫ����
        if (shouldEnd())
            break;

    }

    // �̸߳�����һ
    atcCurTotalThrNum_.fetch_sub(1);
}