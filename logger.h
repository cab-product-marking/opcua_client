#pragma once 

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <memory>
 
#include "common_defines.h"
#include "os_wrappers.h"
#include "job_open62541_node.h"
#include "data_open62541.h"

/* Classic log */
#define cLOG(level, message)            Logger::get_instance().log(level, message)   

/* Job log */
#define jLOG(level, message, jsptr)     Logger::get_instance().log(level, message, jsptr)

/* Data log */
#define dLOG(level, message, jsptr)     Logger::get_instance().log(level, message, jsptr)

enum class Level
{
    INFO, 
    WARNING,
    ERROR,
    JOB,
    DATA
};

/* Forward declaration */
class Job;

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
    static Logger& 
    get_instance(void);

    /* Basic log level parser */
    void 
    log(Level level, const std::string&, opcuac::jobsptr = nullptr);

    /* Time function */
    std::string 
    actual_time(void);

};

/* Eof */