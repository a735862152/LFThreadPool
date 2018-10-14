/**
* @file   : taskqueue.h
* @brief  : ������У���Ҫ��֤�����ִ�кͷ��ʰ�ȫ
* @version: 1.0
* @author : LWF
* @create : 2018��10��
* @date   : 2018��10��4��
*/


#ifndef __OETASKQUEUE_H__
#define __OETASKQUEUE_H__

#include <deque>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <memory>
#include <thread>


// 1. �������  (put)
// 2. ��ȡ����  (get)
// 3. �������  (onTaskFinished)
// 4. ����ȴ�  (wait)
// end. �ͷ������б� (release)
/// @class : ���������
template<class T>
class OETaskQueue {

private:

    /// �����������                               
    std::mutex                          mutexQueue_;
    std::deque<std::shared_ptr<T> >     queue_;
    /// ��������                                      
    std::mutex                          mutexConditPut_;
    std::condition_variable             conditPut_;
    /// ��������map                                   
    std::mutex                          mutexDoingTask_;
    std::unordered_map<size_t, std::shared_ptr<T> > mapDoingTask_;

public:

    /**
    * @brief ������е�ĩβ��������
    * @param ��task ������
    */
    void put_back(std::shared_ptr<T> &task) {
        std::unique_lock<std::mutex> lock(mutexQueue_);

        queue_.push_back(task);

        conditPut_.notify_one();
    }
    /**
    * @brief ������е�ͷ����������
    * @param ��task ������
    */
    void put_front(std::shared_ptr<T> &task) {
        std::unique_lock<std::mutex> lock(mutexQueue_);

        queue_.push_front(task);

        conditPut_.notify_one();
    }
    /**
    * @brief ����ȡ���ף���������ӵ����������б��У�
    * @return��������
    */
    std::shared_ptr<T> get(void) {

        std::unique_lock<std::mutex> lock(mutexQueue_);

        if (queue_.empty())
            return nullptr;


        std::lock_guard<std::mutex> lock_doing_task(mutexDoingTask_);
        std::shared_ptr<T> &task = queue_.front();
        mapDoingTask_.insert(std::make_pair(task->getID(), task));
        queue_.pop_front();

        return task;
    }
    /**
    * @brief ����ȡ����˫������Ĵ�С
    * @return����С
    */
    size_t size(void) {
        std::unique_lock<std::mutex> lock(mutexQueue_);
        return queue_.size();
    }

    /**
    * @brief ���ͷŶ���
    */
    void release(void) {

        deleteAllTasks();


        conditPut_.notify_all();
    }
    /**
    * @brief ��ɾ�����񣨴Ӿ�������ɾ���������������û�У���ִ�ж�����û�У��еĻ�����ȡ��״̬λ��
    * @param ��nID ����ı��
    * @return���ɹ�����0 ʧ�ܷ��ط�0
    */
    int deleteTask(size_t nID) {

        std::unique_lock<std::mutex> lock(mutexQueue_, std::defer_lock);
        lock.lock();


        // std::deque<std::shared_ptr<OETask>>::iterator
        auto it = queue_.begin();
        for (; it != queue_.end(); ++it) {
            if ((*it)->getID() == nID) {
                queue_.erase(it);
                lock.unlock();
                return 0;
            }
        }
        //������߼����ܻ��������������Ҫ��ʱ�ͷ�
        lock.unlock();

        // ��ͼȡ������ִ�е�����
        {
            std::lock_guard<std::mutex> lock_doing_task(mutexDoingTask_);
            // std::unordered_map<int64_t, std::shared_ptr<OETask>>::iterator
            auto it_map = mapDoingTask_.find(nID);
            if (it_map != mapDoingTask_.end())
                it_map->second->setCancelRequired();
        }




        //����ִ������ٷ���
        while (mapDoingTask_.count(nID))
            std::this_thread::sleep_for(std::chrono::milliseconds(20));

        return 0;
    }
    /**
    * @brief ��ɾ����������
    * @return���ɹ�����0 ʧ�ܷ��ط�0
    */
    int deleteAllTasks(void) {


        std::unique_lock<std::mutex> lock(mutexQueue_, std::defer_lock);
        lock.lock();

        if (!queue_.empty())
            queue_.clear();//���


        {
            std::lock_guard<std::mutex> lock_doing_task(mutexDoingTask_);
            if (!mapDoingTask_.empty()) {

                // std::unordered_map<int64_t, std::shared_ptr<OETask>>::iterator
                auto it_map = mapDoingTask_.begin();

                for (; it_map != mapDoingTask_.end(); ++it_map)
                    it_map->second->setCancelRequired();

            }
        }

        lock.unlock();


        //����ִ������ٷ���
        while (mapDoingTask_.empty())
            std::this_thread::sleep_for(std::chrono::milliseconds(50));


        return 0;
    }
    /**
    * @brief ��������ɻص����������б���ɾ��ָ������
    * @param ��nID ����ı��
    * @return���ɹ�����0 ʧ�ܷ��ط�0
    */
    int onTaskFinished(size_t nID) {

        std::lock_guard<std::mutex> lock_doing_task(mutexDoingTask_);

        // std::unordered_map<size_t, std::shared_ptr<OETask>>::iterator
        auto it_map = mapDoingTask_.find(nID);

        if (it_map != mapDoingTask_.end())
            mapDoingTask_.erase(it_map);

        return 0;
    }
    /**
    * @brief ���ж������Ƿ�ִ�����
    * @param ��nID ����ı��
    * @return��������
    */
    std::shared_ptr<T> isTaskProcessed(size_t nId) {
        std::lock_guard<std::mutex> lock_queue(mutexQueue_);
        // std::deque<std::shared_ptr<OETask>>::iterator
        auto it = queue_.begin();
        for (; it != queue_.end(); ++it) {

            if ((*it)->getID() == nId)
                return *it;

        }

        std::lock_guard<std::mutex> lock_doing_task(mutexDoingTask_);
        // std::unordered_map<int64_t, std::shared_ptr<OETask>>::iterator
        auto it_map = mapDoingTask_.find(nId);
        if (it_map != mapDoingTask_.end())
            return it_map->second;


        return nullptr;
    }

    /**
    * @brief ���ȴ������񵽴����ʱ����ʱ�Զ����ѣ�
    * @param ��millsec ��ʱʱ�䣨���룩
    * @return���ɹ�����true ʧ�ܷ���false
    */
    bool wait(std::chrono::milliseconds millsec) {
        std::unique_lock<std::mutex> lock(mutexConditPut_);
        conditPut_.wait_for(lock, millsec);

        return true;
    }

};

#endif // __OETASKQUEUE_H__