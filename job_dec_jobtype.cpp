#include "job_dec_jobtype.h"

using namespace dec;

JobType::JobType(open62541::jsptr j) noexcept : j_(j) 
{
    
}

void
JobType::print(std::ostream& os) const 
{
    os << "Initial: " << this->initial_str_;
    return;
}

/* Eof */