/**
* @file   : threadpool.h
* @brief  : �̳߳��࣬��Ҫ������̵߳Ĺ�������
* @version: 1.0
* @author : LWF
* @create : 2018��10��
* @date   : 2018��10��4��
*/


#ifndef __OETHREADPOOL_H__
#define __OETHREADPOOL_H__


#include <atomic>
#include <memory>
#include <mutex>

#include "TaskQueue.h"

// 1. �̳߳صĳ�ʼ��  (init)
//  while:
//    1. ����������
//    2. ��������̳߳ص���  addTask
//    3. �̳߳��첽����
// 2. �����̳߳���Դ  (release)

class OETask;

/// ���������
class OEThreadPool
{
public:
    /// �̳߳����ò���
    typedef struct tagThreadPoolConfig {
        int     nMaxThreadsNum;		    /// ����߳�����
        int     nMinThreadsNum;		    /// ��С�߳�����
        double  dbTaskAddThreadRate;    /// �� ����߳������ (�����������߳�������ʲô������ʱ��ż�)
        double  dbTaskSubThreadRate;    /// �� ��С�߳������ (�����������߳�������ʲô������ʱ��ż�)
    } ThreadPoolConfig;

private:
    /// �������
    std::shared_ptr<OETaskQueue<OETask> >   taskQueue_;

    /// �̳߳����ã������С�߳�����Ϊ1�����ʾ��Ҫһ����פ�Ĵ����̣߳�
    ThreadPoolConfig                        threadPoolConfig_;
    /// �̳߳��Ƿ�Ҫ�����
    std::atomic<bool>                       atcWorking_;
    /// ��ǰ�̸߳���
    std::atomic<int>                        atcCurTotalThrNum_;

public:

    OEThreadPool(void);
    ~OEThreadPool(void);

    /**
    * @brief ���̳߳���Դ���ó�ʼ��
    * @param ��config ��ʼ����������Ϣ
    * @return��0 ִ�гɹ�  ��0 ִ��ʧ��
    */
    int 
	init(const ThreadPoolConfig& config);
    /**
    * @brief ���ͷ���Դ���ͷ��̳߳ء��ͷ�������У�
    * @return��true ִ�гɹ�  false ִ��ʧ��
    */
    bool 
	release(void);

    /**
    * @brief ���������
    * @param ��taskptr ������
    * @param ��priority �Ƿ����޴��� true�����ȴ���
    * @return��0 ִ�гɹ�  ��0 ִ��ʧ��
    */
    int 
	addTask(std::shared_ptr<OETask> taskptr, bool priority = false);

    /**
    * @brief ��ɾ�����񣨴Ӿ�������ɾ���������������û�У���ִ�ж�����û�У��еĻ�����ȡ��״̬λ��
    * @param ��nID ������
    * @return��0 ִ�гɹ�  ��0 ִ��ʧ��
    */
    int 
	deleteTask(size_t nID);

    /**
    * @brief ��ɾ����������
    * @return��0 ִ�гɹ�  ��0 ִ��ʧ��
    */
    int 
	deleteAllTasks(void);
    /**
    * @brief ���ж������Ƿ�ִ�����
    * @param ��nID ������
    * @return��ִ����ϣ�ִ���귵��null�����򷵻�����ָ��
    */
    std::shared_ptr<OETask> 
	isTaskProcessed(size_t nId);

private:
    /**
    * @brief ����ȡ��ǰ�߳������
    * @return���߳������
    */
    double 
	getThreadTaskRate(void);
    /**
    * @brief ����ǰ�߳��Ƿ���Ҫ����
    * @return��true:���Խ��� false:�����Խ���
    * @note  ���ѿ��ǵ���С�߳�����
    */
    bool 
	shouldEnd(void);
    /**
    * @brief �����ָ�������Ĵ����߳�
    * @param ��nThreadsNum ��ӵ��߳�����
    * @return��0 ִ�гɹ�  ��0 ִ��ʧ��
    */
    int 
	addProThreads(int nThreadsNum);
    /**
    * @brief ���ͷ��̳߳�
    * @return��true ִ�гɹ�  false ִ��ʧ��
    */
    bool 
	releaseThreadPool(void);
    /**
    * @brief ���������̺߳���
    */
    void 
	taskProcThread(void);

};

// extern OEThreadPool SystemThreadPool;

#endif // __OETHREADPOOL_H__