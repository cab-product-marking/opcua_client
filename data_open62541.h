#pragma once

#include <ctime>

#include "Idata_opcua.h"

#include "common_types.h"

#include <open62541/client.h>

namespace open62541
{
    /* Fd */
    class Data;

    std::ostream&
    operator<<(std::ostream& os, const Data& data);

    /* Class Data */
    
    class Data : public opcuac::IData
    {
    public:

        virtual
        ~Data() = default;

        virtual void
        print(std::ostream& os) const = 0;

        virtual std::string
        type(void) const = 0;

        friend std::ostream&
        operator<<(std::ostream& os, const Data& data);

    };

    /* Class DDateTime */

    class DDateTime : public Data
    {
    public:

        DDateTime(opc_time_t);

        virtual 
        ~DDateTime() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        opc_time_t
        get(void);

        void
        update(opc_time_t);

    private:

        opc_time_t value_;

    };

    /* Class DBoolean */

    class DBoolean : public Data
    {
    public:

        DBoolean(bool);

        virtual 
        ~DBoolean() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        bool
        get(void);

    private:

        bool value_;

    };

    /* Class DInt16 */

    class DInt16 : public Data
    {
    public:

        DInt16(int16_t);

        virtual 
        ~DInt16() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        int16_t
        get(void);

    private:

        int16_t value_;

    };

    /* Class DUInt16 */

    class DUInt16 : public Data
    {
    public:

        DUInt16(uint16_t);

        virtual 
        ~DUInt16() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        uint16_t
        get(void);

    private:

        uint16_t value_;

    };

    /* Class DInt32 */

    class DInt32 : public Data
    {
    public:

        DInt32(int32_t);

        virtual 
        ~DInt32() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        int32_t
        get(void);

    private:

        int32_t value_;

    };

    /* Class DUInt32 */

    class DUInt32 : public Data
    {
    public:

        DUInt32(uint32_t);

        virtual 
        ~DUInt32() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        uint32_t
        get(void);

    private:

        uint32_t value_;

    };

    /* Class DInt64 */

    class DInt64 : public Data
    {
    public:

        DInt64(int64_t);

        virtual 
        ~DInt64() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        int64_t
        get(void);

    private:

        int64_t value_;

    };

    /* Class DUInt64 */

    class DUInt64 : public Data
    {
    public:

        DUInt64(uint64_t);

        virtual 
        ~DUInt64() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        uint64_t
        get(void);

    private:

        uint64_t value_;

    };

    /* Class DFloat */

    class DFloat : public Data
    {
    public:

        DFloat(f32_t);

        virtual 
        ~DFloat() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        f32_t
        get(void);

    private:

        f32_t value_;

    };

    /* Class DDouble */

    class DDouble : public Data
    {
    public:

        DDouble(f64_t);

        virtual 
        ~DDouble() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        f64_t
        get(void);

    private:

        f64_t value_;

    };

    /* Class DString */

    class DString : public Data
    {
    public:

        DString(std::string);

        virtual 
        ~DString() = default;

        void
        print(std::ostream& os) const override;

        std::string
        type(void) const override;

        std::string
        get(void);

    private:

        std::string value_;

    };

    /* Feel free implementing more types */

} // namespace open62541

/* Eof */