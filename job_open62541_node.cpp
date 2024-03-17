#include "job_open62541_node.h"

using namespace open62541;

/* Class JobDecNode */

JobDecNode::JobDecNode(opcuac::jobsptr old) noexcept : open62541::Job(old)
{

}

/* Class JNumeric */

JNumeric::JNumeric(opcuac::jobsptr old, const int& id, const int& ns) noexcept 
        : open62541::JobDecNode(old)
{
    UA_NodeId_init(&this->target_);
    this->target_ = UA_NODEID_NUMERIC(ns, id);

    this->add_info(PRAEFIX_ID, std::to_string(id));
    this->add_info(PRAEFIX_NS, std::to_string(ns));
    this->add_info(PRAEFIX_NODE, "Numeric");
}

JNumeric::~JNumeric() noexcept
{
    /* Clean up */
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JNumeric::nodeID(void) 
{
    return this->target_;
}

/* Class JString */

JString::JString(opcuac::jobsptr old, const std::string& id, const int& ns) noexcept
        : open62541::JobDecNode(old)
{
    UA_NodeId_init(&this->target_);
    this->target_ = UA_NODEID_STRING_ALLOC(ns, id.c_str());

    this->add_info(PRAEFIX_ID, id);
    this->add_info(PRAEFIX_NS, std::to_string(ns));
    this->add_info(PRAEFIX_NODE, "String");
}

JString::~JString() noexcept
{
    /* Clean up */
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JString::nodeID(void)
{
    return this->target_;
}

/* Eof */