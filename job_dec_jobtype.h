#pragma once

#include "job_open62541.h"

#define PRAEFIX_TYPE     "Type"

namespace dec
{
    /* Decorator job types  */

    class JobType : public open62541::Job
    {
    public:

        JobType() = delete;

        explicit JobType(open62541::jsptr) noexcept;

        virtual
        ~JobType() = default;

        virtual std::string
        iam(void) const = 0;

    };

    /* Useful types */

    class MitemAdd : public JobType
    {
    public:

        MitemAdd() = delete;

        explicit MitemAdd(open62541::jsptr) noexcept;

        virtual
        ~MitemAdd() = default;

        virtual std::string
        iam(void) const;

    };

    class MitemDel : public JobType
    {
    public:

        MitemDel() = delete;

        explicit MitemDel(open62541::jsptr) noexcept;

        virtual
        ~MitemDel() = default;

        virtual std::string
        iam(void) const;

    };

    class ReadNode : public JobType
    {
    public:

        ReadNode() = delete;

        explicit ReadNode(open62541::jsptr) noexcept;

        virtual
        ~ReadNode() = default;

        virtual std::string
        iam(void) const;

    };

    class WriteNode : public JobType
    {
    public:

        WriteNode() = delete;

        explicit WriteNode(open62541::jsptr) noexcept;

        virtual
        ~WriteNode() = default;

        virtual std::string
        iam(void) const;

    };

    class Browse : public JobType
    {
    public:

        Browse() = delete;

        explicit Browse(open62541::jsptr) noexcept;

        virtual
        ~Browse() = default;

        virtual std::string
        iam(void) const;

    };

    class Print : public JobType
    {
    public:

        Print() = delete;

        explicit Print(open62541::jsptr) noexcept;

        virtual
        ~Print() = default;

        virtual std::string
        iam(void) const;

    };

    class Replace : public JobType
    {
    public:

        Replace() = delete;

        explicit Replace(open62541::jsptr) noexcept;

        virtual
        ~Replace() = default;

        virtual std::string
        iam(void) const;

    };

} // namespace dec

/* Eof */