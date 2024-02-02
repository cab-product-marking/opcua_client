#pragma once






/* To do ... */



/**
 * Class job
 * ^^^^^^^^^
*/
class JOB
{
    public:
        /* Job information */
        job_type type;
        id_type type_id;
        std::string id_string;
        int id_numeric;
        int namespace_index;
        std::string init_string;
        bool erase;

        /* Data */
        std::shared_ptr<DATA> intern_data;
        std::shared_ptr<DATA> extern_data;
};






/* Eof */