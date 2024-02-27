#include "job_dec_datatype.h"

using namespace dec;

/* Decorator class DataType */

DataType::DataType(open62541::jsptr other) noexcept : open62541::Job(std::move(other))
{

}

/* Class DataTemplate for all data types */

template<typename T> 
DataTemplate<T>::DataTemplate(open62541::jsptr other, T value, open62541::data_type type) noexcept
        : DataType(std::move(other)), value_(value), type_(type)
{
    this->info_.emplace(PRAEFIX_DATATYPE, open62541::StringType::type(type));
}

template<typename T>
DataTemplate<T>::DataTemplate(open62541::jsptr other) noexcept
        : DataType(std::move(other))
{
    auto local = dynamic_cast<dec::DataTemplate<T>*>(other.get());
    this->value_ = local->value_;
    this->type_ = local->type_;
}

template<typename T>
T 
DataTemplate<T>::value(void) const 
{
    return this->value_;
}

template<typename T>
open62541::data_type
DataTemplate<T>::type(void) const 
{
    return this->type_;
}

// /* Class DateTime */

// DDateTime::DDateTime(jsptr other) noexcept : dec::DataType(std::move(other))
// {

// }

/* Class Boolean */

// DBoolean::DBoolean(jsptr other, bool value) noexcept
//         : dec::DataType(std::move(other)), value_bool_(value)
// {
//     info_.emplace(PRAEFIX_DATATYPE, "boolean");
// }

// DBoolean::DBoolean(jsptr other) noexcept : dec::DataType(std::move(other))
// {
//     auto local = dynamic_cast<DBoolean*>(other.get());
//     /* Workflow */
//     this->value_bool_ = local->value_bool_;
// }

// bool
// DBoolean::get_value(void)
// {
//     return value_bool_;
// }

// /* Class Int16 */

// DInt16::DInt16(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class UInt16 */

// DUInt16::DUInt16(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class Int32 */

// DInt32::DInt32(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class UInt32 */

// DUInt32::DUInt32(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class Int64 */

// DInt64::DInt64(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class UInt64 */

// DUInt64::DUInt64(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class F32 */

// DF32::DF32(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class F64 */

// DF64::DF64(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

// /* Class String */

// DString::DString(jsptr other) noexcept : dec::DataType(std::move(other))
// {
    
// }

/* Eof */