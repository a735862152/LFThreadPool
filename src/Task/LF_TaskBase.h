/*----------------------------------------------------------------
* Copyright @WF.Lin 2018. All rights reserved.
* Filename    : LF_TaskBase.h
* Description :
* Author      ; WF.Lin
* Email       : a735862152@163.com
* Git         : github.com/a735862152
* Creat time  ; 2018:10:5 00:25
* Update time ; 2018:10:5 00:25
* Version     ; v1.0.0.0
* History     ;
------------------------------------------------------------------*/
#pragma once
#include <ctime>
#include <atomic>
#include "LF_BaseType.h"

namespace LFnamespace
{
	class LFBaseClass;
}

class LFnamespace::LFBaseClass
{
public:
	LFBaseClass(LF_clock_t timeout):m_id(m_nReqID++), m_createTime(std::clock()), m_status(true), m_timeOut(timeout)
	{};
	virtual ~LFBaseClass(void)
	{};
	// 任务类虚接口，子类通过他实现具体事务
	virtual int doWork(void) = 0;

	// 任务已取消回调
	virtual int	onCanceled(void)
	{
		return 1;
	}
	// 任务已完成
	virtual int	onCompleted(int)
	{
		return 1;
	}

	// 任务是否超时
	virtual bool isTimeout(const LF_clock_t& now)
	{
		return ((now - m_createTime) > m_timeOut);
	}

	// 获取任务ID

	LF_size_t	getID(void)
	{
		return m_id;
	}

	// 获取任务状态
	bool isCancel(void)
	{
		return m_status;
	}

	// 设置任务取消状态
	void setCancel(void)
	{
		m_status = false;
	}
protected:
	// 任务的唯一标识
	LF_size_t                      m_id;
	// 任务创建时间 （非Unix时间戳）
	LF_clock_t                     m_createTime;
private:
	static std::atomic<LF_size_t>  m_nReqID;
	// 任务状态
	std::atomic<bool>           m_status;
	const LF_int	m_timeOut;
};
__declspec(selectany) std::atomic<LF_size_t> LFnamespace::LFBaseClass::m_nReqID = 100000;
