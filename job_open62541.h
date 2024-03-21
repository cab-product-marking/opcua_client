#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <map>

#include "Ijob_opcua.h"
#include "data_open62541.h"

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

        Job(const std::string&, std::string) noexcept;

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

        opcuac::datasptr
        get_data(std::string) override;

        void
        add_data(std::string, opcuac::datasptr) override;

        void
        erase_data(std::string) override;

        void
        status(int) override;

        int
        status(void) const override;

        friend std::ostream&
        operator<<(std::ostream&, const Job&);

    protected:

        explicit 
        Job(opcuac::jobsptr) noexcept;

    };

} // namespace open62541

/* Eof */