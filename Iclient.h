#pragma once

#include <memory>

#include "job.h"

/* Reference on std::shared_ptr for short overhead */
typedef std::shared_ptr<JOB>& sptr;

/* Connection arguments */
struct server_arguments
{
    std::string server_ip;
    std::string server_port; 
    std::string server_username; 
    std::string server_password;
    bool server_user_mode;
    std::string server_url;
};

namespace opcuac
{
    /**
     * Class IClient - OPC UA client interface 
     * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
     */
    class IClient 
    {

    public:
        /**
         * Basics 
         * ^^^^^^
         * 
         * Copy-, copy asign, move and move asign contructor delete explicit. If these are 
         * required, they must be implemented subsequently. Each client must be initialized 
         * and connected. The user must decide how the connection is disconnected or how 
         * the processing of the tasks works.
        */

        IClient();

        IClient(const IClient&) = delete;

        IClient&
        operator=(const IClient&) = delete;

        IClient(IClient&&) = delete;

        IClient&
        operator=(IClient&&) = delete;

        virtual
        ~IClient();

        /* Init client, init member values and start basic configuration. */
        virtual void
        init(void) = 0;

        /* Attempts to connect the client several times. */
        virtual int 
        connect(const server_arguments&) = 0;

        /* Clean up and controlled shutdown of the client. */
        virtual void 
        disconnect(void);

        /* Client workbench. */
        virtual int
        run_iterate(void);

        /**
         * Printer services
         * ^^^^^^^^^^^^^^^^
         * 
         * These are not just methods that have been implemented exclusively for cab printers.
         * These services can be used to read and write the structure tree. 
        */

        /* Monitors the node and reports if the status or value changes. */
        virtual void
        add_monitored_item(sptr) = 0;

        /* Remove monitored item. */
        virtual void
        del_monitored_item(sptr) = 0;

        /* Read node attributes. */
        virtual void 
        read_node(sptr) = 0;

        /* Write node attributes. */
        virtual int
        write_node(sptr) = 0;

        /* Browse nodes structure. */
        virtual void
        browse(sptr) = 0;

        /**
         * Printer methods
         * ^^^^^^^^^^^^^^^
         * 
         * Only the pure virtual methods are currently implemented and usable. This interface 
         * gives you an overview of all methods that are available on the printer. feel free 
         * to implement additional methods. 
         * */

        /* Returns the last printed label in PNG format as processed by the printengine. */
        virtual void
        bitmap(sptr);

        /* Cancels the current print job. */
        virtual void
        cancel_job(sptr);

        /* Exports the printer's configuration in XML format. */
        virtual void
        export_settings(sptr);

        /* Delete given file on a printer's storage device. */
        virtual void
        file_delete(sptr);

        /* Download e.g. a label format or image file from a printer's storage device. */
        virtual void
        file_download(sptr);

        /* List files on given storage device. */
        virtual void
        file_list(sptr);

        /* Upload e.g. a label format or image file to a printer's storage device. */
        virtual void
        file_upload(sptr) = 0;

        /* Query for available fonts. */
        virtual void
        font_list(sptr);

        /**
         * Imports a custom TLS certificate in PEM format. This will override the supplied 
         * self-signed certificate. */
        virtual void
        import_certificateTLS(sptr);

        /* Imports an XML printer configuration. */
        virtual void
        import_settings(sptr);

        /* Returns the last printed label in PNG format with viewport adopted to job settings. */
        virtual void
        label_bitmap(sptr);

        /* Loads a label from any attached printer storage device. */
        virtual void
        load_label(sptr);

        /**
         * Prints given JScript or ZPL data directly on the printer. The method expects UTF-8 
         * content encoding. */
        virtual void
        print_data(sptr) = 0;

        /* Query for object availability. */
        virtual void
        query_object(sptr);

        /* Resets passwords to factory defaults. */
        virtual void
        reset_passwords(sptr);

        /* Resets to factory defaults. */
        virtual void
        reset_settings(sptr);

        /* Returns a screenshot of the printer's display in PNG format. */
        virtual void
        screen(sptr);

        /* Set or unset an I/O input, mostly adequate with PAUSE and LBLROT. */
        virtual void
        set_input(sptr);

        /* Sets an OPC UA servers URL for data retrieval in standalone mode. */
        virtual void
        set_opcua_client_url(sptr);

        /* Cancels all print jobs scheduled or currently printed by the printer. */
        virtual void
        total_cancel(sptr);

        /**
         * Triggers an I/O input, one of FSTLBL, REPRINT, START, LBLREM, JOBDEL, RSTERR, 
         * STOP or LBLFEED. PAUSE and LBLROT is not implemented as trigger, use SetInput 
         * instead. */
        virtual void
        trigger_input(sptr);

        /** 
         * Printer interpreter methods
         * ^^^^^^^^^^^^^^^^^^^^^^^^^^^
         * 
         * The methods in this area are only available if there is a label in the printer buffer. 
         */

        /* Make given number of copies from current label. */
        virtual void
        print_current_label(sptr) = 0;

    };
    
} // namespace opcuac

/* Eof */