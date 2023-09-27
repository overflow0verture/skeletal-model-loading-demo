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
	float deltaTime = 0.0f; // 当前帧与上一帧的时间差
	float lastFrame = 0.0f; // 上一帧的时间
	void setNewTime();

private:
	Timer();
	~Timer();
	
};

