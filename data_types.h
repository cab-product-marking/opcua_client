#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include <iostream>
#include <ctime>
#include <map>
#include <memory>

#define OUTPUT_DIRECTORY    "res/"
#define BUILD_DIRECTORY     "build/"


/* Data Types */
/* ^^^^^^^^^^ */

typedef float f32_t;
typedef double f64_t;

/* CAB_Client */

enum class data_type : int
{
    c_datetime = 0,
    c_boolean,
    c_int16,
    c_uint16,
    c_int32,
    c_uint32,
    c_int64,
    c_uint64,
    c_f32,
    c_f64,
    c_string,
    DEFAULT
};

struct data_time
{
    int tm_msec;    
    int tm_sec;			        /* Seconds.	[0-60] (1 leap second) */
    int tm_min;			        /* Minutes.	[0-59] */
    int tm_hour;		        /* Hours.	[0-23] */
    int tm_mday;		        /* Day.		[1-31] */
    int tm_mon;			        /* Month.	[0-11] */
    int tm_year;		        /* Year	- 1900.  */
    int tm_wday;		        /* Day of week.	[0-6] */
    int tm_yday;		        /* Days in year.[0-365]	*/
    int tm_isdst;		        /* DST.		[-1/0/1]*/

# ifdef	__USE_MISC
    long int tm_gmtoff;		    /* Seconds east of UTC.  */
    const char *tm_zone;		/* Timezone abbreviation.  */
# else
    long int __tm_gmtoff;		/* Seconds east of UTC.  */
    const char *__tm_zone;	    /* Timezone abbreviation.  */
# endif
};

struct DATA
{
    data_type type_data;
    data_time time;
    bool b_value;
    int16_t s16_value;
    uint16_t u16_value;
    int32_t s32_value;
    uint32_t u32_value;
    int64_t s64_value;
    uint64_t u64_value;
    f32_t f32_value;
    f64_t f64_value;
    std::string string_value;
    std::string file_name;
    std::map<std::string, std::string> replace;
};

/* CAB_Client / OPCUA_Client */

struct client_arg
{
    std::string server_ip = {"192.168.200.70"};
    std::string port = {"4840"}; 
    std::string username = {"opcuser"}; 
    std::string password = {"opcpass"};
    bool user_mode_enable = {false};
};

enum class job_type : int
{
    mitem_add = 0,
    mitem_delete = 1,
    node_read = 2,
    node_write = 3,
    browse = 4,
    print = 5,
    replace = 6,
    DEFAULT = 7
};

enum class id_type : int
{
    string = 0,
    numeric,
    DEFAULT
};

struct JOB
{
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

enum class node_type : int
{
    method = 1,
    variable = 2,
    object = 3, 
    DEFAULT = 4
};

struct node
{
    /* Node id information */
    int namespace_index;
    id_type ident_type;
    std::string identifier_string;
    int identifier_int;
    node_type node_art;
    std::string display_name;
    std::string description;
    /* Data to do */

};

struct MITEM
{
    int monitored_item_id;
    int subscription_id;
};

#endif // __DATA_TYPES_H__