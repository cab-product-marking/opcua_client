#include "job_dec_jobtype.h"

using namespace dec;

/* Decorator class JobType */

JobType::JobType(open62541::jsptr other) noexcept : open62541::Job(std::move(other))
{

}

void
JobType::print(std::ostream& os) const 
{
    for(auto &it : this->info_)
    {
        os << std::setw(20) << std::left << it.first << ": " << it.second << "\n";
    }
    return;
}

/* Class MitemAdd */

MitemAdd::MitemAdd(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "MitemAdd");
}

/* Class MitemDel */

MitemDel::MitemDel(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "MitemDel");
}

/* Class ReadNode */

ReadNode::ReadNode(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "ReadNode");
}

/* Class WriteNode */

WriteNode::WriteNode(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "WriteNode");
}

/* Class Browse */

Browse::Browse(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "Browse");
}

/* Class Print */

Print::Print(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "Print");
}

/* Class Replace */

Replace::Replace(open62541::jsptr other) noexcept : dec::JobType(std::move(other))
{
    this->info_.emplace(PRAEFIX_TYPE, "Replace");
}

/* Eof */