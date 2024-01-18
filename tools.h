#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <atomic>
#include <map>


/* Settings */
/* ^^^^^^^^ */

// #define FEATURE_CONSTRUCTOR_VISABLE 
// #define FEATURE_DEBUG_INFO
// #define FEATURE_DEBUG_ERROR
// 

/* Console color features */
/* ^^^^^^^^^^^^^^^^^^^^^^ */

#define FONT_BLACK              "\033[0;30m"
#define FONT_RED                "\033[0;31m"
#define FONT_GREEN              "\033[0;32m"
#define FONT_BROWN              "\033[0;33m"
#define FONT_BLUE               "\033[0;34m"
#define FONT_MAGENTA            "\033[0;35m"
#define FONT_CYAN               "\033[0;36m"
#define FONT_LIGHT_GRAY         "\033[0;37m"
#define FONT_DARK_GRAY          "\033[1;30m"
#define FONT_LIGHT_BLUE         "\033[1;34m"
#define FONT_LIGHT_GREEN        "\033[1;32m"
#define FONT_LIGHT_CYAN         "\033[1;36m"
#define FONT_LIGHT_RED          "\033[1;31m"
#define FONT_LIGHT_PURPLE       "\033[1;35m"
#define FONT_YELLOW             "\033[1;33m"
#define FONT_WHITE              "\033[1;37m"
#define FONT_RESET              "\033[0m"

#define BACK_BLACK              "\033[0;40m"
#define BACK_RED                "\033[0;41m"
#define BACK_GREEN              "\033[0;42m"
#define BACK_BROWN              "\033[0;43m"
#define BACK_BLUE               "\033[0;44m"
#define BACK_MAGENTA            "\033[0;45m"
#define BACK_CYAN               "\033[0;46m"
#define BACK_LIGHT_GREY         "\033[0;47m"


/* Console input / output */
/* ^^^^^^^^^^^^^^^^^^^^^^ */

#define DEBUG(x)                std::cout << "****** test " << x << std::endl;

#define CONSOLE_LINE            std::cout << std::string(80, '-') << std::endl;
#define CONSOLE_STARS_50        std::string(50, '*')
#define CONSOLE_STARS_60        std::string(60, '*')
#define CONSOLE_STARS_70        std::string(70, '*')
#define CONSOLE_STARS_80        std::string(80, '*')
#define CONSOLE_STARS_90        std::string(90, '*')
#define CONSOLE_STARS_100       std::string(100, '*')

#define CURSOR(x,y)             "\033[" << y << ";" << x << "H"
#define CURSOR_COL(x)           "\033[" << x << "G"
#define CURSOR_ROW(y)           "\033[" << y << "d"
#define CURSOR_OFF              "\033[?25l"
#define CURSOR_ON               "\033[?25h"

#ifdef FEATURE_DEBUG_INFO
#define print_info(x)           std::cout << "*** " << x << std::endl;
#else 
#define print_info(x)
#endif   

#ifdef FEATURE_DEBUG_ERROR
#define print_error(x)          std::cerr << FONT_RED << "*** " << x << FONT_RESET << std::endl;
#else
#define print_error(x)
#endif


/* cab console shows - class CAB */
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */

#define _CAB_TIME_MS        2000
#define CAB_USER            FONT_LIGHT_BLUE << "cab" << FONT_RESET

class CAB
{
public:
    CAB()
    {
    #ifdef FEATURE_CONSTRUCTOR_VISABLE
        std::cout   << FONT_BROWN << "CAB::CAB()"
                    << FONT_RESET << std::endl;
    #endif // FEATURE_CONSTRUCTOR_VISABLE 
        m_ = {"_cab"}, e_ = {"end  "};
        ml_ = m_.length(); /* = 4 */
        el_ = ml_;
        counter_ = {0};
    }

    ~CAB()
    {
    #ifdef FEATURE_CONSTRUCTOR_VISABLE
        std::cout   << FONT_BROWN << "CAB::~CAB()"
                    << FONT_RESET << std::endl;
    #endif // FEATURE_CONSTRUCTOR_VISABLE
    }

    void 
    _cab(int length = 100)
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
            std::this_thread::sleep_for(std::chrono::milliseconds((int)_CAB_TIME_MS/length));
        }   
        std::cout.flush()   << CURSOR_COL(length - 1) 
                            << " |";
        std::this_thread::sleep_for(std::chrono::milliseconds((int)_CAB_TIME_MS/length));
        std::cout.flush()   << CURSOR_COL(length - 1) 
                            << " @";
        std::this_thread::sleep_for(std::chrono::milliseconds((int)_CAB_TIME_MS/length));
        std::cout.flush()   << CURSOR_COL(length - 1) 
                            << "  "
                            << CURSOR_ON 
                            << std::endl;
        return;
    }

    void 
    _cab_wait(int length = 100)
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

private:
    int counter_;

    std::string m_;
    int ml_;
    std::string e_;
    int el_;

};

#endif // __TOOLS_H__