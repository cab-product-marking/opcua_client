#include "job_dec_nodetype.h"

using namespace dec;

void
NodeType::print(std::ostream& os) const
{
    os << "Initial: " << this->initial_str_;
    return;
}

/* Class JobNumeric */

JobNumeric::JobNumeric(open62541::jsptr j, int id, int ns) noexcept : j_(j)
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

JobString::JobString(open62541::jsptr j, std::string str, int ns) noexcept : j_(j)
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