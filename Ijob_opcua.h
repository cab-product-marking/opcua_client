#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <set>

#include "Idata_opcua.h"

namespace opcuac
{
    /* Fd */
    class IJob;

    typedef std::shared_ptr<opcuac::IJob> jobsptr;

    /**
     * Class IJob - Job interface
     * ^^^^^^^^^^^^^^^^^^^^^^^^^^
    */
    class IJob
    {
    public:

        virtual 
        ~IJob() = default;

        /**
         * Basic 
         * ^^^^^
         * 
         * The methods declared here control the output, the status and the information 
         * handling of the job. Further functionality can be added at runtime via a decorator.
        */

        /* Writes the content of the info_ map to terminal - used with operator<< */
        virtual void
        print(std::ostream& os) const = 0;
        
        /* Returns the content of the info_ map as a reference using the praefix */
        virtual void
        get_info(std::string, std::string&) = 0;

        /* Returns the content of the info_ map via return using the praefix */
        virtual std::string
        get_info(std::string) = 0;   

        /* Sets contents in the info_ map using the praefix */
        virtual void
        add_info(std::string, std::string) = 0;

        /* Returns data of the data_ map via return using the praefix */
        virtual opcuac::datasptr
        get_data(std::string) = 0;

        /* Insert data in the data_ map using the praefix */
        virtual void
        add_data(std::string, opcuac::datasptr) = 0;

        /* Erase data from the data_ map with name */
        virtual void
        erase_data(std::string) = 0;

        /* Sets the job activity status to false */
        virtual void 
        status(int) = 0;

        /* Returns the job activity status */
        virtual int
        status(void) const = 0;

    protected: 

        /* Holds useful basic information about the job and data */
        std::map<std::string, std::string> info_;

        /* Contains the used data objects */
        std::map<std::string, opcuac::datasptr> data_;

        /* Holds the information about the current activity status of the job */
        int active_ = 2;

    };

} // namespace opcuac

/* Eof */