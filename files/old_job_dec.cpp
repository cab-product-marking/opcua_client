#include "old_job_dec.h"

using namespace dec;

JobDecorator::JobDecorator(opcuac::jobsptr job) : wrapped_job_(job)
{

}

void
JobDecorator::print(std::ostream& os) const
{
    this->wrapped_job_->print(os);
    return;
}

void
JobDecorator::get_info(std::string key, std::string& value) 
{
    this->wrapped_job_->get_info(key, value);
    return;
}

std::string
JobDecorator::get_info(std::string key)
{
    return this->wrapped_job_->get_info(key);
}

void
JobDecorator::erase(void)
{
    return this->wrapped_job_->erase();
}

bool 
JobDecorator::status(void) const
{
    return this->wrapped_job_->status();
}

void
JobDecorator::add_info(std::string type, std::string des)
{
    this->wrapped_job_->add_info(type, des);
    return;
}

/* Eof */