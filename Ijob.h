#pragma once

namespace opcuac
{
    /**
     * Class IJob - Job interface
     * ^^^^^^^^^^^^^^^^^^^^^^^^^^
     */
    class IJob
    {
    public:

        virtual void
        print(std::ostream& os) const = 0;
        
        /* More pure virtual functions  */    

    };

    class Job : public IJob
    {
    public:

        Job() = default;

        virtual 
        ~Job() = default;

    };

} // namespace opcuac

/* Eof */