#include "job_open62541.h"

using namespace open62541;

/* Class JobNumeric */

JobNumeric::JobNumeric(int id, int ns) noexcept : id_(id), ns_(ns)
{
    UA_NodeId_init(&target_);
    target_ = UA_NODEID_NUMERIC(ns, id);
}

JobNumeric::~JobNumeric() 
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JobNumeric::nodeID(void)
{
    return target_;
}



/* Class JobString */

JobString::JobString(std::string str, int ns) noexcept : str_(str), ns_(ns)
{
    UA_NodeId_init(&target_);
    target_ = UA_NODEID_STRING_ALLOC(ns, str.c_str());
}

JobString::~JobString() 
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JobString::nodeID(void)
{
    return target_;
}

/* Eof */