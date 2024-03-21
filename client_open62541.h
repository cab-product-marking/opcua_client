#pragma once

#include <iostream>
#include <map>
#include <cstring>

#include "Iclient_opcua.h"
#include "job_open62541_node.h" 
#include "data_open62541.h"
#include "common_types.h"
#include "node_tree.h"

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server_config_default.h>
#include <open62541/client.h>

#define TIMEOUT                     1000
#define CONNECTION_ATTEMPTS         4
#define CONNECTION_WAITING_TIME     100
#define CONNECTION_RETRY_TIME       10000

#define ITERATIE_TIME               100

#define START_LAYER                 1

namespace open62541
{
    /**
     * Class Client 
     */
    class Client : public opcuac::IClient
    {
    public:
    
        /* Basics */

        Client() = default;

        Client(const Client&) = delete;

        Client&
        operator=(const Client&) = delete;

        Client(Client&&) = delete;

        Client&
        operator=(Client&&) = delete;

        virtual
        ~Client() = default;

        void
        init(void) override;

        int
        connect(const open62541::server_arguments&);

        void
        disconnect(void) override;

        int
        run_iterate(void) override;

        /* Printer services */

        void
        add_monitored_item(opcuac::jobsptr) override;

        void
        del_monitored_item(opcuac::jobsptr) override;

        void
        read_node(opcuac::jobsptr) override;

        void
        write_node(opcuac::jobsptr) override;

        void
        browse(opcuac::jobsptr) override;

        /* Printer methods */

        void 
        file_upload(opcuac::jobsptr) override;

        void
        print_data(opcuac::jobsptr) override;

        /* Printer interpreter methods */

        void 
        print_current_label(opcuac::jobsptr) override;

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

        static void
        data_handler_read(UA_Variant&, opcuac::jobsptr);

        static void
        data_handler_write(UA_Variant&, opcuac::jobsptr);

        std::string
        to_string(const UA_String*);

        void
        to_uabytestring(const std::string&, UA_ByteString*);

        UA_Client* opcuac_;
        UA_ClientConfig* opcuac_c_;

        std::map<opcuac::jobsptr, open62541::mitem_t> items_;

        static UA_SessionState actual_session_state_;

    };

} // namespace open62541

/* Eof */