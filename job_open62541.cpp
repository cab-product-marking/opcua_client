#include "job_open62541.h"

using namespace open62541;

/* Class Job */

Job::Job(const std::string& init, std::string type) noexcept 
{
    info_.emplace(PRAEFIX_INIT, init);
    info_.emplace(PRAEFIX_TYPE, type);
}

Job::Job(opcuac::jobsptr old) noexcept 
{
    auto job = std::dynamic_pointer_cast<open62541::Job>(old);
    this->info_ = job->info_;
    this->data_ = job->data_;
    this->active_ = job->active_;
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
    value = this->info_[key];
    return;
}

std::string
Job::get_info(std::string key)
{
    return info_[key];
}

void
Job::add_info(std::string type, std::string des)
{
    this->info_.emplace(type, des);
    return;
}

opcuac::datasptr
Job::get_data(std::string key)
{
    return data_[key];
}

void
Job::add_data(std::string type, opcuac::datasptr data)
{
    /* Override existing element or insert new one */
    this->data_.insert_or_assign(type, data);
    return;
}

void
Job::erase_data(std::string type)
{
    this->data_.erase(type);
    return;
}

void
Job::status(int status)
{
    this->active_ = status;
    return;
}

int
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