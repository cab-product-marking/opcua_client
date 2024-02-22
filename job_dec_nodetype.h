#pragma once

#include "job_open62541.h"

#include <open62541/client.h>

namespace dec
{
    /* Decorator for node type */
    
    class NodeType : public open62541::Job
    {
    public:

        virtual
        ~NodeType() = default;

        virtual UA_NodeId&
        nodeID(void) = 0;

        virtual void
        print(std::ostream& os) const override;

    };

    /* Useful types */

    class JobNumeric : public NodeType
    {
    public:

        JobNumeric() = delete;

        JobNumeric(open62541::jsptr, int, int) noexcept;
        
        virtual
        ~JobNumeric() override;

        UA_NodeId&
        nodeID(void);

    protected:
        open62541::jsptr j_;

    private:

        int id_;
        int ns_;
        UA_NodeId target_;

    };
    
    class JobString : public NodeType
    {
    public:

        JobString() = delete;

        JobString(open62541::jsptr, std::string, int) noexcept;

        virtual
        ~JobString() override;

        UA_NodeId&
        nodeID(void);

    protected:
    
        open62541::jsptr j_;

    private:

        std::string str_;
        int ns_;
        UA_NodeId target_;

    };

} // namespace dec 

/* Eof */