#pragma once
class Timer
{
public:
	static Timer& getInstance()
	{
		static Timer s_Timer;
		return s_Timer;
	}
	Timer(const Timer&) = delete;
	Timer operator=(const Timer&) = delete;
	float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
	float lastFrame = 0.0f; // ��һ֡��ʱ��
	void setNewTime();

private:
	Timer();
	~Timer();
	
};

