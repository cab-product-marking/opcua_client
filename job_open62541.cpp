#include "job_open62541.h"

using namespace open62541;

Job::Job(const std::string& str) noexcept : initial_str_(str)
{

}

void
Job::print(std::ostream& os) const
{
    return;
}

std::ostream&
open62541::operator<<(std::ostream& os, const open62541::Job& job)
{
    job.print(os);
    return os;
}

/* Eof */