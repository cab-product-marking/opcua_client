#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <memory>
#include <fstream>

#include "common_defines.h"
#include "common_types.h"
#include "logger.h"
#include "os_wrappers.h"

#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>

#define NODE_IDENT          "Ident"
#define NODE_ID             "Id"
#define NODE_NS             "Ns"
#define NODE_TYPE           "Type"

namespace open62541
{
    /* Fd */
    class NodeTree;
    class Node;

    typedef std::shared_ptr<NodeTree> ntreesptr;
    typedef std::shared_ptr<Node> nodesptr;

    /* Class NodeTree */
    
    class NodeTree
    {
    public:

        NodeTree(UA_Client*) noexcept;

        NodeTree(const NodeTree&) = delete;

        NodeTree&
        operator=(const NodeTree&) = delete;

        NodeTree(NodeTree&&) = delete;

        NodeTree&
        operator=(NodeTree&&) = delete;

        virtual
        ~NodeTree() noexcept;

        void
        print(UA_NodeId&, std::map<int, bool> = {}, int layer = 0);

        void
        printfile(UA_NodeId&, std::string);

        void
        print(UA_NodeId&, std::string, std::map<int, bool> = {}, int layer = 0);

        std::string
        color(int);

        std::vector<open62541::nodesptr>
        get_childs(UA_NodeId&);

        nodesptr
        build_node(UA_ReferenceDescription*);

    private:

        UA_Client* client_;

    };

    std::ostream&
    operator<<(std::ostream& os, const Node& node);

    /* Class Node */

    class Node
    {
    public:

        virtual 
        ~Node() = default;

        virtual UA_NodeId&
        nodeID(void) = 0;

        virtual void
        print(std::ostream&) const = 0;

        virtual void
        add_child(open62541::nodesptr) = 0;

        friend std::ostream&
        operator<<(std::ostream&, const Node&);

    };

    /* Class NodeNumeric */

    class NodeNumeric : public Node
    {
    public:

        NodeNumeric() = delete;

        NodeNumeric(std::map<std::string, std::string>, int, int) noexcept;

        ~NodeNumeric() noexcept override;

        UA_NodeId&
        nodeID(void) override;

        void
        print(std::ostream&) const override;

        void
        add_child(open62541::nodesptr) override;

     protected:

        std::map<std::string, std::string> param_;
        UA_NodeId target_;
        std::vector<open62541::nodesptr> childs_;

    };

    /* Class NodeString */

    class NodeString : public Node
    {
    public:

        NodeString() = delete;

        NodeString(std::map<std::string, std::string>, std::string, int) noexcept;

        ~NodeString() noexcept override;

        UA_NodeId&
        nodeID(void) override;

        void
        print(std::ostream&) const override;

        void
        add_child(open62541::nodesptr) override;

     protected:

        std::map<std::string, std::string> param_;
        UA_NodeId target_;
        std::vector<open62541::nodesptr> childs_;

    };
    
} // namespace open62541

/* Eof */