#pragma once

namespace cgt
{

class Clock {
public:
    Clock();

    float Tick();

private:
    uint64_t m_Last;
};

}
