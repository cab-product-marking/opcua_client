#include "show_cab.h"

CAB::CAB()
{
#ifndef NDEBUG
    std::cout   << FONT_BROWN << "CAB::CAB()"
                << FONT_RESET << std::endl;
#endif // _DEBUG 
    m_ = {"_cab"}, e_ = {"end  "};
    ml_ = (int)m_.length(); /* = 4 */
    el_ = ml_;
    counter_ = {0};
}

CAB::~CAB()
{
#ifndef NDEBUG
    std::cout   << FONT_BROWN << "CAB::~CAB()"
                << FONT_RESET << std::endl;
#endif // _DEBUG
}

void 
CAB::_cab(int length)
{
    std::cout.flush()   << CURSOR_OFF 
                        << CURSOR_COL(length - 1) 
                        << "[]"
                        << CURSOR_COL(0) 
                        << m_;
    while(true)
    {
        if(length > m_.length() + 1)
        {
            m_ = ' ' + m_;
        }
        else
        {
            ml_--;
            m_ = ' ' + m_;
            m_.pop_back();
            if(ml_ == 0)
            {
                break;
            }
            std::cout.flush()   << CURSOR_COL(length + 1) 
                                << e_.substr(0, el_ - ml_);
        }
        std::cout.flush()   << CURSOR_COL(0) 
                            << m_;
        std::this_thread::sleep_for(std::chrono::milliseconds((int)CAB_TIME_MS/length));
    }   
    std::cout.flush()   << CURSOR_COL(length - 1) 
                        << " |";
    std::this_thread::sleep_for(std::chrono::milliseconds((int)CAB_TIME_MS/length));
    std::cout.flush()   << CURSOR_COL(length - 1) 
                        << " @";
    std::this_thread::sleep_for(std::chrono::milliseconds((int)CAB_TIME_MS/length));
    std::cout.flush()   << CURSOR_COL(length - 1) 
                        << "  "
                        << CURSOR_ON 
                        << std::endl;
    return;
}

void 
CAB::_cab_wait(int length)
{
    if(counter_ == 0)
    {
        std::cout.flush()   << CURSOR_OFF
                            << "[]" 
                            << CURSOR_COL(length - 1) 
                            << "[]";
    }
    counter_++;
    return;
}

/* Eof */