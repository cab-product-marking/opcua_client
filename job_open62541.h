#pragma once

#include <iostream>
#include <string>

#include "Ijob.h"

#include <open62541/client.h>

namespace open62541
{

    /* Class Job */

    class Job : public opcuac::Job
    {
    public:

        virtual 
        ~Job() = default;

        virtual UA_NodeId&
        nodeID(void) = 0;

    protected:

        Job() = default;

    };

    /* Class JobNumeric */

    class JobNumeric : public Job
    {
    public:

        JobNumeric(int id, int ns) noexcept;
        
        virtual
        ~JobNumeric() override;

        UA_NodeId&
        nodeID(void) override;

    private:

        int id_;
        int ns_;
        UA_NodeId target_;

    };

    /* Class JobString */
    
    class JobString : public Job
    {
    public:

        JobString(std::string str, int ns) noexcept;

        virtual
        ~JobString() override;

        UA_NodeId&
        nodeID(void) override;

    private:

        std::string str_;
        int ns_;
        UA_NodeId target_;

    };

};

/* Eof */