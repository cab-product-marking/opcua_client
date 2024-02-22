#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "Ijob.h"

namespace freeopcua
{
    class Job;
    typedef std::shared_ptr<freeopcua::Job> jsptr;

    std::ostream&
    operator<<(std::ostream& os, const Job& job);

    /* Class Job */

    class Job : public opcuac::Job
    {
    public:

        Job() = default;

        virtual 
        ~Job() = default;

        virtual void
        print(std::ostream& os) const override {};

        friend std::ostream&
        operator<<(std::ostream& os, const Job& job);

        /* Variables */
        std::string initial_str_ = "(*_*)";

    };

} // namespace freeopcua

/* Eof */