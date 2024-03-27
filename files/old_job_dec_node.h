#pragma once

#include "old_job_dec.h"

#include <open62541/client.h>

#define PRAEFIX_ID          "Id"
#define PRAEFIX_NAMESPACE   "Namespace"

namespace dec
{
    /* Class JobDecoratorNode */

    class JobDecoratorNode : public dec::JobDecorator
    {
    public:

        JobDecoratorNode() = delete;

        virtual
        ~JobDecoratorNode() = default;

        virtual UA_NodeId&
        nodeID(void) = 0;

    protected:

        using dec::JobDecorator::JobDecorator;

    };

    /* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

    /* Class JNumeric */

    class JNumeric : public JobDecoratorNode
    {
    public:

        JNumeric() = delete;

        JNumeric(opcuac::jobsptr, int, int) noexcept;

        ~JNumeric() noexcept override;

        UA_NodeId&
        nodeID(void) override;

    protected:

        int id_;
        int ns_;
        UA_NodeId target_;

    };

    /* Class JString */
    
    class JString : public JobDecoratorNode
    {
    public:

        JString() = delete;

        JString(opcuac::jobsptr, std::string, int) noexcept;

        ~JString() noexcept override;

        UA_NodeId&
        nodeID(void) override;

    protected:

        std::string id_;
        int ns_;
        UA_NodeId target_;

    };

} // namespace dec 

/* Eof */