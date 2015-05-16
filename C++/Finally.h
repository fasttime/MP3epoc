#pragma once

#include <functional>

class Finally
{
public:
    Finally(std::function<void ()> fn);
    Finally(const Finally &) = delete;
    ~Finally();
    Finally & operator = (const Finally &) = delete;
private:
    std::function<void ()> _fn;
};
