#include "job_open62541.h"

using namespace open62541;

/* Class Job */

Job::Job(const std::string& str) noexcept 
{
    info_.emplace(PRAEFIX_INIT, str);
}

Job::Job(open62541::jsptr other) noexcept
{
    this->info_ = other->info_;
}

void
Job::print(std::ostream& os) const
{
    for(auto &it : this->info_)
    {
        os << std::setw(20) << std::left << it.first << ": " << it.second << "\n";
    }
    return;
}

/* Output stream operator definition */

std::ostream&
open62541::operator<<(std::ostream& os, const open62541::Job& job)
{
    job.print(os);
    return os;
}

/* Eof */