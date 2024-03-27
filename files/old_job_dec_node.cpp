#include "old_job_dec_node.h"

using namespace dec;

/* Class JNumeric */

JNumeric::JNumeric(opcuac::jobsptr other, int id, int ns) noexcept 
        : dec::JobDecoratorNode(other), id_(id), ns_(ns)
{
    UA_NodeId_init(&this->target_);
    this->target_ = UA_NODEID_NUMERIC(ns, id);

    this->wrapped_job_->add_info(PRAEFIX_ID, std::to_string(id));
    this->wrapped_job_->add_info(PRAEFIX_NAMESPACE, std::to_string(ns));
}

JNumeric::~JNumeric() noexcept
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JNumeric::nodeID(void)
{
    return this->target_;
}

/* Class JString */

JString::JString(opcuac::jobsptr other, std::string id, int ns) noexcept 
        : dec::JobDecoratorNode(other), id_(id), ns_(ns)
{
    UA_NodeId_init(&this->target_);
    this->target_ = UA_NODEID_STRING_ALLOC(ns, id.c_str());
    
    this->wrapped_job_->add_info(PRAEFIX_ID, id);
    this->wrapped_job_->add_info(PRAEFIX_NAMESPACE, std::to_string(ns));
}

JString::~JString() noexcept
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JString::nodeID(void)
{
    return this->target_;
}

/* Eof */