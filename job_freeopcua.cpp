#include "job_freeopcua.h"

using namespace freeopcua;

std::ostream&
freeopcua::operator<<(std::ostream& os, const freeopcua::Job& job)
{
    job.print(os);
    return os;
}

/* Eof */