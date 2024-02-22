#pragma once

#include <memory>

#include "Ijob.h"

typedef std::shared_ptr<opcuac::Job> job_sptr;

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
         * Each client must be initialized and connected. The user must decide how the 
         * connection is disconnected or how the processing of the tasks works.
        */

        /* Init client, init member values and start basic configuration. */
        virtual void
        init(void) = 0;

        /* Attempts to connect the client several times. */
        virtual void
        connect(void) {};

        /* Clean up and controlled shutdown of the client. */
        virtual void 
        disconnect(void) {};

        /* Client workbench. */
        virtual int
        run_iterate(void) = 0;

        /**
         * Printer services
         * ^^^^^^^^^^^^^^^^
         * 
         * These are not just methods that have been implemented exclusively for cab printers.
         * These services can be used to read and write the structure tree. 
        */

        /* Monitors the node and reports if the status or value changes. */
        virtual void
        add_monitored_item(job_sptr) = 0;

        /* Remove monitored item. */
        virtual void
        del_monitored_item(job_sptr) = 0;

        /* Read node attributes. */
        virtual void 
        read_node(job_sptr) = 0;

        /* Write node attributes. */
        virtual int
        write_node(job_sptr) = 0;

        /* Browse nodes structure. */
        virtual void
        browse(job_sptr) = 0;

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
        bitmap(job_sptr) {};

        /* Cancels the current print job. */
        virtual void
        cancel_job(job_sptr) {};

        /* Exports the printer's configuration in XML format. */
        virtual void
        export_settings(job_sptr) {};

        /* Delete given file on a printer's storage device. */
        virtual void
        file_delete(job_sptr) {};

        /* Download e.g. a label format or image file from a printer's storage device. */
        virtual void
        file_download(job_sptr) {};

        /* List files on given storage device. */
        virtual void
        file_list(job_sptr) {};

        /* Upload e.g. a label format or image file to a printer's storage device. */
        virtual void
        file_upload(job_sptr) {};

        /* Query for available fonts. */
        virtual void
        font_list(job_sptr) {};

        /**
         * Imports a custom TLS certificate in PEM format. This will override the supplied 
         * self-signed certificate. */
        virtual void
        import_certificateTLS(job_sptr) {};

        /* Imports an XML printer configuration. */
        virtual void
        import_settings(job_sptr) {};

        /* Returns the last printed label in PNG format with viewport adopted to job settings. */
        virtual void
        label_bitmap(job_sptr) {};

        /* Loads a label from any attached printer storage device. */
        virtual void
        load_label(job_sptr) {};

        /**
         * Prints given JScript or ZPL data directly on the printer. The method expects UTF-8 
         * content encoding. */
        virtual void
        print_data(job_sptr) {};

        /* Query for object availability. */
        virtual void
        query_object(job_sptr) {};

        /* Resets passwords to factory defaults. */
        virtual void
        reset_passwords(job_sptr) {};

        /* Resets to factory defaults. */
        virtual void
        reset_settings(job_sptr) {};

        /* Returns a screenshot of the printer's display in PNG format. */
        virtual void
        screen(job_sptr) {};

        /* Set or unset an I/O input, mostly adequate with PAUSE and LBLROT. */
        virtual void
        set_input(job_sptr) {};

        /* Sets an OPC UA servers URL for data retrieval in standalone mode. */
        virtual void
        set_opcua_client_url(job_sptr) {};

        /* Cancels all print jobs scheduled or currently printed by the printer. */
        virtual void
        total_cancel(job_sptr) {};

        /**
         * Triggers an I/O input, one of FSTLBL, REPRINT, START, LBLREM, JOBDEL, RSTERR, 
         * STOP or LBLFEED. PAUSE and LBLROT is not implemented as trigger, use SetInput 
         * instead. */
        virtual void
        trigger_input(job_sptr) {};

        /** 
         * Printer interpreter methods
         * ^^^^^^^^^^^^^^^^^^^^^^^^^^^
         * 
         * The methods in this area are only available if there is a label in the printer buffer. 
         */

        /* Make given number of copies from current label. */
        virtual void
        print_current_label(job_sptr) {};

    };

    class Client : public IClient
    {
    public:
    
        Client() = default;

        virtual 
        ~Client() = default;

    };
    
} // namespace opcuac

/* Eof */