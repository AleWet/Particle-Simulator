#pragma once
#include <GLFW/glfw3.h>
#include <deque>

class Time {
private:
    float m_FixedDeltaTime;
    double m_LastTime;
    float m_Accumulator;
    float m_LastFrameTime;
    
    // For tracking average performance
    std::deque<float> m_FrameTimeHistory;
    size_t m_MaxFrameHistorySize;

public:
    Time(float fixedDeltaTime);
    int update();
    float getFixedDeltaTime() const;
    float getInterpolationFactor() const;
    float getLastFrameTimeMs() const;
    float getLastfps() const;
    
    // better metrics functions
    float getAverageFPS() const;
    float getAverageFrameTimeMs() const;
};