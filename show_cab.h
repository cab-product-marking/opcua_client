#pragma once 

/* Class cab - console show */
/* ^^^^^^^^^^^^^^^^^^^^^^^^ */

#include <iostream>
#include <thread>
#include <chrono>

#include "defines.h"
#include "logger.h"

#define CAB_TIME_MS             2000

class CAB
{
public:
    CAB();

    virtual
    ~CAB();

    CAB(const CAB&) = delete;

    CAB&
    operator=(const CAB&) = delete;

    CAB(CAB&&) = delete;

    CAB&
    operator=(CAB&&) = delete;

    void 
    _cab(int length = 100);

    void 
    _cab_wait(int length = 100);

private:
    int counter_;

    std::string m_;
    int ml_;
    std::string e_;
    int el_;

};

/* Eof */