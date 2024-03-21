#include "client_open62541.h"

using namespace open62541;

UA_SessionState Client::actual_session_state_;

/* Basics */

void
Client::init(void)
{
    /* Create new client and config */
    Client::opcuac_ = UA_Client_new();
    Client::opcuac_c_ = UA_Client_getConfig(opcuac_);
    UA_ClientConfig_setDefault(opcuac_c_);
    /* Settings */
    opcuac_c_->timeout = TIMEOUT;
    opcuac_c_->stateCallback = callback_state;
    return;
}

int
Client::connect(const open62541::server_arguments& arg)
{
    UA_StatusCode state = UA_STATUSCODE_BADINTERNALERROR;
    int looper = 1;

    /* Connect client */
    while(true)
    {
        /* Try multiple times */
        if(arg.server_secure_mode)
        {
            state = UA_Client_connectUsername(opcuac_,
                                              arg.server_url.c_str(),
                                              arg.server_username.c_str(),
                                              arg.server_password.c_str());
        }
        else
        {
            state = UA_Client_connect(opcuac_, 
                                      arg.server_url.c_str());
        }

        /* Linux/ Windows needs time to response */
        UA_sleep_ms(CONNECTION_WAITING_TIME);

        if((state != UA_STATUSCODE_GOOD) && (looper != CONNECTION_ATTEMPTS))
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                         "Not connected. Retrying to connect in %u seconds. "
                         "Statuscode: 0x%X - %s",
                         (CONNECTION_RETRY_TIME / 1000) * looper, state,
                         UA_StatusCode_name(state));
            /* More time starting printer */
            UA_sleep_ms(CONNECTION_RETRY_TIME * looper);
        }
        else if((state != UA_STATUSCODE_GOOD) && (looper == CONNECTION_ATTEMPTS))
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Not possible to connect server - make sure the device is" 
                        "connected and switched on. Statuscode: 0x%X - %s", 
                        state, UA_StatusCode_name(state));
            /* Clean up */
            UA_Client_delete(opcuac_);
            break;
        }
        else
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Successfully connected!");
            break;
        }
        looper++;
    }   
    return state;
}

void
Client::disconnect(void)
{
    UA_StatusCode state = UA_STATUSCODE_BADINTERNALERROR;
    /* Delete all subscriptions incl. monitored items */
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Controlled deletion of subscriptions and "\
                "monitored items before the client disconnect and shutdown.");

    if(!items_.empty())
    {
        for(auto item : items_)
        {
            UA_Client_MonitoredItems_deleteSingle(opcuac_,
                                                  item.second.subscription_id,
                                                  item.second.monitored_item_id);
            
            UA_Client_Subscriptions_deleteSingle(opcuac_,
                                                 item.second.subscription_id);
        }
    }

    /* Disconnect */
    state = UA_Client_disconnect(opcuac_);

    if(state != UA_STATUSCODE_GOOD)
    { 
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Client disconnect failed. Statuscode: 0x%X - %s",
                    state, UA_StatusCode_name(state));
    }
    /* Clean up */
    UA_Client_delete(opcuac_);
    return;
}

int
Client::run_iterate(void)
{
    UA_StatusCode state = UA_STATUSCODE_BADINTERNALERROR;
    if(actual_session_state_ != UA_SESSIONSTATE_ACTIVATED)
    {
        return state;
    }

    state = UA_Client_run_iterate(this->opcuac_, ITERATIE_TIME);

    if(state != UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Call run iterate failed. Statuscode: 0x%X - %s",
                    state, UA_StatusCode_name(state));
    }
    return state;
}

/* Printer services */

void
Client::add_monitored_item(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    if(actual_session_state_ != UA_SESSIONSTATE_ACTIVATED)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Session is not ready for monitored items.");
    }
    else
    {
        /* Subscription needed */
        UA_CreateSubscriptionRequest sub_request;
        UA_CreateSubscriptionRequest_init(&sub_request);
        sub_request = UA_CreateSubscriptionRequest_default();

        UA_CreateSubscriptionResponse sub_response;
        UA_CreateSubscriptionResponse_init(&sub_response);
        sub_response = UA_Client_Subscriptions_create(opcuac_,
                                                      sub_request,
                                                      nullptr,
                                                      nullptr,
                                                      callback_subscription_delete);

        if(sub_response.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                         "Error, while create subscription. Statuscode: 0x%X - %s",
                         sub_response.responseHeader.serviceResult,
                         UA_StatusCode_name(sub_response.responseHeader.serviceResult));

            /* Clean up */
            UA_CreateSubscriptionResponse_clear(&sub_response);
            UA_CreateSubscriptionRequest_clear(&sub_request);
            return;
        }
        else
        {
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                        "SUB ID [%u] create successfully.", 
                        sub_response.subscriptionId);

            /* Node id */
            UA_NodeId target;
            UA_NodeId_init(&target);
            UA_NodeId_copy(&local->nodeID(), &target);

            /* Monitored item */
            UA_MonitoredItemCreateRequest mon_request;
            UA_MonitoredItemCreateRequest_init(&mon_request);
            mon_request = UA_MonitoredItemCreateRequest_default(target);

            /* Get the data element */
            auto data = local->get_data(DATA_READ);
            void* context = static_cast<void*>(data.get());

            UA_MonitoredItemCreateResult mon_result;
            UA_MonitoredItemCreateResult_init(&mon_result);
            mon_result = 
                    UA_Client_MonitoredItems_createDataChange(Client::opcuac_,
                                                              sub_response.subscriptionId,
                                                              UA_TIMESTAMPSTORETURN_NEITHER,
                                                              mon_request,
                                                              context,
                                                              handler_monitored_item,
                                                              callback_monitored_item_delete);


            if(mon_result.statusCode != UA_STATUSCODE_GOOD)
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                             "Error, while create monitored item. Statuscode: 0x%X - %s",
                             mon_result.statusCode, UA_StatusCode_name(mon_result.statusCode));

                /* Clean up */
                UA_MonitoredItemCreateResult_clear(&mon_result);
                UA_MonitoredItemCreateRequest_clear(&mon_request);

                /* Delete subscription */
                UA_StatusCode state = UA_Client_Subscriptions_deleteSingle(Client::opcuac_,
                                                                 sub_response.subscriptionId);
                
                if(state != UA_STATUSCODE_GOOD)
                {
                    UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                 "SUB ID [%u] delete failed. Statuscode: 0x%X - %s",
                                 sub_response.subscriptionId, state,
                                 UA_StatusCode_name(state));
                }
                else
                {
                    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                "SUB ID [%u] delete successfully.",
                                sub_response.subscriptionId);
                }
                UA_CreateSubscriptionResponse_clear(&sub_response);
                UA_CreateSubscriptionRequest_clear(&sub_request);
                return;
            }
            else
            {
                /* Workroutine done */
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "SUB ID [%u] | MON ID [%u] create successfully.",
                            sub_response.subscriptionId, mon_result.monitoredItemId);

                /* Save subscription id with node id */
                mitem_t item = {(int)mon_result.monitoredItemId, (int)sub_response.subscriptionId};
                items_.emplace(local, item);

                /* Clean up */
                UA_MonitoredItemCreateResult_clear(&mon_result);
                UA_MonitoredItemCreateRequest_clear(&mon_request);
                UA_CreateSubscriptionResponse_clear(&sub_response);
                UA_CreateSubscriptionRequest_clear(&sub_request);
            }
        }
    }
    return;
}

void
Client::del_monitored_item(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    if(items_.empty())
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Error, delete monitored item. No items are monitored.");
        return;
    }
    else
    {
        int notifier = 0;
        /* Iterate through the list of monitored items */
        for(auto item : items_)
        {
            auto mitem = std::dynamic_pointer_cast<open62541::JobDecNode>(item.first);

            if(UA_NodeId_equal(&mitem->nodeID(), &local->nodeID()));
            {
                /* Delete monitored item and subscription process */
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Found the monitored item.");

                UA_StatusCode state = UA_Client_MonitoredItems_deleteSingle(opcuac_,
                        item.second.subscription_id,
                        item.second.monitored_item_id);

                notifier = 1;

                if(state != UA_STATUSCODE_GOOD)
                {
                    UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                 "SUB ID [%u] | MON ID [%u] delete failed. Statuscode: 0x%X - %s",
                                 item.second.subscription_id, item.second.monitored_item_id,
                                 state, UA_StatusCode_name(state));
                }
                else
                {
                    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                "SUB ID [%u] | MON ID [%u] delete successfully.",
                                item.second.subscription_id, item.second.monitored_item_id);

                    /* Delete subscription */
                    state = UA_Client_Subscriptions_deleteSingle(opcuac_,
                                                                 item.second.subscription_id);

                    if(state != UA_STATUSCODE_GOOD)
                    {
                        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                    "SUB ID [%u] delete failed. Statuscode: 0x%X - %s",
                                    item.second.subscription_id, state, 
                                    UA_StatusCode_name(state));
                    }
                    else
                    {
                        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                    "SUB ID [%u] delete successfully.",
                                    item.second.subscription_id);
                    }
                }
                mitem->status(STATUS_DEAD);
                break;
            }     
        }
        if(notifier == 0)
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "The monitored item to be deleted was not found.");
        }
    }
    return;
}

void
Client::read_node(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_StatusCode state = UA_Client_readValueAttribute(opcuac_,
                                                       local->nodeID(),
                                                       &variant);

    if(state == UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Read node successfully.");
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, sending read node request. Statuscode: Statuscode: 0x%X - %s",
                     state, UA_StatusCode_name(state));

        UA_Variant_clear(&variant);
        return;
    }

    data_handler_read(variant, job);                                               

    /* Clean up */
    UA_Variant_clear(&variant);
    return;
}

void
Client::write_node(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    UA_Variant variant;
    UA_Variant_init(&variant);

    data_handler_write(variant, job);

    UA_StatusCode state = UA_Client_writeValueAttribute(opcuac_,
                                                        local->nodeID(),
                                                        &variant);

    if(state == UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Write node successfully.");
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, sending write node request. Statuscode: Statuscode: 0x%X - %s",
                     state, UA_StatusCode_name(state));
    }

    /* Clean up */
    UA_Variant_clear(&variant);
    return;
}

void
Client::browse(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    /* Create tree and print tree */
    std::shared_ptr<NodeTree> tree = std::make_shared<NodeTree>(this->opcuac_);
    tree->print(local->nodeID());
    tree->printfile(local->nodeID(), "browse.txt");
    return;
}

/* Printer methods */

void 
Client::file_upload(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    auto data = local->get_data(DATA_PRINT);
    auto value = std::dynamic_pointer_cast<open62541::DString>(data);
    
    /* Node id object MethodSet node */
    UA_NodeId object_id;
    UA_NodeId_init(&object_id);
    /* Object MethodSet */
    object_id = UA_NODEID_NUMERIC(3, 10012);

    /* 1. Storage device */
    // UA_UInt32 storage_device = 0; /* Default storage */
    UA_Int32 storage_device = 1; /* IFFS */

    /* Convert */
    std::string fname = value->get();
    size_t pos = fname.find_last_of('/');
    std::string identifier;
    if(pos != std::string::npos)
    {
        identifier = fname.substr(pos + 1);
    }
    else
    {
        identifier = fname;
    }
    /* 2. Filename */
    UA_String filename;
    UA_String_init(&filename);
    filename = UA_String_fromChars(identifier.c_str());
    
    /* 3. File contents */
    UA_ByteString byte_string;
    UA_ByteString_init(&byte_string);
    std::ifstream file(value->get(), std::ios::binary | std::ios::ate);
    if(!file)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, %s is not available.",
                     value->get().c_str());
        UA_ByteString_clear(&byte_string);
        UA_String_clear(&filename);
        UA_NodeId_clear(&object_id);
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    /* Read in string */
    std::string buffer(size, '\0');
    if(file.read(&buffer[0], size))
    {
        to_uabytestring(buffer, &byte_string);
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, convert %s to string failed.",
                     value->get().c_str());
        UA_ByteString_clear(&byte_string);
        UA_String_clear(&filename);
        UA_NodeId_clear(&object_id);
        return;
    }
    
    /* Array with language type at first position and data at second*/
    size_t array_size = {3};
    UA_Variant *array = (UA_Variant*)UA_Array_new(array_size, &UA_TYPES[UA_TYPES_VARIANT]);
    if(!array)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, nullptr!");
        UA_Array_delete(array, array_size, &UA_TYPES[UA_TYPES_VARIANT]);
        UA_ByteString_clear(&byte_string);
        UA_String_clear(&filename);
        UA_NodeId_clear(&object_id);
        return;
    }
    for(size_t i = 0; i < array_size; ++i)
    {
        UA_Variant_init(&array[i]);
    }
    UA_Variant_setScalarCopy(&array[0], &storage_device, &UA_TYPES[UA_TYPES_INT32]);
    UA_Variant_setScalarCopy(&array[1], &filename, &UA_TYPES[UA_TYPES_STRING]);
    UA_Variant_setScalarCopy(&array[2], &byte_string , &UA_TYPES[UA_TYPES_BYTESTRING]);

    UA_StatusCode state = UA_Client_call(opcuac_,
                                         object_id,
                                         local->nodeID(),
                                         array_size,
                                         array,
                                         nullptr,
                                         nullptr);

    if(state == UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Call method FileUpload successfully.");
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, call method FileUpload request. Statuscode: 0x%X - %s",
                     state, UA_StatusCode_name(state));
    }
    UA_Array_delete(array, array_size, &UA_TYPES[UA_TYPES_VARIANT]);
    UA_ByteString_clear(&byte_string);
    UA_String_clear(&filename);
    UA_NodeId_clear(&object_id);
    return;
}

void
Client::print_data(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    auto data = local->get_data(DATA_PRINT);
    auto value = std::dynamic_pointer_cast<open62541::DString>(data);

    /* Node id object MethodSet node */
    UA_NodeId object_id;
    UA_NodeId_init(&object_id);
    /* Object MethodSet */
    object_id = UA_NODEID_NUMERIC(3, 10012);

    /* Language */
    UA_Int32 language;
    // UA_Boolean language; /* Works */
    language = local->get_info(PRAEFIX_LANGUAGE) == "js" ? 1 : 0;

    /* Data */
    UA_ByteString byte_string;
    UA_ByteString_init(&byte_string);
    to_uabytestring(value->get(), &byte_string);
    
    /* 1. Array with language type at first position and data at second*/
    size_t array_size = {2};
    UA_Variant *array = (UA_Variant*)UA_Array_new(array_size, &UA_TYPES[UA_TYPES_VARIANT]);
    if(!array)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, nullptr!");
        UA_Array_delete(array, array_size, &UA_TYPES[UA_TYPES_VARIANT]);
        UA_ByteString_clear(&byte_string);
        UA_NodeId_clear(&object_id);
        return;
    }
    for(size_t i = 0; i < array_size; ++i)
    {
        UA_Variant_init(&array[i]);
    }
    UA_Variant_setScalarCopy(&array[0], &language, &UA_TYPES[UA_TYPES_INT32]);
    UA_Variant_setScalarCopy(&array[1], &byte_string, &UA_TYPES[UA_TYPES_BYTESTRING]);

    UA_StatusCode state = UA_Client_call(opcuac_,
                                         object_id,
                                         local->nodeID(),
                                         array_size,
                                         array,
                                         nullptr,
                                         nullptr);

    if(state == UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Call method PrintData successfully.");
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, call method PrintData request. Statuscode: 0x%X - %s",
                     state, UA_StatusCode_name(state));
    }
    UA_Array_delete(array, array_size, &UA_TYPES[UA_TYPES_VARIANT]);
    UA_ByteString_clear(&byte_string);
    UA_NodeId_clear(&object_id);
    return;
}

/* Printer interpreter methods */

void 
Client::print_current_label(opcuac::jobsptr job)
{
    auto local = std::dynamic_pointer_cast<open62541::JobDecNode>(job);

    /* Node id object MethodSet node */
    UA_NodeId object_id;
    UA_NodeId_init(&object_id);
    /* Object MethodSet */
    object_id = UA_NODEID_NUMERIC(3, 10012);

    /* Storage device - default is one */
    UA_UInt32 amount_labels = (UA_UInt32)(1U); 

    UA_Variant variant;
    UA_Variant_init(&variant);
    
    UA_Variant_setScalarCopy(&variant, &amount_labels, &UA_TYPES[UA_TYPES_UINT32]);
    

    UA_StatusCode state = UA_Client_call(opcuac_,
                                         object_id,
                                         local->nodeID(),
                                         1,
                                         &variant,
                                         nullptr,
                                         nullptr);

    if(state == UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Call method PrintCurrentLabel successfully.");
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, call method PrintCurrentLabel request. Statuscode: 0x%X - %s",
                     state, UA_StatusCode_name(state));
    }
    UA_Variant_clear(&variant);
    UA_NodeId_clear(&object_id);
    return;
}

void 
Client::callback_state(UA_Client *opcuac_, 
                       UA_SecureChannelState channel_state, 
                       UA_SessionState session_state, 
                       UA_StatusCode connect_status)
{
    switch(session_state)
    {
        case UA_SESSIONSTATE_CLOSED:
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                        "Session closed.");
            break;
        case UA_SESSIONSTATE_CREATE_REQUESTED:
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Session create requested.");
            break;
        case UA_SESSIONSTATE_CREATED:
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Session created.");
            break;
        case UA_SESSIONSTATE_ACTIVATE_REQUESTED:
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Session activate requested.");
            break;
        case UA_SESSIONSTATE_ACTIVATED:
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                        "Session activated.");
            break;
        case UA_SESSIONSTATE_CLOSING:
            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Session closing.");
            break;
    }
    actual_session_state_ = session_state;
    return;
}

void
Client::callback_subscription_inactivity(UA_Client *opcuac_, 
                        UA_UInt32 subscription_id, 
                        void *subscription_context)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] inactivity callback.",
                subscription_id);
    return;
}

void
Client::callback_subscription_delete(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] delete callback.",
                subscription_id);
    return;
}

void
Client::callback_monitored_item_delete(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context,
                        UA_UInt32 monitored_item_id,
                        void *monitored_item_context)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] | MON ID [%u] delete callback.",
                subscription_id, monitored_item_id);

    return;
}

void
Client::handler_monitored_item(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *scontext,
                        UA_UInt32 monitored_item_id,
                        void *mcontext,
                        UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] | MON ID [%u] data handler.",
                subscription_id, monitored_item_id);

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DATETIME])) 
    {
        /* To date time struct */
        UA_DateTimeStruct dts = UA_DateTime_toStruct(*(UA_DateTime *)value->value.data);

        /* Time */
        opc_time_t dummy;
        dummy.msec = dts.milliSec;
        dummy.sec = dts.sec;
        dummy.min = dts.min;
        dummy.hour = dts.hour + 1; /* UTC+1 */
        dummy.day = dts.day;
        dummy.mon = dts.month;
        dummy.year = dts.year;
        /* ... */
        auto data = static_cast<open62541::DDateTime*>(mcontext);
        data->update(dummy);

        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Second space use monitored data. | opc_time_t - "\
                    "%02d.%02d.%04d - %02d:%02d:%02d.%03d",
                    dummy.day, dummy.mon, dummy.year,
                    dummy.hour, dummy.min, dummy.sec, dummy.msec);
        return;
    }

    UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                   "Not all types are implemented as monitored item.");
    return;
}

void
Client::data_handler_read(UA_Variant &target, opcuac::jobsptr job)
{
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_DATETIME])) 
    {
        /* To date time struct */
        UA_DateTimeStruct dts = UA_DateTime_toStruct(*(UA_DateTime *)target.data);

        /* Time */
        opc_time_t dummy;
        dummy.msec = dts.milliSec;
        dummy.sec = dts.sec;
        dummy.min = dts.min;
        dummy.hour = dts.hour + 1; /* UTC+1 */
        dummy.day = dts.day;
        dummy.mon = dts.month;
        dummy.year = dts.year;
        /* ... */
        auto data = std::make_shared<open62541::DDateTime>(dummy);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_BOOLEAN]))
    {
        /* To boolean */
        auto data = std::make_shared<open62541::DBoolean>(*(UA_Boolean *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_INT16]))
    {
        /* To int16 */
        auto data = std::make_shared<open62541::DInt16>(*(UA_Int16 *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_UINT16]))
    {
        /* To uint16 */
        auto data = std::make_shared<open62541::DUInt16>(*(UA_UInt16 *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_INT32]))
    {
        /* To int32 */
        auto data = std::make_shared<open62541::DInt32>(*(UA_Int32 *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_UINT32]))
    {
        /* To uint32*/
        auto data = std::make_shared<open62541::DUInt32>(*(UA_UInt32 *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_INT64]))
    {
        /* To int64 */
        auto data = std::make_shared<open62541::DInt64>(*(UA_Int64 *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_UINT64]))
    {
        /* To uint64 */
        auto data = std::make_shared<open62541::DUInt64>(*(UA_UInt64 *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_FLOAT]))
    {
        /* To float */
        auto data = std::make_shared<open62541::DFloat>(*(UA_Float *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_DOUBLE]))
    {
        /* To double */
        auto data = std::make_shared<open62541::DDouble>(*(UA_Double *)target.data);
        job->add_data(DATA_READ, data);
        return;
    }
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_STRING]))
    {
        /* To string */
        UA_String ua_string = *(UA_String *)target.data;
        auto data = std::make_shared<open62541::DString>(std::string(
                    reinterpret_cast<char*>(ua_string.data), ua_string.length));
        job->add_data(DATA_READ, data);
        return;
    }

    /* Implement more types from types_generated.h */
    UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Error, node datatype in UA_Variant not known.");
    return;
}

void
Client::data_handler_write(UA_Variant &target, opcuac::jobsptr job)
{
    auto data = job->get_data(DATA_WRITE);

    if(data->type() == "opc_time_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DDateTime>(data);
        
        opc_time_t time = local->get();

        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Time write: %d.%d.%d | %d:%d:%d:%d",
                    time.day,
                    time.mon,
                    time.year,
                    time.hour,
                    time.min,
                    time.sec,
                    time.msec);

        UA_DateTimeStruct dts;
        dts.nanoSec = 0;
        dts.microSec = 0;
        dts.milliSec = time.msec;
        dts.sec = time.sec;
        dts.min = time.min;
        dts.hour = time.hour - 1; /* UTC+1 */
        dts.day = time.day;
        dts.month = time.mon;
        dts.year = time.year;

        UA_DateTime date_time;
        UA_DateTime_init(&date_time);

        date_time = UA_DateTime_fromStruct(dts);
        UA_Variant_setScalarCopy(&target, &date_time, &UA_TYPES[UA_TYPES_DATETIME]);

        UA_DateTime_clear(&date_time);
        return;
    }
    if(data->type() == "bool")
    {
        auto local = std::dynamic_pointer_cast<open62541::DBoolean>(data);
        UA_Boolean b_value = local->get();
        UA_Variant_setScalarCopy(&target, &b_value, &UA_TYPES[UA_TYPES_BOOLEAN]);
        return;
    }
    if(data->type() == "int16_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DInt16>(data);
        UA_Int16 s16_value = local->get();
        UA_Variant_setScalarCopy(&target, &s16_value, &UA_TYPES[UA_TYPES_INT16]);
        return;
    }
    if(data->type() == "uint16_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DUInt16>(data);
        UA_UInt16 u16_value = local->get();
        UA_Variant_setScalarCopy(&target, &u16_value, &UA_TYPES[UA_TYPES_UINT16]);
        return;
    }
    if(data->type() == "int32_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DInt32>(data);
        UA_Int32 s32_value = local->get();
        UA_Variant_setScalarCopy(&target, &s32_value, &UA_TYPES[UA_TYPES_INT32]);
        return;
    }
    if(data->type() == "uint32_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DUInt32>(data);
        UA_UInt32 u32_value = local->get();
        UA_Variant_setScalarCopy(&target, &u32_value, &UA_TYPES[UA_TYPES_UINT32]);
        return;
    }
    if(data->type() == "int64_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DInt64>(data);
        UA_Int64 s64_value = local->get();
        UA_Variant_setScalarCopy(&target, &s64_value, &UA_TYPES[UA_TYPES_INT64]);
        return;
    }
    if(data->type() == "uint64_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DUInt64>(data);
        UA_UInt64 u64_value = local->get();
        UA_Variant_setScalarCopy(&target, &u64_value, &UA_TYPES[UA_TYPES_UINT64]);
        return;
    }
    if(data->type() == "f32_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DFloat>(data);
        UA_Float f32_value = local->get();
        UA_Variant_setScalarCopy(&target, &f32_value, &UA_TYPES[UA_TYPES_FLOAT]);
        return;
    }
    if(data->type() == "f64_t")
    {
        auto local = std::dynamic_pointer_cast<open62541::DDouble>(data);
        UA_Double f64_value = local->get();
        UA_Variant_setScalarCopy(&target, &f64_value, &UA_TYPES[UA_TYPES_DOUBLE]);
        return;
    }
    if(data->type() == "string")
    {
        auto local = std::dynamic_pointer_cast<open62541::DString>(data);
        std::string std_string = local->get();
        UA_String ua_string = UA_STRING_ALLOC(std_string.c_str());
        UA_Variant_setScalarCopy(&target, &ua_string, &UA_TYPES[UA_TYPES_STRING]);
        UA_String_clear(&ua_string);
        return;
    }

    /* Implement more types from types_generated.h */
    UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Error, node datatype in UA_Variant not known.");
    return;
}

std::string 
Client::to_string(const UA_String *uaString) 
{
    if (!uaString || uaString->length == 0 || uaString->data == nullptr) 
    {
        /* Returns empty string when UA_String is invalid */
        return std::string();  
    }
    /* New string and copy data inside */
    std::string str;
    str.reserve(uaString->length);  /* Reserve memory */
    std::copy(uaString->data, uaString->data + uaString->length, std::back_inserter(str));
    return str;
}

void
Client::to_uabytestring(const std::string &str, UA_ByteString *uaStr)
{
    if(str.empty())
    {
        return;
    }
    uaStr->length = str.size();
    uaStr->data = reinterpret_cast<UA_Byte*>(UA_malloc(uaStr->length));
    if (!uaStr->data) 
    {
        throw std::bad_alloc();
    }
    std::memcpy(uaStr->data, str.data(), uaStr->length);
    return;
}

/* Eof */