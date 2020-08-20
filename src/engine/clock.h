#pragma once

namespace cgt
{

class Clock final
{
public:
    Clock();

    float Tick();

private:
    u64 m_Last;
};

}
