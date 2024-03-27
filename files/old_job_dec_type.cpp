#include "old_job_dec_type.h"

using namespace dec;

/* Class JMitemAdd */

JMitemAdd::JMitemAdd(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "MitemAdd");
}

std::string
JMitemAdd::iam(void) const 
{
    return "MitemAdd";
}

/* Class JMitemDel */

JMitemDel::JMitemDel(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "MitemDel");
}

std::string
JMitemDel::iam(void) const 
{
    return "MitemDel";
}

/* Class JReadNode */

JReadNode::JReadNode(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "ReadNode");
}

std::string
JReadNode::iam(void) const
{
    return "ReadNode";
}

/* Class JWriteNode */

JWriteNode::JWriteNode(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "WriteNode");
}

std::string
JWriteNode::iam(void) const
{
    return "WriteNode";
}

/* Class JBrowse */

JBrowse::JBrowse(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "Browse");
}

std::string
JBrowse::iam(void) const 
{
    return "Browse";
}

/* Class JPrint */

JPrint::JPrint(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "Print");
}

std::string
JPrint::iam(void) const
{
    return "Print";
}

/* Class JReplace */

JReplace::JReplace(opcuac::jobsptr other) noexcept : dec::JobDecoratorType(other)
{
    this->wrapped_job_->add_info(PRAEFIX_TYPE, "Replace");
}

std::string 
JReplace::iam(void) const
{
    return "Replace";
}

/* Eof */