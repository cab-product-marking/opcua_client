#pragma once

#include "job_open62541.h"
#include <memory>

namespace dec
{
    /* Class JobDecorator */

    class JobDecorator : public open62541::Job
    {
    public:

        virtual
        ~JobDecorator() = default;

        void 
        print(std::ostream&) const override;

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

    protected:

        JobDecorator(opcuac::jobsptr);

        opcuac::jobsptr wrapped_job_;
        
    };

} // namespace dec

/* Eof */