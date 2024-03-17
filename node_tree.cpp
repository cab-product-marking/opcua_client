#include "node_tree.h"

using namespace open62541;

/* Class NodeTree */

NodeTree::NodeTree(UA_Client* client) noexcept : client_(client)
{
    std::cout << "\n";
    std::cout << FONT_WHITE 
        << "BROWSE[DISPLAY_NAME][IDENTIFIER][NS][NODE_TYPE]"
        << FONT_RESET 
        << std::endl;
}

NodeTree::~NodeTree()
{
    std::cout << "\n";
}

void
NodeTree::print(UA_NodeId& id, std::map<int, bool> map, int layer) 
{
    std::vector<open62541::nodesptr> dummy;
    std::string praefix = NODE_ADD;
    map.emplace(layer, true);

    dummy = get_childs(id);

    if(dummy.size() != 0)
    {
        auto last = dummy.end() - 1;
        for(auto it : dummy)
        {
            if(it == *last)
            {
                map.find(layer)->second = false;
                praefix = NODE_NEW;
            }
            for(auto el : map)
            {
                if(el.first != layer)
                {
                    if(el.second == true)
                    {
                        std::cout << NODE_DOWN;
                    }
                    if(el.second == false)
                    {
                        std::cout << NODE_EMPTY;
                    }
                }
            }
            
            std::cout << praefix << color(layer) << *it << FONT_RESET << std::endl;

            print(it->nodeID(), map, layer + 1);
        }
    }
    return;
}

void
NodeTree::printfile(UA_NodeId& id, std::string filename)
{
    std::ofstream file(filename);
    if(!file.is_open())
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, create file browse_out.txt");
        cLOG(Level::ERROR, "Can not create file browse.txt");
    }
    else
    {
        file.clear();
        file << "BROWSE[DISPLAY_NAME][IDENTIFIER][NS][NODE_TYPE]" << std::endl;
        this->print(id, filename);
    }
    file.close();
    return;
}

void 
NodeTree::print(UA_NodeId& id, std::string filename, std::map<int, bool> map, int layer)
{
    /* Open to write file */
    std::ofstream file(filename, std::ios::app);
    if(!file.is_open())
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Error, while open file output_browse.txt");
        cLOG(Level::ERROR, "Can not open file browse.txt");
    }
    else
    {
        /* Same procedure as in the method above */
        std::vector<open62541::nodesptr> dummy;
        std::string praefix = NODE_ADD;
        map.emplace(layer, true);
        dummy = get_childs(id);
        if(dummy.size() != 0)
        {
            auto last = dummy.end() - 1;
            for(auto it : dummy)
            {
                if(it == *last)
                {
                    map.find(layer)->second = false;
                    praefix = NODE_NEW;
                }
                for(auto el : map)
                {
                    if(el.first != layer)
                    {
                        if(el.second == true)
                        {
                            file << NODE_DOWN;
                        }
                        if(el.second == false)
                        {
                            file << NODE_EMPTY;
                        }
                    }
                }
                file << praefix << *it << std::endl;
                print(it->nodeID(), filename, map, layer + 1);
            }
        }
    }
    file.close();
    return;
}

std::string
NodeTree::color(int layer)
{
    std::string color;
    switch (layer)
    {
    case 0:
        color = FONT_RED;
        break;
    case 1:
        color = FONT_GREEN;
        break;
    case 2:
        color = FONT_BROWN;
        break;
    case 3:
        color = FONT_BLUE;
        break;
    case 4:
        color = FONT_MAGENTA;
        break;
    case 5:
        color = FONT_CYAN;
        break;
    case 6:
        color = FONT_YELLOW;
        break;
    case 7:
        color = FONT_LIGHT_GRAY;
        break;
    default:
        color = FONT_LIGHT_PURPLE;
        break;
    }
    return color;
}

std::vector<open62541::nodesptr>
NodeTree::get_childs(UA_NodeId& id)
{
    std::vector<open62541::nodesptr> dummy;

    UA_BrowseRequest request;
    UA_BrowseRequest_init(&request);

    UA_BrowseResponse response;
    UA_BrowseResponse_init(&response);

    UA_NodeId target;
    UA_NodeId_init(&target);
    UA_NodeId_copy(&id, &target);

    /* Settings */
    request.requestedMaxReferencesPerNode = 0;
    request.nodesToBrowse = UA_BrowseDescription_new();
    request.nodesToBrowseSize = 1;
    request.nodesToBrowse[0].nodeId = target;
    request.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL;

    response = UA_Client_Service_browse(this->client_,
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

                auto child = this->build_node(reference);

                /* Check the assembled children for validity */
                if(child != nullptr)
                {
                    dummy.push_back(child);
                }
            }
        }
    }
    UA_BrowseResponse_clear(&response);
    UA_BrowseRequest_clear(&request);
    return dummy;
}

nodesptr
NodeTree::build_node(UA_ReferenceDescription* ref)
{
    std::map<std::string, std::string> dummy;

    if(ref->displayName.text.length == 0 || ref->displayName.text.data == nullptr)
    {
        cLOG(Level::ERROR, "Node data in reference description invalid.");
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                     "Node data in reference description invalid.");
        return nullptr;
    }

    /* Insert display name */
    std::string str;
    str.reserve(ref->displayName.text.length);
    std::copy(ref->displayName.text.data,
                ref->displayName.text.data +
                ref->displayName.text.length,
                std::back_inserter(str));
    dummy.emplace(NODE_IDENT, str);

    if((ref->nodeClass == UA_NODECLASS_VARIABLE ||
        ref->nodeClass == UA_NODECLASS_OBJECT ||
        ref->nodeClass == UA_NODECLASS_METHOD) &&
        (str != "Mandatory" && str != "NamingRule" && str != "Optional"))
    {
        /* Insert node type */
        if(ref->nodeClass == UA_NODECLASS_METHOD)
        {
            dummy.emplace(NODE_TYPE, "[method]");
        }
        if(ref->nodeClass == UA_NODECLASS_VARIABLE)
        {
            dummy.emplace(NODE_TYPE, "[variable]");
        }
        if(ref->nodeClass == UA_NODECLASS_OBJECT)
        {
            dummy.emplace(NODE_TYPE, "[object]");
        }
        else
        {
            dummy.emplace(NODE_TYPE, "[undefined]");
        }

        /* Insert namespace */
        dummy.emplace(NODE_NS, 
                "[" + std::to_string(ref->nodeId.nodeId.namespaceIndex) + "]");

        /* Insert identifier and build object */
        if(ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
        {
            dummy.emplace(NODE_ID, 
                    "[" + std::to_string(ref->nodeId.nodeId.identifier.numeric) + "]");

            auto node = std::make_shared<NodeNumeric>(dummy, 
                    ref->nodeId.nodeId.identifier.numeric,
                    ref->nodeId.nodeId.namespaceIndex);

            return node;
        }
        else if(ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING)
        {
            if(ref->nodeId.nodeId.identifier.string.length == 0 ||
                ref->nodeId.nodeId.identifier.string.data == nullptr)
            {
                UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                             "Error, node id data string invalid.");
                cLOG(Level::ERROR, "Node id data string invalid");
                return nullptr;
            }
            std::string idstr;
            idstr.reserve(ref->nodeId.nodeId.identifier.string.length);
            std::copy(ref->nodeId.nodeId.identifier.string.data,
                        ref->nodeId.nodeId.identifier.string.data +
                        ref->nodeId.nodeId.identifier.string.length,
                        std::back_inserter(idstr));

            dummy.emplace(NODE_ID, "[" + idstr + "]");

            auto node = std::make_shared<NodeString>(dummy,
                    idstr, ref->nodeId.nodeId.namespaceIndex);

            return node;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        // UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        //             "No object, method or variable.");
        // cLOG(Level::ERROR, "No object, method or variable.");
        return nullptr;
    }  
}

/* Output stream operator definition */

std::ostream&
open62541::operator<<(std::ostream& os, const open62541::Node& node)
{
    node.print(os);
    return os;
}

/* Class NodeNumeric */

NodeNumeric::NodeNumeric(std::map<std::string, std::string> map, int id, int ns) noexcept
        : param_(map)
{
    UA_NodeId_init(&this->target_);
    this->target_ = UA_NODEID_NUMERIC(ns, id);
}

NodeNumeric::~NodeNumeric() noexcept
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
NodeNumeric::nodeID(void) 
{
    return this->target_;
}

void
NodeNumeric::print(std::ostream& os) const
{
    os 
    << param_.find(NODE_IDENT)->second 
    << param_.find(NODE_ID)->second
    << param_.find(NODE_NS)->second
    << param_.find(NODE_TYPE)->second;
    return;
}

void
NodeNumeric::add_child(open62541::nodesptr node)
{
    this->childs_.push_back(node);
}

/* Class NodeString */

NodeString::NodeString(std::map<std::string, std::string> map, std::string id, int ns) noexcept
        : param_(map)
{
    UA_NodeId_init(&this->target_);
    this->target_ = UA_NODEID_STRING_ALLOC(ns, id.c_str());
}

NodeString::~NodeString() noexcept
{
    UA_NodeId_clear(&this->target_);
}

UA_NodeId&
NodeString::nodeID(void) 
{
    return this->target_;
}

void
NodeString::print(std::ostream& os) const
{
    os 
    << param_.find(NODE_IDENT)->second 
    << param_.find(NODE_ID)->second
    << param_.find(NODE_NS)->second
    << param_.find(NODE_TYPE)->second;
    return;
}

void
NodeString::add_child(open62541::nodesptr node)
{
    this->childs_.push_back(node);
}

/* Eof */