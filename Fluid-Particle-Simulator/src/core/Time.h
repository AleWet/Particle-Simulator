#pragma once

#include <GLFW/glfw3.h>

class Time {
public:
    // Constructor: fixedDeltaTime is set to 1/60 by default.
    Time(float fixedDeltaTime = 1.0f / 60.0f);

    // Call this at the beginning of your frame.
    // It calculates how many fixed update steps should be processed.
    int update();

    // Returns the fixed delta time (e.g., 1/60 sec).
    float getFixedDeltaTime() const;

    // Returns the interpolation factor for smooth rendering.
    float getInterpolationFactor() const;

    // Returns the last frame's time in milliseconds.
    float getLastFrameTimeMs() const;

    float getLastfps() const;

private:
    float m_FixedDeltaTime;   // The fixed time step (e.g., 1/60 second)
    double m_LastTime;        // Time at the end of the previous frame
    float m_Accumulator;      // Accumulated frame time
    float m_LastFrameTime;
};