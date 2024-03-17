#include "data_open62541.h"

using namespace open62541;

/* Class DDateTime */

DDateTime::DDateTime(opc_time_t value)
{
    this->value_ = value;
}

void
DDateTime::print(std::ostream& os) const 
{
    os 
    << std::setw(2) << std::setfill('0') << this->value_.day << "."
    << std::setw(2) << std::setfill('0') << this->value_.mon << "."
    << std::setw(4) << std::setfill('0') << this->value_.year << " - "
    << std::setw(2) << std::setfill('0') << this->value_.hour << ":"
    << std::setw(2) << std::setfill('0') << this->value_.min << ":"
    << std::setw(2) << std::setfill('0') << this->value_.sec << "."
    << std::setw(3) << std::setfill('0') << this->value_.msec
    << std::setfill(' ');
    return;
}

std::string
DDateTime::type(void) const
{
    return "opc_time_t";
}

opc_time_t
DDateTime::get(void)
{
    return this->value_;
}

void
DDateTime::update(opc_time_t val)
{
    this->value_ = val;
}

/* Class DBoolean */

DBoolean::DBoolean(bool value)
{
    this->value_ = value;
}

void
DBoolean::print(std::ostream& os) const 
{
    os << this->value_;
    return;
}

std::string
DBoolean::type(void) const
{
    return "bool";
}

bool
DBoolean::get(void)
{
    return this->value_;
}

/* Class DInt16 */

DInt16::DInt16(int16_t value)
{
    this->value_ = value;
}

void
DInt16::print(std::ostream& os) const 
{
    os << this->value_;
    return;
}

std::string
DInt16::type(void) const
{
    return "int16_t";
}

int16_t
DInt16::get(void)
{
    return this->value_;
}

/* Class DUInt16 */

DUInt16::DUInt16(uint16_t value)
{
    this->value_ = value;
}

void
DUInt16::print(std::ostream& os) const 
{
    os << this->value_;
    return;
}

std::string
DUInt16::type(void) const
{
    return "uint16_t";
}

uint16_t
DUInt16::get(void)
{
    return this->value_;
}

/* Class DInt32 */

DInt32::DInt32(int32_t value)
{
    this->value_ = value;
}

void
DInt32::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DInt32::type(void) const
{
    return "int32_t";
}

int32_t
DInt32::get(void)
{
    return this->value_;
}

/* Class DUInt32*/

DUInt32::DUInt32(uint32_t value)
{
    this->value_ = value;
}

void
DUInt32::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DUInt32::type(void) const
{
    return "uint32_t";
}

uint32_t
DUInt32::get(void)
{
    return this->value_;
}

/* Class DInt64 */

DInt64::DInt64(int64_t value)
{
    this->value_ = value;
}

void
DInt64::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DInt64::type(void) const
{
    return "int64_t";
}

int64_t
DInt64::get(void)
{
    return this->value_;
}

/* Class DUInt64 */

DUInt64::DUInt64(uint64_t value)
{
    this->value_ = value;
}

void
DUInt64::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DUInt64::type(void) const
{
    return "uint64_t";
}

uint64_t
DUInt64::get(void)
{
    return this->value_;
}

/* Class DFloat */

DFloat::DFloat(f32_t value)
{
    this->value_ = value;
}

void
DFloat::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DFloat::type(void) const
{
    return "f32_t";
}

f32_t
DFloat::get(void)
{
    return this->value_;
}

/* Class DDouble */

DDouble::DDouble(f64_t value)
{
    this->value_ = value;
}

void
DDouble::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DDouble::type(void) const
{
    return "f64_t";
}

f64_t
DDouble::get(void)
{
    return this->value_;
}

/* Class DString */

DString::DString(std::string value)
{
    this->value_ = value;
}

void
DString::print(std::ostream& os) const
{
    os << this->value_;
    return;
}

std::string 
DString::type(void) const
{
    return "string";
}

std::string
DString::get(void)
{
    return this->value_;
}

/* Output stream operator definition */

std::ostream&
open62541::operator<<(std::ostream& os, const open62541::Data& data)
{
    data.print(os);
    return os;
}

/* Eof */