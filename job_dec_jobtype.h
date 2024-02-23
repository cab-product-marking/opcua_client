#pragma once

#include "job_open62541.h"

#define PRAEFIX_TYPE     "Type"

namespace dec
{
    /* Decorator for job type  */

    class JobType : public open62541::Job
    {
    public:

        JobType() = delete;

        explicit JobType(open62541::jsptr) noexcept;

        virtual
        ~JobType() = default;

        virtual void
        print(std::ostream& os) const override;

    };

    /* Useful types */

    class MitemAdd : public JobType
    {
    public:

        explicit MitemAdd(open62541::jsptr) noexcept;

    };

    class MitemDel : public JobType
    {
    public:

        explicit MitemDel(open62541::jsptr) noexcept;

    };

    class ReadNode : public JobType
    {
    public:

        explicit ReadNode(open62541::jsptr) noexcept;

    };

    class WriteNode : public JobType
    {
    public:

        explicit WriteNode(open62541::jsptr) noexcept;

    };

    class Browse : public JobType
    {
    public:

        explicit Browse(open62541::jsptr) noexcept;

    };

    class Print : public JobType
    {
    public:

        explicit Print(open62541::jsptr) noexcept;

    };

    class Replace : public JobType
    {
    public:

        explicit Replace(open62541::jsptr) noexcept;

    };

} // namespace dec

/* Eof */