#include "job_dec_nodetype.h"

using namespace dec;

/* Decorator class NodeType */

NodeType::NodeType(open62541::jsptr other) noexcept : open62541::Job(std::move(other))
{

}

void
NodeType::print(std::ostream& os) const
{
    for(auto &it : this->info_)
    {
        os << std::setw(20) << std::left << it.first << ": " << it.second << "\n";
    }
    return;
}

/* Class Numeric */

Numeric::Numeric(open62541::jsptr other, int id, int ns) noexcept 
        : dec::NodeType(std::move(other)), id_(id), ns_(ns)
{
    UA_NodeId_init(&target_);
    target_ = UA_NODEID_NUMERIC(ns, id);

    info_.emplace(PRAEFIX_ID, std::to_string(id));
    info_.emplace(PRAEFIX_NAMESPACE, std::to_string(ns));
}

Numeric::Numeric(open62541::jsptr other) noexcept : dec::NodeType(std::move(other))
{
    auto local = dynamic_cast<Numeric*>(other.get());
    /* Workflow */
    this->id_ = local->id_;
    this->ns_ = local->ns_;
    this->target_ = local->target_;
}   

Numeric::~Numeric() 
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
Numeric::nodeID(void)
{
    return target_;
}

/* Class String */

String::String(open62541::jsptr other, std::string id, int ns) noexcept 
        : dec::NodeType(std::move(other)), id_(id), ns_(ns)
{
    UA_NodeId_init(&target_);
    target_ = UA_NODEID_STRING_ALLOC(ns, id.c_str());
    
    info_.emplace(PRAEFIX_ID, id);
    info_.emplace(PRAEFIX_NAMESPACE, std::to_string(ns));
}

String::String(open62541::jsptr other) noexcept : dec::NodeType(std::move(other))
{
    auto local = dynamic_cast<String*>(other.get());
    /* Workflow */
    this->id_ = local->id_;
    this->ns_ = local->ns_;
    this->target_ = local->target_;
}

String::~String() 
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
String::nodeID(void)
{
    return target_;
}

/* Eof */