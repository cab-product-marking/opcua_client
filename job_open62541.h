#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <map>

#include "Ijob.h"

#define PRAEFIX_INIT    "Initial"

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

        explicit Job(open62541::jsptr) noexcept;

        virtual void
        print(std::ostream& os) const override;

        friend std::ostream&
        operator<<(std::ostream& os, const Job& job);

    protected: 

        std::map<std::string, std::string> info_;

    };

} // namespace open62541

/* Eof */