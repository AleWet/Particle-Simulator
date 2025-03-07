#include "Time.h"
#include <cmath>
#include <algorithm>

const int MAXSTEPS = 1000;

Time::Time(float fixedDeltaTime)
    : m_FixedDeltaTime(fixedDeltaTime), m_LastTime(glfwGetTime()), m_Accumulator(0.0f), m_LastFrameTime(0.0f)
{
}

int Time::update() 
{
    double currentTime = glfwGetTime();
    float frameTime = static_cast<float>(currentTime - m_LastTime);
    frameTime = std::min(frameTime, 0.25f); // Cap at 250ms prevent errors (?)
    m_LastTime = currentTime;
    m_LastFrameTime = frameTime;

    m_Accumulator += frameTime;
    // Calculate the number of fixed steps needed to cover the elapsed time.
    int steps = std::min(MAXSTEPS, static_cast<int>(std::floor(m_Accumulator / m_FixedDeltaTime)));

    // Remove the time used by these fixed steps from the accumulator.
    if (steps > 0) {
        m_Accumulator -= steps * m_FixedDeltaTime;
    }
    return steps;
}

float Time::getFixedDeltaTime() const 
{
    return m_FixedDeltaTime;
}

float Time::getInterpolationFactor() const 
{
    return m_Accumulator / m_FixedDeltaTime;
}

float Time::getLastFrameTimeMs() const {
    // Convert the last frame time from seconds to milliseconds
    return m_LastFrameTime * 1000.0f;
}

float Time::getLastfps() const
{
    return (m_LastFrameTime > 0.0f) ? (1.0f / m_LastFrameTime) : 0.0f;
}
