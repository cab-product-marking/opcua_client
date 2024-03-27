#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <map>

#include "Ijob_opcua.h"

#define PRAEFIX_INIT        "Initial"

namespace open62541
{
    /* Fd Job */
    class Job;

    std::ostream&
    operator<<(std::ostream& os, const Job& job);

    /* Class Job */

    class Job : public opcuac::IJob
    {
    public:

        Job(const std::string&) noexcept;

        virtual 
        ~Job() = default;

        void
        print(std::ostream& os) const override;

        void
        get_info(std::string, std::string&) override;

        std::string
        get_info(std::string) override;

        void
        add_info(std::string, std::string) override;

        void
        erase(void) override;

        bool
        status(void) const override;

        friend std::ostream&
        operator<<(std::ostream& os, const Job& job);

protected:

        Job() = default;

    };

} // namespace open62541

/* Eof */