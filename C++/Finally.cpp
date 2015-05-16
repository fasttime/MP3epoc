#include "Finally.h"

using namespace std;

Finally::Finally(function<void ()> fn): _fn(fn) { }

Finally::~Finally()
{
    if (_fn != nullptr)
    {
        _fn();
        _fn = nullptr;
    }
}
