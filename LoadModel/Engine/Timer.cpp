
#include "Timer.h"
#include <GLFW/glfw3.h>
void Timer::setNewTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

Timer::~Timer()
{
}
Timer::Timer()
{

}
