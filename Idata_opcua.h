#pragma once

#include <iostream>
#include <memory>

namespace opcuac
{
    /* Fd */
    class IData;
    
    typedef std::shared_ptr<opcuac::IData> datasptr;

    /**
     * Class IData - Data interface 
     * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    */
    class IData
    {
    public:

        virtual 
        ~IData() = default;

        /**
         * Basic 
         * ^^^^^
         * 
         * The methods declared here implement the data handling of implemented data types.
        */

        /* Writes the data to terminal - used with operator<< */
        virtual void
        print(std::ostream& os) const = 0;

        /* Return data type */
        virtual std::string
        type(void) const = 0;
        
    };

} // namespace opcuac

/* Eof */