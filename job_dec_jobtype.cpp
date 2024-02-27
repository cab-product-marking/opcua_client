#include "job_dec_jobtype.h"

using namespace dec;

/* Decorator class JobType */

JobType::JobType(open62541::jsptr other) noexcept : open62541::Job(std::move(other))
{

}

/* Class MitemAdd */

MitemAdd::MitemAdd(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "MitemAdd");
}

std::string
MitemAdd::iam(void) const 
{
    return "MitemAdd";
}

/* Class MitemDel */

MitemDel::MitemDel(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "MitemDel");
}

std::string
MitemDel::iam(void) const
{
    return "MitemDel";
}

/* Class ReadNode */

ReadNode::ReadNode(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "ReadNode");
}

std::string
ReadNode::iam(void) const
{
    return "ReadNode";
}

/* Class WriteNode */

WriteNode::WriteNode(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "WriteNode");
}

std::string
WriteNode::iam(void) const
{
    return "WriteNode";
}

/* Class Browse */

Browse::Browse(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "Browse");
}

std::string
Browse::iam(void) const
{
    return "Browse";
}

/* Class Print */

Print::Print(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "Print");
}

std::string
Print::iam(void) const
{
    return "Print";
}

/* Class Replace */

Replace::Replace(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "Replace");
}

std::string
Replace::iam(void) const
{
    return "Replace";
}

/* Eof */