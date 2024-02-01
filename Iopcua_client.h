#include <iostream>
#include <memory>
#include <cstring>

class IOPCUA_Client 
{

public:
    IOPCUA_Client();

    IOPCUA_Client(const IOPCUA_Client&) = delete;

    IOPCUA_Client&
    operator=(const IOPCUA_Client&) = delete;

    IOPCUA_Client(IOPCUA_Client&&) = delete;

    IOPCUA_Client&
    operator=(IOPCUA_Client&&) = delete;

    virtual
    ~IOPCUA_Client();

    virtual void
    opcuac_init(void) = 0;

    virtual int 
    opcuac_connect(const std::string &url, 
                        client_arg &arg);

    virtual void 
    opcuac_disconnect(void);

    virtual int
    opcuac_run_iterate(void);

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