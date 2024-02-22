#pragma once

#include "job_open62541.h"

namespace dec
{
    /* Decorator for job type  */

    class JobType : public open62541::Job
    {
    public:

        JobType() = delete;

        JobType(open62541::jsptr) noexcept;

        virtual
        ~JobType() = default;

        virtual void
        print(std::ostream& os) const override;

    protected:
    
        open62541::jsptr j_;

    };

    /* Useful types */

    class MitemAdd : public JobType
    {
        using JobType::JobType;
    };

    class MitemDel : public JobType
    {
        using JobType::JobType;
    };

    class ReadNode : public JobType
    {
        using JobType::JobType;
    };

    class WriteNode : public JobType
    {
        using JobType::JobType;
    };

    class Browse : public JobType
    {
        using JobType::JobType;
    };

    class Print : public JobType
    {
        using JobType::JobType;
    };

    class Replace : public JobType
    {
        using JobType::JobType;
    };

} // namespace dec

/* Eof */