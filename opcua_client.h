#pragma once

#ifndef __OPCUA_CLIENT_H__
#define __OPCUA_CLIENT_H__

#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>
#include <sstream>

#include "tools.h"
#include "data_types.h"
#include "tree_node.h"

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server_config_default.h>
#include <open62541/client.h>

#define CLIENT_TIMEOUT                  1000
#define CLIENT_CONNECTIONS_ATTEMPTS     4
#define CLIENT_CONNECTION_WAITING_TIME  100
#define CLIENT_CONNECTION_RETRY_TIME    10000
#define CLIENT_ITERATE_TIME             100

#define BROWSE_START_LAYER              1


/* Class OPCUA_Client */
/* ^^^^^^^^^^^^^^^^^^ */

class OPCUA_Client 
{

public:

    OPCUA_Client();

    ~OPCUA_Client();

    void
    opcuac_init(void);

    int 
    opcuac_connect(const std::string &url, 
                        client_arg &arg);

    void 
    opcuac_disconnect();

    int
    opcuac_run_iterate();

    /* Services */
    void
    opcuac_add_monitored_item(std::shared_ptr<JOB>&);

    void
    opcuac_delete_monitored_item(std::shared_ptr<JOB>&);

    void 
    opcuac_read_node(std::shared_ptr<JOB>&);

    int
    opcuac_write_node(std::shared_ptr<JOB>&);

    void
    opcuac_browse_nodes(std::shared_ptr<JOB> &job, 
                        std::string url);

    void
    opcuac_call_method_PrintData(std::shared_ptr<JOB> &job);

    void
    opcuac_call_method_FileUpload(std::shared_ptr<JOB> &job);

    void
    opcuac_call_method_PrintCurrentLabel(std::shared_ptr<JOB> &job);


    /* Callbacks */
    static void 
    callback_state(UA_Client *opcuac_, 
                        UA_SecureChannelState channel_state, 
                        UA_SessionState session_state, 
                        UA_StatusCode connect_status);

    static void
    callback_subscription_inactivity(UA_Client *opcuac_, 
                        UA_UInt32 subscription_id, 
                        void *subscription_context);

    static void
    callback_subscription_delete(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context);

    static void
    callback_monitored_item_delete(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context,
                        UA_UInt32 monitored_item_id,
                        void *monitored_item_context);

    /* Handler */
    static void
    handler_monitored_item(UA_Client *opcuac_,
                        UA_UInt32 subscription_id,
                        void *subscription_context,
                        UA_UInt32 monitored_item_id,
                        void *monitored_item_context,
                        UA_DataValue *value);

private:

    void 
    data_handler_read(UA_Variant &target, 
                        std::shared_ptr<JOB> &job);

    void
    data_handler_write(UA_Variant &target,
                        std::shared_ptr<JOB> &job);

    std::string 
    ua_string_to_stdstring(const UA_String *uaString); 

    void
    stdstring_to_ua_bytestring(const std::string &str, 
                        UA_ByteString *uaStr);

    UA_Client *opcuac_;
    UA_ClientConfig *opcuac_c_;

    std::map<std::shared_ptr<JOB>, MITEM> monitored_items_;

    static UA_SessionState actual_session_state_;

};

#endif // __OPCUA_CLIENT_H__