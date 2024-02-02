#pragma once 

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <memory>
 
#include "defines.h"
#include "local_set.h"
#include "data_types.h"
// #include "job.h"

#ifndef NDEBUG
/* Classic log */
#define cLOG(level, message)        Logger::get_instance().log(level, message)   
#endif

/* Job log */
#define jLOG(level, message, ptr)   Logger::get_instance().log(level, message, ptr)
/* Data log */
// #define dLOG

enum class Level
{
    INFO, 
    WARNING,
    ERROR,
    JOB,
    DATA
};

/**
 * Class Logger
 * ^^^^^^^^^^^^
*/
class Logger
{
public:
    Logger();

    virtual 
    ~Logger();

    Logger(const Logger&) = delete;

    Logger&
    operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;

    Logger&
    operator=(Logger&&) = delete;

    /* Return static logger instance */
    static Logger& get_instance(void);

    /* Basic log level parser */
    void 
    log(Level level, const std::string& message, 
            std::shared_ptr<JOB> sptr = nullptr);

    /* Time function */
    std::string 
    actual_time(void);

};

/* Eof */