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