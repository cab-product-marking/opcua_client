#pragma once

#include <iostream>

#include "job_open62541.h"

#include <open62541/client.h>

namespace open62541
{
    /* Class JobDecNode */

    class JobDecNode : public open62541::Job
    {
    public:

        JobDecNode() = delete;

        virtual
        ~JobDecNode() = default;

        virtual UA_NodeId&
        nodeID(void) = 0;

    protected:

        JobDecNode(opcuac::jobsptr) noexcept;

        /* Holds the actual node id */
        UA_NodeId target_;

    };

    /* Class JNumeric */

    class JNumeric : public JobDecNode
    {
    public:

        JNumeric() = delete;

        JNumeric(opcuac::jobsptr, const int&, const int&) noexcept;
 
        ~JNumeric() noexcept override;

        UA_NodeId&
        nodeID(void) override;

    };

    /* Class JString */

    class JString : public JobDecNode
    {
    public:

        JString() = delete;

        JString(opcuac::jobsptr, const std::string&, const int&) noexcept;

        ~JString() noexcept override;

        UA_NodeId&
        nodeID(void) override;
         
    };
    
} // namespace open62541

/* Eof */