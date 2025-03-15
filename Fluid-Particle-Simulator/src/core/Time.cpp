#include "Time.h"
#include <cmath>
#include <algorithm>
#include <deque>

const int MAXSTEPS = 100;

Time::Time(float fixedDeltaTime)
    : m_FixedDeltaTime(fixedDeltaTime), m_LastTime(glfwGetTime()),
    m_Accumulator(0.0f), m_LastFrameTime(0.0f)
{
    // Initialize tracking variables for averages
    m_FrameTimeHistory.clear();
    m_MaxFrameHistorySize = 120; // Track last 2 seconds at 60fps
}

int Time::update()
{
    double currentTime = glfwGetTime();
    float frameTime = static_cast<float>(currentTime - m_LastTime);
    frameTime = std::min(frameTime, 0.25f); // Cap at 250ms prevent errors (?)
    m_LastTime = currentTime;
    m_LastFrameTime = frameTime;

    // Add this frame's time to our history
    m_FrameTimeHistory.push_back(frameTime);

    // Keep history at maximum size by removing oldest entries
    while (m_FrameTimeHistory.size() > m_MaxFrameHistorySize) {
        m_FrameTimeHistory.pop_front();
    }

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

// Get average FPS over history window
float Time::getAverageFPS() const
{
    if (m_FrameTimeHistory.empty()) {
        return 0.0f;
    }

    // Calculate average frame time
    float totalTime = 0.0f;
    for (const auto& frameTime : m_FrameTimeHistory) {
        totalTime += frameTime;
    }

    float avgFrameTime = totalTime / m_FrameTimeHistory.size();

    // Convert to FPS
    return (avgFrameTime > 0.0f) ? (1.0f / avgFrameTime) : 0.0f;
}

// Get average milliseconds per frame over history window
float Time::getAverageFrameTimeMs() const
{
    if (m_FrameTimeHistory.empty()) {
        return 0.0f;
    }

    // Calculate average frame time
    float totalTime = 0.0f;
    for (const auto& frameTime : m_FrameTimeHistory) {
        totalTime += frameTime;
    }

    float avgFrameTime = totalTime / m_FrameTimeHistory.size();

    // Convert to milliseconds
    return avgFrameTime * 1000.0f;
}