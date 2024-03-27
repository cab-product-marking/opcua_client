#pragma once 

#include <iostream>

#include "old_job_dec.h"

#define PRAEFIX_TYPE        "Type"

namespace dec
{
    /* Class JobDecoratorType */

    class JobDecoratorType : public dec::JobDecorator
    {
    public:

        JobDecoratorType() = delete;

        virtual
        ~JobDecoratorType() = default;

        virtual std::string
        iam(void) const = 0;

    protected:

        using dec::JobDecorator::JobDecorator;

    };

    /* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

    /* Class JMitemAdd */

    class JMitemAdd : public JobDecoratorType
    { 
    public:

        JMitemAdd() = delete;

        JMitemAdd(opcuac::jobsptr) noexcept;

        virtual 
        ~JMitemAdd() = default;

        std::string
        iam(void) const override;

    };

    /* Class JMitemDel */

    class JMitemDel : public JobDecoratorType
    {
    public:

        JMitemDel() = delete;

        JMitemDel(opcuac::jobsptr) noexcept;

        virtual
        ~JMitemDel() = default;

        std::string
        iam(void) const override;

    };

    /* Class JReadNode */

    class JReadNode : public JobDecoratorType
    {
    public:

        JReadNode() = delete;

        JReadNode(opcuac::jobsptr) noexcept;

        virtual 
        ~JReadNode() = default;

        std::string
        iam(void) const override;

    };

    /* Class JWriteNode */

    class JWriteNode : public JobDecoratorType
    {
    public:

        JWriteNode() = delete;

        JWriteNode(opcuac::jobsptr) noexcept;

        virtual
        ~JWriteNode() = default;

        std::string
        iam(void) const override;

    };

    /* Class JBrowse */

    class JBrowse : public JobDecoratorType
    {
    public:

        JBrowse() = delete;

        JBrowse(opcuac::jobsptr) noexcept;

        virtual
        ~JBrowse() = default;

        std::string
        iam(void) const override;

    };

    /* Class JPrint */

    class JPrint : public JobDecoratorType
    {
    public:

        JPrint() = delete;

        JPrint(opcuac::jobsptr) noexcept;

        virtual
        ~JPrint() = default;

        std::string
        iam(void) const override;

    };

    /* Class JReplace */

    class JReplace : public JobDecoratorType
    {
    public:

        JReplace() = delete;

        JReplace(opcuac::jobsptr) noexcept;

        virtual 
        ~JReplace() = default;

        std::string
        iam(void) const override;

    };
    
} // namespace dec

/* Eof */