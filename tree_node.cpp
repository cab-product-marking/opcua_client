#include "tree_node.h"

Tree_Node::Tree_Node(std::string url) : printer_identifier_(url)
{
#ifndef NDEBUG
    std::cout   << FONT_MAGENTA 
                << "Tree_Node::Tree_Node(std::string url);" 
                << FONT_RESET << std::endl;
#endif // NDEBUG 

    node_ = init_node();
    node_.display_name = {url};
}

Tree_Node::Tree_Node(node actual_node) : node_(actual_node)
{
#ifndef NDEBUG
    std::cout   << FONT_MAGENTA 
                << "Tree_Node::Tree_Node(node actual_node);" 
                << FONT_RESET << std::endl;
#endif // NDEBUG 
}

Tree_Node::~Tree_Node()
{
#ifndef NDEBUG
    std::cout   << FONT_MAGENTA 
                << "Tree_Node::~Tree_Node();" 
                << FONT_RESET << std::endl;
#endif // NDEBUG
}

node
Tree_Node::init_node(void)
{
    node init_node;
    init_node.namespace_index = {- 1};
    init_node.ident_type = {id_type::DEFAULT};
    init_node.identifier_string = {"DEFAULT"};
    init_node.identifier_int = {- 1};
    init_node.node_art = {node_type::DEFAULT};
    init_node.display_name = {"*_*"};
    return init_node;
}

void 
Tree_Node::init_node(node &init_node)
{
    init_node.namespace_index = {- 1};
    init_node.ident_type = {id_type::DEFAULT};
    init_node.identifier_string = {"DEFAULT"};
    init_node.identifier_int = {- 1};
    init_node.node_art = {node_type::DEFAULT};
    init_node.display_name = {"*_*"};
    return;
}

void 
Tree_Node::add_child(std::shared_ptr<Tree_Node> shared)
{
    children_.push_back(shared);
    return;
}

int
Tree_Node::get_response_data(UA_ReferenceDescription *ref, 
                        node &current_node)
{
    int state = {EXIT_FAILURE};
    /* Check identifier */
    if(ref->displayName.text.length == 0 ||
        ref->displayName.text.data == nullptr)
    {
        // UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        //              "Error, node id data string invalid."):
        cLOG(Level::ERROR, "Node id data sting invalid");
        return state;
    }
    std::string str;
    str.reserve(ref->displayName.text.length);
    std::copy(ref->displayName.text.data,
                ref->displayName.text.data +
                ref->displayName.text.length,
                std::back_inserter(str));

    if((ref->nodeClass == UA_NODECLASS_VARIABLE ||
        ref->nodeClass == UA_NODECLASS_OBJECT ||
        ref->nodeClass == UA_NODECLASS_METHOD) &&
        (str != "Mandatory" && str != "NamingRule" && str != "Optional"))
    {
        /* namespace_index */
        current_node.namespace_index = ref->nodeId.nodeId.namespaceIndex;
        /* identifier_type */
        if(ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
        {
            current_node.ident_type = id_type::numeric;
            /* identifier_int */
            current_node.identifier_int = 
                    ref->nodeId.nodeId.identifier.numeric;
        }
        else if(ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING)
        {
            current_node.ident_type = id_type::string;
            /* identifier_string */
            if(ref->nodeId.nodeId.identifier.string.length == 0 ||
                ref->nodeId.nodeId.identifier.string.data == nullptr)
            {
                // UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                //              "Error, node id data string invalid.");
                cLOG(Level::ERROR, "Node id data string invalid");
                return state;
            }
            std::string str;
            str.reserve(ref->nodeId.nodeId.identifier.string.length);
            std::copy(ref->nodeId.nodeId.identifier.string.data,
                        ref->nodeId.nodeId.identifier.string.data +
                        ref->nodeId.nodeId.identifier.string.length,
                        std::back_inserter(str));
            current_node.identifier_string = str;
        }
        else
        {
            current_node.ident_type = id_type::DEFAULT;
        }
        /* node_art */
        if(ref->nodeClass == UA_NODECLASS_METHOD)
        {
            current_node.node_art = {node_type::method};
        }
        else if(ref->nodeClass == UA_NODECLASS_VARIABLE)
        {
            current_node.node_art = {node_type::variable};
        }
        else if(ref->nodeClass == UA_NODECLASS_OBJECT)
        {
            current_node.node_art = {node_type::object};
        }
        else
        {
            current_node.node_art = {node_type::DEFAULT};
        }
        /* display_name */
        current_node.display_name = str;
        state = EXIT_SUCCESS;
    }
    else
    {
        // UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        //             "No object, method or variable.");
        cLOG(Level::ERROR, "No object, method or variable.");
    }
    return state;
}

void
Tree_Node::set_node_job(std::shared_ptr<JOB> &job)
{
    node_.namespace_index = job->namespace_index;
    node_.ident_type = job->type_id;
    node_.identifier_string = job->id_string;
    node_.identifier_int = job->id_numeric;
    return;
}

int
Tree_Node::get_namespace(void)
{
    return node_.namespace_index;
}

id_type
Tree_Node::get_ident_type(void)
{
    return node_.ident_type;
}

std::string 
Tree_Node::get_ident_string(void)
{
    return node_.identifier_string;
}

int
Tree_Node::get_ident_numeric(void)
{
    return node_.identifier_int;
}

std::string
Tree_Node::get_display_name(void)
{
    return node_.display_name;
}

std::string
Tree_Node::get_namespace_index(void)
{
    std::string type = {""};
    if(node_.namespace_index < 0)
    {
        type = "DEFAULT";
    }
    else
    {
        type = std::to_string(node_.namespace_index);
    }
    return type;
}

std::string
Tree_Node::get_identifier(void)
{
    std::string type = {""};
    if(node_.ident_type == id_type::string)
    {
        type = node_.identifier_string;
    }
    else if(node_.ident_type == id_type::numeric)
    {
        type = std::to_string(node_.identifier_int);
    }
    else if(node_.ident_type == id_type::DEFAULT)
    {
        type = "DEFAULT";
    }
    else
    {
        type = "DEFAULT";
    }
    return type;
}

std::string
Tree_Node::get_node_type(void)
{
    std::string type = {""};
    if(node_.node_art == node_type::method)
    {
        type = "method";
    }
    else if(node_.node_art == node_type::variable)
    {
        type = "variable";
    }
    else if(node_.node_art == node_type::object)
    {
        type = "object";
    }
    else if(node_.node_art == node_type::DEFAULT)
    {
        type = "DEFAULT";
    }
    else
    {
        type = "DEFAULT";
    }
    return type;
}

std::string
Tree_Node::get_identifier_type(void)
{
    if(node_.ident_type == id_type::string)
    {
        return "string";
    }
    else if(node_.ident_type == id_type::numeric)
    {
        return "numeric";
    }
    else if(node_.ident_type == id_type::DEFAULT)
    {
        return "DEFAULT";
    }
    else
    {
        return "DEFAULT";
    }
}

void
Tree_Node::print_node(void)
{
    std::cout   << FONT_GREEN
                << "[ " << get_identifier_type() << " ]"
                << "[ " << get_ident_string() << " ]"
                << "[ " << get_ident_numeric() << " ]"
                << "[ " << get_namespace() << " ]"
                << FONT_RESET
                << std::endl;
    return;
}

void
Tree_Node::print_tree_console(void)
{
    std::cout   << FONT_BLUE
                << "\n[Cab printer]     " 
                << CURSOR_COL(57)
                << "[IDENTIFIER] "
                << "[NS] "
                << "[NODE TYPE] "
                << FONT_RESET
                << std::endl;
    print_tree_console_();
    std::cout << std::endl;
    return;
}

void
Tree_Node::print_tree_console_(const std::string &prefix, bool is_last)
{
    std::cout << prefix;
    std::cout << (is_last ? "└── " : "├── ");
    // Print actual node 
    std::cout   << FONT_LIGHT_BLUE
                << get_display_name()  
                << FONT_LIGHT_PURPLE
                << CURSOR_COL(57) << "[" << get_identifier() << "]"
                << FONT_DARK_GRAY
                << " [" << get_namespace_index() << "]"
                << FONT_LIGHT_BLUE
                << " [" << get_node_type() << "]"
                << FONT_RESET
                << std::endl;
    // Iterate truth the child nodes
    for(int i = 0; i < children_.size(); i++)
    {
        children_[i]->print_tree_console_(prefix + (is_last ? "    " : "│   "), 
                i == children_.size() - 1);
    }
    return;
}

void
Tree_Node::print_tree_file(const std::string &filename)
{
    std::ofstream file(filename);
    if(!file.is_open())
    {
        // UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        //              "Error, create file browse_out.txt");
        cLOG(Level::ERROR, "Can not create file browse.txt");
    }
    else
    {
        file.close();
        /* Open to write file */
        std::ofstream out_file(filename, std::ios::app);
        if(!out_file.is_open())
        {
            // UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
            //              "Error, while open file output_browse.txt");
            cLOG(Level::ERROR, "Can not open file browse.txt");
        }
        else
        {
            out_file    << "[Cab printer] " 
                        << "[IDENTIFIER] "
                        << "[NS] "
                        << "[NODE TYPE] "
                        << std::endl;

            print_tree_file_(out_file);
        }
        out_file.close();
    }
    file.close();
    return;
}

void
Tree_Node::print_tree_file_(std::ofstream &file,
                        const std::string &prefix,
                        bool is_last)
{
    file    << prefix;
    file    << (is_last ? "└── " : "├── ");
    file    << get_display_name()  
            << "[" << get_identifier() << "]"
            << "[" << get_namespace_index() << "]"
            << "[" << get_node_type() << "]"
            << std::endl;
    for(int i = 0; i < children_.size(); i++)
    {
        children_[i]->print_tree_file_(file, prefix + (is_last ? "    " : "│   "), 
                i == children_.size() - 1);
    }
    return;
}

/* Eof */