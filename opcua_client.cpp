#include "opcua_client.h"

UA_SessionState OPCUA_Client::actual_session_state_;

OPCUA_Client::OPCUA_Client()
{
#ifndef NDEBUG
    std::cout   << FONT_RED << "OPCUA_Client::OPCUA_Client()" 
                << FONT_RESET << std::endl;
#endif // NDEBUG 
}

OPCUA_Client::~OPCUA_Client()
{
#ifndef NDEBUG
    std::cout   << FONT_RED << "OPCUA_Client::~OPCUA_Client()" 
                << FONT_RESET << std::endl;
#endif // NDEBUG        
}

void 
OPCUA_Client::opcuac_init(void)
{
    /* Set new OPC UA client */
    opcuac_ = UA_Client_new();

    /* Client configuration */
    opcuac_c_ = UA_Client_getConfig(opcuac_);
    UA_ClientConfig_setDefault(opcuac_c_);

    /* High timeout because there may be other client an processing may tage long if many 
    * method calls are waiting */
    opcuac_c_->timeout = CLIENT_TIMEOUT;
    opcuac_c_->stateCallback = callback_state;
    return;
}

int
OPCUA_Client::opcuac_connect(const std::string &url, client_arg &arg)
{
    UA_StatusCode state;
    int retrys = {1};

    /* Connect client */
    while(true)
    {
        /* Try three times */
        if(arg.user_mode_enable)
        {
            state = UA_Client_connectUsername(opcuac_,
                                              url.c_str(),
                                              arg.username.c_str(),
                                              arg.password.c_str());
        }
        else
        {
            state = UA_Client_connect(opcuac_, 
                                      url.c_str());
        }

        /* Linux/ Windows needs time to response */
        UA_sleep_ms(CLIENT_CONNECTION_WAITING_TIME);

        if((state != UA_STATUSCODE_GOOD) && (retrys != CLIENT_CONNECTIONS_ATTEMPTS))
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                         "Not connected. Retrying to connect in %u seconds. Statuscode: 0x%X - %s",
                         (CLIENT_CONNECTION_RETRY_TIME / 1000) * retrys, state,
                         UA_StatusCode_name(state));
            /* More time start the printer */
            UA_sleep_ms(CLIENT_CONNECTION_RETRY_TIME * retrys);
        }
        else if((state != UA_STATUSCODE_GOOD) && (retrys == CLIENT_CONNECTIONS_ATTEMPTS))
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
        retrys++;
    }   
    return state;
}

void 
OPCUA_Client::opcuac_disconnect()
{
    /* Delete all subscriptions incl. monitored items */
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Controlled deletion of subscriptions and "
                "monitored items before the client disconnects.");

    /* Iterate through the list of monitored items */
    if(!monitored_items_.empty())
    {
        for(auto item : monitored_items_)
        {
            /* Delete monitored item */
            UA_Client_MonitoredItems_deleteSingle(opcuac_,
                                                  item.second.subscription_id,
                                                  item.second.monitored_item_id);

            /* Delete subscription */
            UA_Client_Subscriptions_deleteSingle(opcuac_,
                                                 item.second.subscription_id);
        }
    }

    /* Disconnect */
    UA_StatusCode state = UA_Client_disconnect(opcuac_);

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
OPCUA_Client::opcuac_run_iterate()
{
    UA_StatusCode state = {EXIT_FAILURE};
    if(actual_session_state_ != UA_SESSIONSTATE_ACTIVATED)
    {
        return state;
    }

    state = UA_Client_run_iterate(this->opcuac_, CLIENT_ITERATE_TIME);

    if(state != UA_STATUSCODE_GOOD)
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Call run iterate failed. Statuscode: 0x%X - %s",
                    state, UA_StatusCode_name(state));
    }
    return state;
}

void
OPCUA_Client::opcuac_add_monitored_item(std::shared_ptr<JOB> &job)
{
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
            if(job->type_id == id_type::string)
            {
                target = UA_NODEID_STRING_ALLOC(job->namespace_index, 
                                                job->id_string.c_str());
            }
            else if(job->type_id == id_type::numeric)
            {
                target = UA_NODEID_NUMERIC(job->namespace_index, 
                                           job->id_numeric);
            }
            else
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                             "Error, while create node id.");

                /* Clean up */
                UA_NodeId_clear(&target);

                /* Delete subscription */
                UA_StatusCode state = UA_Client_Subscriptions_deleteSingle(OPCUA_Client::opcuac_,
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

            /* Monitored item */
            UA_MonitoredItemCreateRequest mon_request;
            UA_MonitoredItemCreateRequest_init(&mon_request);
            mon_request = UA_MonitoredItemCreateRequest_default(target);

            /* Create a copy of the shared_ptr on the heap to ensure that it is not deleted */
            void* context = new std::shared_ptr<JOB>(job);

            UA_MonitoredItemCreateResult mon_result;
            UA_MonitoredItemCreateResult_init(&mon_result);
            mon_result = 
                    UA_Client_MonitoredItems_createDataChange(OPCUA_Client::opcuac_,
                                                              sub_response.subscriptionId,
                                                              UA_TIMESTAMPSTORETURN_NEITHER,
                                                              mon_request,
                                                              context,
                                                              handler_monitored_item,
                                                              callback_monitored_item_delete);

            // typedef enum {
            //     UA_TIMESTAMPSTORETURN_SOURCE = 0,
            //     UA_TIMESTAMPSTORETURN_SERVER = 1,
            //     UA_TIMESTAMPSTORETURN_BOTH = 2,
            //     UA_TIMESTAMPSTORETURN_NEITHER = 3,
            //     UA_TIMESTAMPSTORETURN_INVALID = 4,
            //     __UA_TIMESTAMPSTORETURN_FORCE32BIT = 0x7fffffff
            // } UA_TimestampsToReturn;

            // UA_STATUSCODE_BADTIMESTAMPSTORETURNINVALID 0x802B0000
            // UA_STATUSCODE_BADNODEIDUNKNOWN 0x80340000
            if(mon_result.statusCode != UA_STATUSCODE_GOOD)
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, 
                             "Error, while create monitored item. Statuscode: 0x%X - %s",
                             mon_result.statusCode, UA_StatusCode_name(mon_result.statusCode));

                /* Clean up */
                UA_MonitoredItemCreateResult_clear(&mon_result);
                UA_MonitoredItemCreateRequest_clear(&mon_request);
                // UA_NodeId_clear(&target);

                /* Delete subscription */
                UA_StatusCode state = UA_Client_Subscriptions_deleteSingle(OPCUA_Client::opcuac_,
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

                MITEM ids = {(int)mon_result.monitoredItemId, 
                             (int)sub_response.subscriptionId};

                /* Save subscription id with node id */
                monitored_items_.emplace(job, ids);

                /* Clean up */
                UA_MonitoredItemCreateResult_clear(&mon_result);
                UA_MonitoredItemCreateRequest_clear(&mon_request);
                // UA_NodeId_clear(&target);
                UA_CreateSubscriptionResponse_clear(&sub_response);
                UA_CreateSubscriptionRequest_clear(&sub_request);
            }
        }
    }
    return;
}

void
OPCUA_Client::opcuac_delete_monitored_item(std::shared_ptr<JOB> &job)
{
    if(monitored_items_.empty())
    {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Error, delete monitored item. No items are monitored.");
        return;
    }
    else
    {
        /* First node id */
        UA_NodeId target;
        UA_NodeId_init(&target);
        if(job->type_id == id_type::string)
        {
            target = UA_NODEID_STRING_ALLOC(job->namespace_index, 
                                            job->id_string.c_str());
        }
        else if(job->type_id == id_type::numeric)
        {
            target = UA_NODEID_NUMERIC(job->namespace_index, 
                                        job->id_numeric);
        }
        else
        {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                         "Error, while create node id target.");

            /* Clean up */
            UA_NodeId_clear(&target);
            return;
        }

        /* Iterate through the list of monitored items */
        /* Second node id */
        UA_NodeId bullet;
        for(auto item : monitored_items_)
        {
            UA_NodeId_init(&bullet);
            if(item.first->type_id == id_type::string)
            {
                bullet = UA_NODEID_STRING_ALLOC(item.first->namespace_index, 
                                                item.first->id_string.c_str());
            }
            else if(item.first->type_id == id_type::numeric)
            {
                bullet = UA_NODEID_NUMERIC(item.first->namespace_index, 
                                           item.first->id_numeric);
            }
            else
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                             "Error, while create node id bullet.");

                /* Clean up */
                UA_NodeId_clear(&bullet);
                break;
            }

            /* Check node id's */
            if(UA_NodeId_equal(&target, &bullet))
            {
                /* Clean up */
                UA_NodeId_clear(&bullet);

                /* Delete monitored item and subscription process */
                UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            "Found the monitored item.");

                UA_StatusCode state = UA_Client_MonitoredItems_deleteSingle(opcuac_,
                                                                  item.second.subscription_id,
                                                                  item.second.monitored_item_id);

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
                break;
            }     
        }
        UA_NodeId_clear(&target);
    }
    return;
}

void 
OPCUA_Client::opcuac_read_node(std::shared_ptr<JOB> &job)
{
    UA_NodeId target;
    UA_NodeId_init(&target);
    if(job->type_id == id_type::string)
    {
        target = UA_NODEID_STRING_ALLOC(job->namespace_index, 
                                        job->id_string.c_str());
    }
    else if(job->type_id == id_type::numeric)
    {
        target = UA_NODEID_NUMERIC(job->namespace_index, 
                                   job->id_numeric);
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, while generating node id.");
        UA_NodeId_clear(&target);
        return;
    }

    UA_Variant variant;
    UA_Variant_init(&variant);

    UA_StatusCode state = UA_Client_readValueAttribute(opcuac_,
                                                       target,
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
        UA_NodeId_clear(&target);
        UA_Variant_clear(&variant);
        return;
    }

    data_handler_read(variant, job);                                               

    /* Clean up */
    UA_NodeId_clear(&target);
    UA_Variant_clear(&variant);
    return;
}

int
OPCUA_Client::opcuac_write_node(std::shared_ptr<JOB> &job)
{
    UA_NodeId target;
    UA_NodeId_init(&target);
    if(job->type_id == id_type::string)
    {
        target = UA_NODEID_STRING_ALLOC(job->namespace_index, 
                                        job->id_string.c_str());
    }
    else if(job->type_id == id_type::numeric)
    {
        target = UA_NODEID_NUMERIC(job->namespace_index, 
                                   job->id_numeric);
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, while generating node id.");
        UA_NodeId_clear(&target);
        return EXIT_FAILURE;
    }
    
    UA_Variant variant;
    UA_Variant_init(&variant);

    data_handler_write(variant, job);

    UA_StatusCode state = UA_Client_writeValueAttribute(opcuac_,
                                                        target,
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
    UA_NodeId_clear(&target);
    UA_Variant_clear(&variant);
    return state;
}

void
OPCUA_Client::opcuac_browse_nodes(std::shared_ptr<JOB> &job, 
                        std::string url)
{
    /**
     * Result with valgrind --leak-check=full ./opcuac_s # browse with storrage
     * 
     * ==29313== HEAP SUMMARY:
     * ==29313==     in use at exit: 0 bytes in 0 blocks
     * ==29313==   total heap usage: 41,385 allocs, 41,385 frees, 241,982,482 bytes allocated
     * ==29313== 
     * ==29313== All heap blocks were freed -- no leaks are possible
     * ==29313== 
     * ==29313== For lists of detected and suppressed errors, rerun with: -s
     * ==29313== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
     * 
     * ~ 0.241 GB
    */

   /**
     * Result with valgrind --leak-check=full ./opcuac_s # browse recursiv 
     * This Funktion is not longer available
     * 
     * ==27529== HEAP SUMMARY:
     * ==27529==     in use at exit: 0 bytes in 0 blocks
     * ==27529==   total heap usage: 56,407 allocs, 56,407 frees, 481,149,778 bytes allocated
     * ==27529== 
     * ==27529== All heap blocks were freed -- no leaks are possible
     * ==27529== 
     * ==27529== For lists of detected and suppressed errors, rerun with: -s
     * ==27529== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
     * 
     * ~ 0.481 GB
     * 
     */

    /* Set default when there is no specific node */
    if(job->init_string == "browse")
    {
        job->type_id = id_type::numeric;
        /* Root folder contains types whose handling is not implemented */
        // job->id_numeric = UA_NS0ID_ROOTFOLDER;
        job->id_numeric = UA_NS0ID_OBJECTSFOLDER;
        job->namespace_index = 0;
    }
    /* Create tree object with printer url */
    std::shared_ptr<Tree_Node> tree = std::make_shared<Tree_Node>(url);
    /* Basic settings from job*/
    tree->set_node_job(job);

    /* List for child nodes */
    std::map<int, std::shared_ptr<Tree_Node>> browse_nodes_list;
    int base_index = {0};
    int start_index = base_index;
    browse_nodes_list.emplace(base_index, tree);

    UA_UInt32 required_id = 0;

    while(true)
    {
        /* = 0 */
        int end_index = base_index;
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Elements in browse_node_list: %lu", 
                    browse_nodes_list.size());
        std::map<int, std::shared_ptr<Tree_Node>> temporary;
        
        for(auto it : browse_nodes_list)
        {
            std::shared_ptr<Tree_Node> pointer = it.second;
            // pointer->print_node();
            UA_BrowseRequest request;
            UA_BrowseRequest_init(&request);

            UA_BrowseResponse response;
            UA_BrowseResponse_init(&response);

            /* Settings */
            request.requestedMaxReferencesPerNode = 0;
            request.nodesToBrowse = UA_BrowseDescription_new();
            request.nodesToBrowseSize = 1;
            /* Build node id*/
            if(pointer->get_ident_type() == id_type::numeric)
            {
                int ns = pointer->get_namespace(); 
                // print_info("namespace: " << ns);
                int id = pointer->get_ident_numeric();
                // print_info("id numeric: " << id);
                request.nodesToBrowse[0].nodeId = 
                        UA_NODEID_NUMERIC(ns, id);
            }
            else if(pointer->get_ident_type() == id_type::string)
            {
                int ns = pointer->get_namespace();
                // print_info("namespace: " << ns);
                std::string temp_string = pointer->get_ident_string();
                const char* str = temp_string.c_str();
                // print_info("id string: " << str);
                request.nodesToBrowse[0].nodeId =
                        UA_NODEID_STRING_ALLOC(ns, str);
            }
            else
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                             "Error, while generating node id.");
            }
            request.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL;

            response = UA_Client_Service_browse(opcuac_,
                                                request);

            if(response.results->statusCode != UA_STATUSCODE_GOOD)
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                                "Error, sending browse request. Statuscode: 0x%X - %s",
                                response.results->statusCode,
                                UA_StatusCode_name(response.results->statusCode));
                // UA_STATUSCODE_BADNODEIDUNKNOWN 0x80340000 = 2150891520
            }
            else
            {    
                for(size_t i = 0; i < response.resultsSize; ++i)
                {
                    for(size_t j = 0; j < response.results[i].referencesSize; ++j)
                    {
                        UA_ReferenceDescription *reference = &(response.results[i].references[j]);

                        node current_node = Tree_Node::init_node();

                        if(Tree_Node::get_response_data(reference, current_node) == EXIT_SUCCESS)
                        {
                            std::shared_ptr<Tree_Node> new_child = 
                                    std::make_shared<Tree_Node>(current_node);

                            if(reference->isForward == true)
                            {
                                ++base_index; /* = 1 */
                                temporary.emplace(base_index, new_child);
                            }
                            pointer->add_child(new_child);
                        }
                        else
                        {
                            /* Only methods, objects and variables are processed */
                            // UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                            //              "Error, node data is invalid.");
                        }
                    }
                }
            }
            UA_BrowseResponse_clear(&response);
            UA_BrowseRequest_clear(&request);
        }
        /* Delete the new browsed nodes */
        while(start_index <= end_index)
        {
            browse_nodes_list.erase(start_index);
            ++start_index;
        }
        browse_nodes_list.insert(temporary.begin(), temporary.end());
        if(browse_nodes_list.empty())
        {
            break;
        }
    }
    /* Print complete tree */
    tree->print_tree_console();
    tree->print_tree_file("browse.txt");
    return;
}

void
OPCUA_Client::opcuac_call_method_PrintData(std::shared_ptr<JOB> &job)
{
    /* Set start settings */
    job->type_id = id_type::numeric;
    job->namespace_index = 3;
    job->id_numeric = 6008;

    /* Generate method id */
    UA_NodeId method_id;
    UA_NodeId_init(&method_id);
    /* Method id*/
    method_id = UA_NODEID_NUMERIC(job->namespace_index, 
                                  job->id_numeric);
    
    /* Node id object MethodSet node */
    UA_NodeId object_id;
    UA_NodeId_init(&object_id);
    /* Object MethodSet */
    object_id = UA_NODEID_NUMERIC(3, 10012);

    /* Language */
    UA_Int32 language;
    // UA_Boolean language; /* Works */
    language = job->intern_data->b_value ? 1 : 0;

    /* Data */
    UA_ByteString byte_string;
    UA_ByteString_init(&byte_string);
    stdstring_to_ua_bytestring(job->intern_data->string_value, &byte_string);
    
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
        UA_NodeId_clear(&method_id);
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
                                         method_id,
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
    UA_NodeId_clear(&method_id);
    return;
}

void
OPCUA_Client::opcuac_call_method_FileUpload(std::shared_ptr<JOB> &job)
{
    /* Set start settings */
    job->type_id = id_type::numeric;
    job->namespace_index = 3;
    job->id_numeric = 20006;

    /* Generate method id */
    UA_NodeId method_id;
    UA_NodeId_init(&method_id);
    /* Method id  */
    method_id = UA_NODEID_NUMERIC(job->namespace_index, 
                                  job->id_numeric);
    
    /* Node id object MethodSet node */
    UA_NodeId object_id;
    UA_NodeId_init(&object_id);
    /* Object MethodSet */
    object_id = UA_NODEID_NUMERIC(3, 10012);

    /* 1. Storage device */
    // UA_UInt32 storage_device = 0; /* Default storage */
    UA_Int32 storage_device = 1; /* IFFS */

    /* Convert */
    std::string fname = job->intern_data->file_name;
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
    std::ifstream file(job->intern_data->file_name, std::ios::binary | std::ios::ate);
    if(!file)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, %s is not available.",
                     job->intern_data->file_name.c_str());
        UA_ByteString_clear(&byte_string);
        UA_String_clear(&filename);
        UA_NodeId_clear(&object_id);
        UA_NodeId_clear(&method_id);
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    /* Read in string */
    std::string buffer(size, '\0');
    if(file.read(&buffer[0], size))
    {
        stdstring_to_ua_bytestring(buffer, &byte_string);
    }
    else
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, convert %s to string failed.",
                     job->intern_data->file_name.c_str());
        UA_ByteString_clear(&byte_string);
        UA_String_clear(&filename);
        UA_NodeId_clear(&object_id);
        UA_NodeId_clear(&method_id);
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
        UA_NodeId_clear(&method_id);
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
                                         method_id,
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
    UA_NodeId_clear(&method_id);
    return;
}

void
OPCUA_Client::opcuac_call_method_PrintCurrentLabel(std::shared_ptr<JOB> &job)
{
    /* Set start settings */
    job->type_id = id_type::numeric;
    job->namespace_index = 3;
    job->id_numeric = 10098;

    /* Generate method id */
    UA_NodeId method_id;
    UA_NodeId_init(&method_id);
    /* Method id  */
    method_id = UA_NODEID_NUMERIC(job->namespace_index, 
                                  job->id_numeric);
    
    /* Node id object MethodSet node */
    UA_NodeId object_id;
    UA_NodeId_init(&object_id);
    /* Object MethodSet */
    object_id = UA_NODEID_NUMERIC(3, 10012);

    /* Storage device */
    UA_UInt32 amount_labels = (UA_UInt32)job->intern_data->u16_value; 

    UA_Variant variant;
    UA_Variant_init(&variant);
    
    UA_Variant_setScalarCopy(&variant, &amount_labels, &UA_TYPES[UA_TYPES_UINT32]);
    

    UA_StatusCode state = UA_Client_call(opcuac_,
                                         object_id,
                                         method_id,
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
    UA_NodeId_clear(&method_id);
    return;
}


void 
OPCUA_Client::callback_state(UA_Client *opcuac_, 
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
OPCUA_Client::callback_subscription_inactivity(UA_Client *opcuac_, 
                        UA_UInt32 subscription_id, 
                        void *subscription_context)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] inactivity callback.",
                subscription_id);
    return;
}

void
OPCUA_Client::callback_subscription_delete(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] delete callback.",
                subscription_id);
    return;
}

void
OPCUA_Client::callback_monitored_item_delete(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context,
                        UA_UInt32 monitored_item_id,
                        void *monitored_item_context)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] | MON ID [%u] delete callback.",
                subscription_id, monitored_item_id);

    /* Clean up */
    delete static_cast<std::shared_ptr<JOB>*>(monitored_item_context);
    return;
}

void
OPCUA_Client::handler_monitored_item(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context,
                        UA_UInt32 monitored_item_id,
                        void *monitored_item_context,
                        UA_DataValue *value)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "SUB ID [%u] | MON ID [%u] data handler.",
                subscription_id, monitored_item_id);

    /* Cast back context to shared_ptr to use data of job */
    auto ptr = static_cast<std::shared_ptr<JOB>*>(monitored_item_context);
    std::shared_ptr<JOB> job = *ptr;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Use job to handle data in callback. Init string: %s [%u]",
                job->init_string.c_str(), job->intern_data->u32_value);

    job->intern_data->u32_value = 43;

    if(UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_DATETIME])) 
    {
        UA_DateTime raw_date = *(UA_DateTime *)value->value.data;
        UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "date is: %02u-%02u-%04u %02u:%02u:%02u.%03u",
                    dts.day, 
                    dts.month, 
                    dts.year, 
                    dts.hour + 1, /* UTC+1 */
                    dts.min, 
                    dts.sec, 
                    dts.milliSec);
    }

    /* Implement more data handling if needed */
    return;
}

void
OPCUA_Client::data_handler_read(UA_Variant &target, 
                        std::shared_ptr<JOB> &job)
{
    if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_DATETIME])) 
    {
        /* To date time struct */
        UA_DateTimeStruct dts = UA_DateTime_toStruct(*(UA_DateTime *)target.data);
        job->intern_data->type_data = data_type::c_datetime;
        /* Time */
        job->intern_data->time.tm_msec = dts.milliSec;
        job->intern_data->time.tm_sec = dts.sec;
        job->intern_data->time.tm_min = dts.min;
        job->intern_data->time.tm_hour = dts.hour + 1; /* UTC+1 */
        /* Date */
        job->intern_data->time.tm_mday = dts.day;
        job->intern_data->time.tm_mon = dts.month - 1;
        job->intern_data->time.tm_year = dts.year - 1900;
        /* Timezone */
        /* ... */
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_BOOLEAN]))
    {
        /* To boolean */
        job->intern_data->type_data = data_type::c_boolean;
        job->intern_data->b_value = *(UA_Boolean *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_INT16]))
    {
        /* To int16 */
        job->intern_data->type_data = data_type::c_int16;
        job->intern_data->s16_value = *(UA_Int16 *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_UINT16]))
    {
        /* To uint16 */
        job->intern_data->type_data = data_type::c_uint16;
        job->intern_data->u16_value = *(UA_UInt16 *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_INT32]))
    {
        /* To int32 */
        job->intern_data->type_data = data_type::c_int32;
        job->intern_data->s32_value = *(UA_Int32 *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_UINT32]))
    {
        /* To uint32*/
        job->intern_data->type_data = data_type::c_uint32;
        job->intern_data->u32_value = *(UA_UInt32 *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_INT64]))
    {
        /* To int64 */
        job->intern_data->type_data = data_type::c_int64;
        job->intern_data->s64_value = *(UA_Int64 *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_UINT64]))
    {
        /* To uint64 */
        job->intern_data->type_data = data_type::c_uint64;
        job->intern_data->u64_value = *(UA_UInt64 *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_FLOAT]))
    {
        /* To float */
        job->intern_data->type_data = data_type::c_f32;
        job->intern_data->f32_value = *(UA_Float *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_DOUBLE]))
    {
        /* To double */
        job->intern_data->type_data = data_type::c_f64;
        job->intern_data->f64_value = *(UA_Double *)target.data;
    }
    else if(UA_Variant_hasScalarType(&target, &UA_TYPES[UA_TYPES_STRING]))
    {
        /* To string */
        job->intern_data->type_data = data_type::c_string;
        UA_String ua_string = *(UA_String *)target.data;
        job->intern_data->string_value = std::string(reinterpret_cast<char*>(ua_string.data), ua_string.length);
    }
    else
    {
        /* Implement more types ... */
        /* types_generated.h */
        job->intern_data->type_data = data_type::DEFAULT;
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, node datatype in UA_Variant not known.");
    }
    return;
}

void
OPCUA_Client::data_handler_write(UA_Variant &target, 
                        std::shared_ptr<JOB> &job)
{
    if(job->intern_data->type_data == data_type::c_datetime)
    {
        // typedef struct UA_DateTimeStruct {
        //     UA_UInt16 nanoSec;
        //     UA_UInt16 microSec;
        //     UA_UInt16 milliSec;
        //     UA_UInt16 sec;   
        //     UA_UInt16 min;
        //     UA_UInt16 hour;
        //     UA_UInt16 day;   /* From 1 to 31 */
        //     UA_UInt16 month; /* From 1 to 12 */
        //     UA_UInt16 year;
        // } UA_DateTimeStruct;

        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Time write: %d.%d.%d | %d:%d:%d:%d",
                    job->intern_data->time.tm_mday,
                    job->intern_data->time.tm_mon,
                    job->intern_data->time.tm_year,
                    job->intern_data->time.tm_hour,
                    job->intern_data->time.tm_min,
                    job->intern_data->time.tm_sec,
                    job->intern_data->time.tm_msec);

        UA_DateTimeStruct dts;
        dts.nanoSec = 0;
        dts.microSec = 0;
        dts.milliSec = job->intern_data->time.tm_msec;
        dts.sec = job->intern_data->time.tm_sec;
        dts.min = job->intern_data->time.tm_min;
        dts.hour = job->intern_data->time.tm_hour - 1; /* UCT+1 timezone*/
        dts.day = job->intern_data->time.tm_mday;
        dts.month = job->intern_data->time.tm_mon + 1;
        dts.year = job->intern_data->time.tm_year + 1900;

        UA_DateTime date_time;
        UA_DateTime_init(&date_time);

        date_time = UA_DateTime_fromStruct(dts);
        UA_Variant_setScalarCopy(&target, &date_time, &UA_TYPES[UA_TYPES_DATETIME]);

        UA_DateTime_clear(&date_time);
    }
    else if(job->intern_data->type_data == data_type::c_boolean)
    {
        UA_Boolean b_value = job->intern_data->b_value;
        UA_Variant_setScalarCopy(&target, &b_value, &UA_TYPES[UA_TYPES_BOOLEAN]);
    }
    else if(job->intern_data->type_data == data_type::c_int16)
    {
        UA_Int16 s16_value = job->intern_data->s16_value;
        UA_Variant_setScalarCopy(&target, &s16_value, &UA_TYPES[UA_TYPES_INT16]);
    }
    else if(job->intern_data->type_data == data_type::c_uint16)
    {
        UA_UInt16 u16_value = job->intern_data->u16_value;
        UA_Variant_setScalarCopy(&target, &u16_value, &UA_TYPES[UA_TYPES_UINT16]);
    }
    else if(job->intern_data->type_data == data_type::c_int32)
    {
        UA_Int32 s32_value = job->intern_data->s32_value;
        UA_Variant_setScalarCopy(&target, &s32_value, &UA_TYPES[UA_TYPES_INT32]);
    }
    else if(job->intern_data->type_data == data_type::c_uint32)
    {
        UA_UInt32 u32_value = job->intern_data->u32_value;
        UA_Variant_setScalarCopy(&target, &u32_value, &UA_TYPES[UA_TYPES_UINT32]);
    }
    else if(job->intern_data->type_data == data_type::c_int64)
    {
        UA_Int64 s64_value = job->intern_data->s64_value;
        UA_Variant_setScalarCopy(&target, &s64_value, &UA_TYPES[UA_TYPES_INT64]);
    }
    else if(job->intern_data->type_data == data_type::c_uint64)
    {
        UA_UInt64 u64_value = job->intern_data->u64_value;
        UA_Variant_setScalarCopy(&target, &u64_value, &UA_TYPES[UA_TYPES_UINT64]);
    }
    else if(job->intern_data->type_data == data_type::c_f32)
    {
        UA_Float f32_value = job->intern_data->f32_value;
        UA_Variant_setScalarCopy(&target, &f32_value, &UA_TYPES[UA_TYPES_FLOAT]);
    }
    else if(job->intern_data->type_data == data_type::c_f64)
    {
        UA_Double f64_value = job->intern_data->f64_value;
        UA_Variant_setScalarCopy(&target, &f64_value, &UA_TYPES[UA_TYPES_DOUBLE]);
    }
    else if(job->intern_data->type_data == data_type::c_string)
    {
        std::string std_string = job->intern_data->string_value;
        UA_String ua_string = UA_STRING_ALLOC(std_string.c_str());
        UA_Variant_setScalarCopy(&target, &ua_string, &UA_TYPES[UA_TYPES_STRING]);
        UA_String_clear(&ua_string);
    }
    else if(job->intern_data->type_data == data_type::DEFAULT)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, node datatype in UA_Variant not known.");
    }
    else
    {
        /* Implement more types ... */
        /* types_generated.h */
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, node datatype in UA_Variant not known.");
    }
    return;
}

std::string 
OPCUA_Client::ua_string_to_stdstring(const UA_String *uaString) 
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
OPCUA_Client::stdstring_to_ua_bytestring(const std::string &str,
                        UA_ByteString *uaStr)
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