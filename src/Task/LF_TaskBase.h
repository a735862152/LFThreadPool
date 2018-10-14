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
	// ��������ӿڣ�����ͨ����ʵ�־�������
	virtual int doWork(void) = 0;

	// ������ȡ���ص�
	virtual int	onCanceled(void)
	{
		return 1;
	}
	// ���������
	virtual int	onCompleted(int)
	{
		return 1;
	}

	// �����Ƿ�ʱ
	virtual bool isTimeout(const LF_clock_t& now)
	{
		return ((now - m_createTime) > m_timeOut);
	}

	// ��ȡ����ID

	LF_size_t	getID(void)
	{
		return m_id;
	}

	// ��ȡ����״̬
	bool isCancel(void)
	{
		return m_status;
	}

	// ��������ȡ��״̬
	void setCancel(void)
	{
		m_status = false;
	}
protected:
	// �����Ψһ��ʶ
	LF_size_t                      m_id;
	// ���񴴽�ʱ�� ����Unixʱ�����
	LF_clock_t                     m_createTime;
private:
	static std::atomic<LF_size_t>  m_nReqID;
	// ����״̬
	std::atomic<bool>           m_status;
	const LF_int	m_timeOut;
};
__declspec(selectany) std::atomic<LF_size_t> LFnamespace::LFBaseClass::m_nReqID = 100000;
