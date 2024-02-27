#pragma once

#include "job_open62541.h"

#include <open62541/client.h>

#define PRAEFIX_ID          "Id"
#define PRAEFIX_NAMESPACE   "Namespace"

namespace dec
{
    /* Decorator for node type */
    
    class NodeType : public open62541::Job
    {
    public:

        explicit NodeType(open62541::jsptr) noexcept;

        virtual UA_NodeId&
        nodeID(void) = 0;

        virtual
        ~NodeType() = default;

    };

    /* Useful types */

    class JNumeric : public NodeType
    {
    public:

        JNumeric() = delete;

        JNumeric(open62541::jsptr, int, int) noexcept;

        explicit JNumeric(open62541::jsptr) noexcept;

        virtual
        ~JNumeric() override;

        UA_NodeId&
        nodeID(void);

    protected:

        int id_;
        int ns_;
        UA_NodeId target_;

    };
    
    class JString : public NodeType
    {
    public:

        JString() = delete;

        JString(open62541::jsptr, std::string, int) noexcept;

        explicit JString(open62541::jsptr) noexcept;

        virtual
        ~JString() override;

        UA_NodeId&
        nodeID(void);

    protected:

        std::string id_;
        int ns_;
        UA_NodeId target_;

    };

} // namespace dec 

/* Eof */