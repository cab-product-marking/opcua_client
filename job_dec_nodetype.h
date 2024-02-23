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

        virtual void
        print(std::ostream& os) const override;

    };

    /* Useful types */

    class Numeric : public NodeType
    {
    public:

        Numeric() = delete;

        Numeric(open62541::jsptr, int, int) noexcept;

        explicit Numeric(open62541::jsptr) noexcept;

        virtual
        ~Numeric() override;

        UA_NodeId&
        nodeID(void);

    protected:

        int id_;
        int ns_;
        UA_NodeId target_;

    };
    
    class String : public NodeType
    {
    public:

        String() = delete;

        String(open62541::jsptr, std::string, int) noexcept;

        explicit String(open62541::jsptr) noexcept;

        virtual
        ~String() override;

        UA_NodeId&
        nodeID(void);

    protected:

        std::string id_;
        int ns_;
        UA_NodeId target_;

    };

} // namespace dec 

/* Eof */