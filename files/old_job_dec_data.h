#pragma once

#include "old_job_dec.h"

#define PRAEFIX_DATATYPE     "Datatype"

namespace dec
{
    /* Class JobDecoratorData */

    template <typename T>
    class JobDecoratorData : public dec::JobDecorator
    {
    public:

        JobDecoratorData() = delete;

        virtual 
        ~JobDecoratorData() = default;

        virtual void
        set(T value) = 0;

        virtual T
        get(void) const = 0;

    protected:

    using dec::JobDecorator::JobDecorator;

        T value_;

    };

    /* ***** ***** ***** ***** ***** ***** ***** ***** ***** ***** */

    /* Class XXX */

} // namespace dec

/* Eof */