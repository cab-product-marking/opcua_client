#include "job_open62541.h"

using namespace open62541;

/* Class Job */

Job::Job(const std::string& str) noexcept 
{
    info_.emplace(PRAEFIX_INIT, str);
}

Job::Job(jsptr other) noexcept
{
    auto local = dynamic_cast<open62541::Job*>(other.get());
    this->info_ = local->info_;
    this->active_ = local->active_;
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

void
Job::get_info(std::string key, std::string& value)
{
    value = info_[key];
    return;
}

std::string
Job::get_info(std::string key)
{
    return info_[key];
}

void
Job::erase(void)
{
    this->active_ = false;
    return;
}

bool
Job::status(void) const
{
    return this->active_;
}

/* Output stream operator definition */

std::ostream&
open62541::operator<<(std::ostream& os, const open62541::Job& job)
{
    job.print(os);
    return os;
}

/* Eof */