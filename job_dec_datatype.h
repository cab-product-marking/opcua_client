#pragma once

#include "common_types.h"

#include "job_open62541.h"

#define PRAEFIX_DATATYPE     "Datatype"

namespace dec
{
    /* Decorator for data type  */

    class DataType : public open62541::Job
    {
    public:

        DataType() = delete;

        explicit DataType(open62541::jsptr) noexcept;

        virtual
        ~DataType() = default;

    };

    /* Class DataTemplate for all data types  */

    template<typename T>
    class DataTemplate : public DataType
    {
    public:

        DataTemplate() = delete;

        DataTemplate(open62541::jsptr, T, open62541::data_type) noexcept;

        explicit DataTemplate(open62541::jsptr) noexcept;

        virtual
        ~DataTemplate() = default;

        T
        value(void) const;

        open62541::data_type
        type(void) const;

    protected:

        T value_;
        open62541::data_type type_;

    };


    // class DDateTime : public DataType
    // {
    // public:

    //     DDateTime() = delete;

    //     explicit DDateTime(jsptr) noexcept;

    // };

    // class DBoolean : public DataType
    // {
    // public:

    //     DBoolean(jsptr, bool) noexcept;

    //     explicit DBoolean(jsptr) noexcept;

    //     virtual
    //     ~DBoolean() = default;

    //     bool 
    //     get_value(void);

    // protected:

    //     bool value_bool_ = false;

    // };

    // class DInt16 : public DataType
    // {
    // public:

    //     explicit DInt16(jsptr) noexcept;
        
    // };

    // class DUInt16 : public DataType
    // {
    // public:

    //     explicit DUInt16(jsptr) noexcept;
        
    // };

    // class DInt32 : public DataType
    // {
    // public:

    //     explicit DInt32(jsptr) noexcept;
        
    // };

    // class DUInt32 : public DataType
    // {
    // public:

    //     explicit DUInt32(jsptr) noexcept;
        
    // };

    // class DInt64 : public DataType
    // {
    // public:

    //     explicit DInt64(jsptr) noexcept;
        
    // };

    // class DUInt64 : public DataType
    // {
    // public:

    //     explicit DUInt64(jsptr) noexcept;
        
    // };

    // class DF32 : public DataType
    // {
    // public:

    //     explicit DF32(jsptr) noexcept;
        
    // };

    // class DF64 : public DataType
    // {
    // public:

    //     explicit DF64(jsptr) noexcept;
        
    // };

    // class DString : public DataType
    // {
    // public:

    //     explicit DString(jsptr) noexcept;
        
    // };

} // namespace dec

/* Eof */