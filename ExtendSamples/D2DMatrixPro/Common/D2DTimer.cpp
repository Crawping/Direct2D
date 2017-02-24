#include <windows.h>
#include "D2DTimer.h"

//
// D2DTimer Implement
//

D2DTimer::D2DTimer()
	: m_secondsPerCount(0.0)
	, m_deltaTime(-1.0)
	, m_baseTime(0)
	, m_pausedTime(0)
	, m_prevTime(0)
	, m_currTime(0)
	, m_stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

float D2DTimer::DeltaTime() const
{ 
	return (float)m_deltaTime; 
}

// �����Ե���Reset()����֮�����ʱ�䣬��������ͣʱ��
float D2DTimer::TotalTime()const
{
	// ���������ͣ״̬���������������ͣ��ʼ֮���ʱ�䡣
	// ����,�������֮ǰ�Ѿ��й���ͣ,��m_stopTime - m_baseTime�������ͣʱ��, ���ǲ�����������ͣʱ�䣬
	// ��˻�Ҫ��ȥ��ͣʱ�䣺  
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  m_baseTime       m_stopTime        startTime     m_stopTime    m_currTime

	if( m_stopped )
	{
		return (float)(((m_stopTime - m_pausedTime)-m_baseTime)*m_secondsPerCount);
	}

	// m_currTime - m_baseTime������ͣʱ��,�����ǲ��������ͣʱ�䣬
	// ������Ǵ�m_currTime��Ҫ��ȥm_pausedTime��
	//
	//  (m_currTime - m_pausedTime) - m_baseTime 
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  m_baseTime       m_stopTime        startTime     m_currTime

	else
	{
		return (float)(((m_currTime-m_pausedTime)-m_baseTime)*m_secondsPerCount);
	}
}

void D2DTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_stopped  = false;
}

void D2DTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


	// �ۼ���ͣ�뿪ʼ֮�����ŵ�ʱ��
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  m_baseTime       m_stopTime        startTime     

	// ����Դ�����ͣ״̬
	if( m_stopped )
	{
		// ���ۼ���ͣʱ��
		m_pausedTime += (startTime - m_stopTime); 
		// ��Ϊ�������¿�ʼ��ʱ�����m_prevTime��ֵ�Ͳ���ȷ�ˣ�
		// Ҫ��������Ϊ��ǰʱ�� 
		m_prevTime = startTime;
		// ȡ����ͣ״̬
		m_stopTime = 0;      
		m_stopped  = false;
	}
}

void D2DTimer::Stop()
{
	// �����������ͣ״̬�����Թ�����Ĳ���
	if( !m_stopped )
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// ��¼��ͣ��ʱ�䣬�����ñ�ʾ��ͣ״̬�ı�־
		m_stopTime = currTime;
		m_stopped  = true;
	}
}

void D2DTimer::Tick()
{
	if( m_stopped )
	{
		m_deltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// ��ǰ֡����һ֮֡���ʱ���
	m_deltaTime = (m_currTime - m_prevTime)*m_secondsPerCount;

	// Ϊ������һ֡��׼��
	m_prevTime = m_currTime;

	// ȷ����Ϊ��ֵ��DXSDK�е�CDXUTTimer�ᵽ����������������˽ڵ�ģʽ
	// ���л�����һ����������m_deltaTime���Ϊ��ֵ��
	if(m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}