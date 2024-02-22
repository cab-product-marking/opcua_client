#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "Ijob.h"

namespace open62541
{
    class Job;
    typedef std::shared_ptr<open62541::Job> jsptr;

    std::ostream&
    operator<<(std::ostream& os, const Job& job);

    /* Class Job */

    class Job : public opcuac::Job
    {
    public:

        Job(const std::string&) noexcept;

        virtual 
        ~Job() = default;

        virtual void
        print(std::ostream& os) const override;

        friend std::ostream&
        operator<<(std::ostream& os, const Job& job);

    protected:

        Job() = default;

        // std::string initial_str_ = "(*_*)";
        std::string initial_str_;

    };

} // namespace open62541

/* Eof */