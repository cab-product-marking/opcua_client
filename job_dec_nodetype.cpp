#include "job_dec_nodetype.h"

using namespace dec;

/* Decorator class NodeType */

NodeType::NodeType(open62541::jsptr other) noexcept : open62541::Job(std::move(other))
{

}

/* Class JNumeric */

JNumeric::JNumeric(open62541::jsptr other, int id, int ns) noexcept 
        : dec::NodeType(std::move(other)), id_(id), ns_(ns)
{
    UA_NodeId_init(&target_);
    target_ = UA_NODEID_NUMERIC(ns, id);

    info_.emplace(PRAEFIX_ID, std::to_string(id));
    info_.emplace(PRAEFIX_NAMESPACE, std::to_string(ns));
}

JNumeric::JNumeric(open62541::jsptr other) noexcept : dec::NodeType(std::move(other))
{
    auto local = dynamic_cast<JNumeric*>(other.get());
    /* Workflow */
    this->id_ = local->id_;
    this->ns_ = local->ns_;
    this->target_ = local->target_;
}   

JNumeric::~JNumeric() 
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JNumeric::nodeID(void)
{
    return target_;
}

/* Class JString */

JString::JString(open62541::jsptr other, std::string id, int ns) noexcept 
        : dec::NodeType(std::move(other)), id_(id), ns_(ns)
{
    UA_NodeId_init(&target_);
    target_ = UA_NODEID_STRING_ALLOC(ns, id.c_str());
    
    info_.emplace(PRAEFIX_ID, id);
    info_.emplace(PRAEFIX_NAMESPACE, std::to_string(ns));
}

JString::JString(open62541::jsptr other) noexcept : dec::NodeType(std::move(other))
{
    auto local = dynamic_cast<JString*>(other.get());
    /* Workflow */
    this->id_ = local->id_;
    this->ns_ = local->ns_;
    this->target_ = local->target_;
}

JString::~JString() 
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
JString::nodeID(void)
{
    return target_;
}

/* Eof */