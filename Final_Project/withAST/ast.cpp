#include "ast.h"
#define maplength 3

/*
checker is for the error detection.
*/

// index: 0 is for the global var. map
// index: 1 is for the named fun. map        (can be found by fun. name)
// index: 2 is for the named fun.'s var. map (can be found by fun. name)
std::map<std::string, ASTnode *> var_map[maplength];
ASTnode *id_nodes = NULL;

bool insertmap(int index, std::string insertname, ASTnode *insertnode)
{
    if (index >= maplength || index < 0)
        return false;
    var_map[index].insert(std::pair<std::string, ASTnode *>(insertname, duplicatenodes(insertnode)));
    return true;
}

bool findmap(int index, std::string findname, ASTnode **findnode)
{
    if (index >= maplength || index < 0)
        return false;
    std::map<std::string, ASTnode *>::iterator mapiter;
    mapiter = var_map[index].find(findname);
    if (mapiter != var_map[index].end())
    {
        *findnode = duplicatenodes(mapiter->second);
        return true;
    }
    /*
    checker
    */
    printf("Find error!, check find str or find field.\n");
    return false;
}

ASTnode *manipulatenodes(ASTnode *idnodes, ASTnode *paramnodes)
{
    /* function for pairing ids' name and params */
    if (idnodes == NULL)
    {
        return NULL;
    }
    if ((idnodes->left != NULL) && (paramnodes->left != NULL))
    {
        idnodes->left = manipulatenodes(idnodes->left, paramnodes->left);
    }
    if ((idnodes->right != NULL) && (paramnodes->right != NULL))
    {
        idnodes->right = manipulatenodes(idnodes->right, paramnodes->right);
    }
    if (idnodes->node_type == ast_id)
    {
        printf("manipulatenodes name:%s\n", ((ASTnode_id *)idnodes)->name.c_str());
        ((ASTnode_id *)idnodes)->target = paramnodes;
    }
    return idnodes;
}

ASTnode *duplicatenodes(ASTnode *rootnode)
{
    if (rootnode == NULL)
    {
        return NULL;
    }
    ASTnode *newnode = NULL;
    if (rootnode->node_type == ast_bool)
    {
        newnode = mallocnode(rootnode->node_type, ((ASTnode_bool *)rootnode)->value);
    }
    else if (rootnode->node_type == ast_num)
    {
        newnode = mallocnode(rootnode->node_type, ((ASTnode_num *)rootnode)->value);
    }
    else if (rootnode->node_type == ast_id)
    {
        newnode = mallocnode(rootnode->node_type, ((ASTnode_id *)rootnode)->name);
        ((ASTnode_id *)newnode)->target = duplicatenodes(((ASTnode_id *)rootnode)->target);
    }
    else if (rootnode->node_type == ast_if)
    {
        newnode = (ASTnode *)malloc(sizeof(ASTnode_if));
        newnode->node_type = ast_if;
        ((ASTnode_if *)newnode)->statement = duplicatenodes(((ASTnode_if *)rootnode)->statement);
    }
    else
    {
        newnode = (ASTnode *)malloc(sizeof(ASTnode));
        newnode->node_type = rootnode->node_type;
    }
    newnode->left = duplicatenodes(rootnode->left);
    newnode->right = duplicatenodes(rootnode->right);
    /*
    checker
    */
    if (newnode == NULL)
    {
        printf("Duplicate nodes error!\n");
    }
    return newnode;
}

ASTnode *mallocaddnode(ASTtype newtype, ASTnode *newleft, ASTnode *newright)
{
    ASTnode *newnode;
    newnode = (ASTnode *)malloc(sizeof(ASTnode));
    newnode->node_type = newtype;
    newnode->left = newleft;
    newnode->right = newright;
    return (ASTnode *)newnode;
}

ASTnode *mallocaddnode(ASTtype newtype, ASTnode *newleft, ASTnode *newright, ASTnode *newstatement)
{
    ASTnode_if *newnode = (ASTnode_if *)malloc(sizeof(ASTnode_if));
    newnode->node_type = newtype;
    newnode->left = newleft;
    newnode->right = newright;
    newnode->statement = newstatement;
    return (ASTnode *)newnode;
}

ASTnode *mallocnode(ASTtype newtype, bool newvalue)
{
    if (newtype == ast_bool)
    {
        ASTnode_bool *newnode = (ASTnode_bool *)malloc(sizeof(ASTnode_bool));
        newnode->node_type = newtype;
        newnode->value = newvalue;
        return (ASTnode *)newnode;
    }
    return NULL;
}

ASTnode *mallocnode(ASTtype newtype, int newvalue)
{
    if (newtype == ast_num)
    {
        ASTnode_num *newnode = (ASTnode_num *)malloc(sizeof(ASTnode_num));
        newnode->node_type = newtype;
        newnode->value = newvalue;
        return (ASTnode *)newnode;
    }
    return NULL;
}

ASTnode *mallocnode(ASTtype newtype, std::string newname)
{
    if (newtype == ast_id)
    {
        ASTnode_id *newnode = (ASTnode_id *)malloc(sizeof(ASTnode_id));
        newnode->node_type = newtype;
        /*
        Two nodes point to same name, but if one node has been del.
        , another node may contain possible error!
        */
        newnode->name = newname;
        return (ASTnode *)newnode;
    }
    return NULL;
}

void freenodes(ASTnode *nownode, int freenum)
{
    /*
    just for testing
    */
    return;
    if (freenum >= 4)
    {
        free(((ASTnode_if *)nownode)->statement);
    }
    if (freenum >= 3)
    {
        free(nownode->right);
    }
    if (freenum >= 2)
    {
        free(nownode->left);
    }
    if (freenum >= 1)
    {
        free(nownode);
    }
    return;
}

bool checknode(ASTnode *nownode)
{
    if (nownode != NULL)
    {
        return true;
    }
    /*
    checker
    */
    printf("AST error!\n");
    return false; /* NULL */
}

bool returnnode(ASTnode *nownode, ASTtype prevtype, std::string *returnstring)
{
    if (nownode == NULL)
    {
        return false;
    }
    if (nownode->node_type == prevtype)
    {
        if (prevtype == ast_id)
        {
            *returnstring = ((ASTnode_id *)nownode)->name;
            return true;
        }
    }
    /*
    checker
    */
    printf("Type error!\n");
    /*
    just for testing
    */
    printf("%d, %d.\n", nownode->node_type, prevtype);
    return false;
}

ASTnode *targetnode = NULL;
bool returntargetnode(ASTnode *nownode, std::string idname)
{
    /*
    this function has a big time complexity (BIG-O)
    */
    // the input maybe need to change too may global var.
    bool findcheck = false;
    if (nownode == NULL)
    {
        return false;
    }
    std::string checkstr;
    printf("returntargetnode:%d\n", nownode->node_type);
    if (nownode->node_type == ast_id)
    {
        printf("GOODxxxx:%s\n", ((ASTnode_id *)nownode)->name.c_str());
    }
    returnnode(nownode, ast_id, &checkstr);
    printf("GOODyyyy:%s\n", idname.c_str());
    printf("GOODyyyy:%s\n", checkstr.c_str());
    if (idname == checkstr && ((ASTnode_id *)nownode)->target != NULL)
    {
        findcheck = true;
        targetnode = ((ASTnode_id *)nownode)->target;
        printf("TARGET:%s\n", checkstr.c_str());
        printf("TESTBOOL:%d\n", ((ASTnode_id *)nownode)->target != NULL);
    }
    return (findcheck || returntargetnode(nownode->left, idname) || returntargetnode(nownode->right, idname));
}

bool returnnode(ASTnode *nownode, ASTtype prevtype, bool *returnbool, int *returnnum)
{
    if (nownode == NULL)
    {
        return false;
    }
    if (nownode->node_type == prevtype)
    {
        if (prevtype == ast_bool)
        {
            *returnbool = ((ASTnode_bool *)nownode)->value;
            return true;
        }
        else if (prevtype == ast_num)
        {
            *returnnum = ((ASTnode_num *)nownode)->value;
            return true;
        }
    }
    else if (nownode->node_type == ast_id) // define var. or var. inside fun.
    {
        std::string tmpstr;
        ASTnode *tmpnode;
        returnnode(nownode, ast_id, &tmpstr);
        if (id_nodes != NULL) // inside fun.
        {
            targetnode = NULL;
            printf("GOOD111111111:%s\n", tmpstr.c_str());
            printf("%d\n", returntargetnode(id_nodes, tmpstr));
            printf("%d\n", targetnode != NULL);
            /* The local var.s and its' value in the function, which is been calling recursively, can be found in idnodes. */
            if (targetnode != NULL)
            {
                printf("GOOD2222222222:%s\n", tmpstr.c_str());
                // return will store at global var. targetnode
                if (targetnode->node_type == prevtype)
                {
                    if (prevtype == ast_bool)
                    {
                        *returnbool = ((ASTnode_bool *)targetnode)->value;
                        printf("HI:%s, %d.\n", tmpstr.c_str(), *returnbool);
                        targetnode = NULL;
                        return true;
                    }
                    else if (prevtype == ast_num)
                    {
                        *returnnum = ((ASTnode_num *)targetnode)->value;
                        printf("HI:%s, %d.\n", tmpstr.c_str(), *returnnum);
                        targetnode = NULL;
                        return true;
                    }
                }
            }
            return NULL;
        }
        else if (findmap(0, tmpstr, &tmpnode)) // inside global
        {
            if (tmpnode->node_type == prevtype)
            {
                if (prevtype == ast_bool)
                {
                    *returnbool = ((ASTnode_bool *)tmpnode)->value;
                    return true;
                }
                else if (prevtype == ast_num)
                {
                    *returnnum = ((ASTnode_num *)tmpnode)->value;
                    return true;
                }
            }
        }
        /*
        just for testing
        */
        // printf("%s, %d.\n", tmpstr.c_str(), tmpnode->node_type);
    }
    /*
    checker
    */
    printf("Type error!\n");
    /*
    just for testing
    */
    // printf("%d, %d.\n", nownode->node_type, prevtype);
    return false;
}

// ASTnode *ASTfunction(ASTnode *expnodes, ASTnode *idnodes, ASTnode *paramnodes)
// {
//     ASTnode *new_expnodes = duplicatenodes(expnodes);
//     ASTprocess(new_expnodes, ast_root);
//     return NULL;
// }

ASTnode *ASTprocess(ASTnode *rootnode, ASTtype prevtype)
{
    // check node
    if (rootnode == NULL)
    {
        return NULL;
    }
    /*
    pre setting before travel left/right node
    */
    ASTnode *tmpnode;
    switch (rootnode->node_type)
    {
    case ast_continue:
        rootnode->node_type = prevtype;
        break;
    case ast_fun:
        /*
        ast_fun_continue left tree point to ids, right tree point to exp
        */
        /*
        ids maybe NULL, therefore do not need to check.
        */
        // if (!checknode(rootnode->left))
        //     break;
        if (!checknode(rootnode->right))
            break;
        tmpnode = mallocaddnode(ast_fun_continue, duplicatenodes(rootnode->left), duplicatenodes(rootnode->right));
        freenodes(rootnode, 3);
        return tmpnode;
        break;
    default:
        break;
    }
    // travel left node
    rootnode->left = ASTprocess(rootnode->left, rootnode->node_type);
    // travel right node
    rootnode->right = ASTprocess(rootnode->right, rootnode->node_type);
    // var for self node
    int leftnum, rightnum, printnum;
    bool leftbool, rightbool, ifbool, printbool;
    std::string tmpstring;
    ASTnode *expnodes;

    /*
    just for testing
    */
    printASTtype(rootnode->node_type);
    // travel self node
    switch (rootnode->node_type)
    {
    case ast_continue:
        /*
        should never walk here
        */
        break;
    case ast_root:
        break;
    case ast_print_bool:
        if (!checknode(rootnode->left))
            break;
        if (returnnode(rootnode->left, ast_bool, &printbool, NULL))
        {
            if (printbool)
            {
                printf("#t\n");
            }
            else
            {
                printf("#f\n");
            }
        }
        break;
    case ast_print_num:
        if (!checknode(rootnode->left))
            break;
        if (returnnode(rootnode->left, ast_num, NULL, &printnum))
        {
            printf("%d\n", printnum);
        }
        break;
    case ast_bool:
        break;
    case ast_num:
        break;
    case ast_id:
        break;
    case ast_plus:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_num, leftnum + rightnum);
        break;
    case ast_minus:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_num, leftnum - rightnum);
        break;
    case ast_multiply:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_num, leftnum * rightnum);
        break;
    case ast_divide:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_num, leftnum / rightnum);
        break;
    case ast_greater:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_bool, (leftnum > rightnum) ? true : false);
        break;
    case ast_smaller:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_bool, (leftnum < rightnum) ? true : false);
        break;
    case ast_equal:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, (rootnode->right)->node_type, &rightbool, &rightnum);
        if ((((rootnode->right)->node_type == ast_bool) && (!rightbool)) || (leftnum != rightnum))
        {
            freenodes(rootnode, 3);
            rootnode = mallocnode(ast_bool, false);
        }
        else
        {
            freenodes(rootnode, 3);
            rootnode = mallocnode(ast_num, leftnum);
        }
        break;
    case ast_mod:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_num, NULL, &leftnum);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_num, NULL, &rightnum);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_num, leftnum % rightnum);
        break;
    case ast_and:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_bool, &leftbool, NULL);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_bool, &rightbool, NULL);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_bool, leftbool && rightbool);
        break;
    case ast_or:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_bool, &leftbool, NULL);
        if (!checknode(rootnode->right))
            break;
        returnnode(rootnode->right, ast_bool, &rightbool, NULL);
        freenodes(rootnode, 3);
        rootnode = mallocnode(ast_bool, leftbool || rightbool);
        break;
    case ast_not:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_bool, &leftbool, NULL);
        freenodes(rootnode, 2);
        rootnode = mallocnode(ast_bool, !leftbool);
        break;
    case ast_fun:
        /*
        should never walk here, see above/first switch
        */
        break;
    case ast_fun_continue:
        /*
        should never walk here, see above/first switch
        */
        break;
    case ast_fun_call:
        /* 0th warning & notification */
        /* left is either fun_exp or fun_name, right is params. */
        /* Every operations here need to do under the dul. nodes, the original nodes cannot be poison. */
        if (!checknode(rootnode->left))
            break;
        /* first check ast_fun_continue (cause preprocess therefore will not be ast_fun) */
        if (rootnode->left->node_type == ast_fun_continue)
        {
            /* 1st fun_exp: anonymous function */
            // left tree will point to ast_fun_continue
            // whos left is ids and right is exp
            if (!checknode(rootnode->left->left))
                break;
            id_nodes = rootnode->left->left;
            if (!checknode(rootnode->left->right))
                break;
            expnodes = rootnode->left->right;
        }
        else /* rootnode->left->node_type should be ast_id */
        {
            /* 2nd fun_name: named function */
            // take out ids and exp by name, then do things as 1st will be done.
            // use name find exp in index 1
            // use name find ids in index 2
            returnnode(rootnode->left, ast_id, &tmpstring);
            printf("CALLING NAMED FUN:%s\n", tmpstring.c_str());
            if (!findmap(1, tmpstring, &expnodes) || !findmap(2, tmpstring, &id_nodes))
            {
                /*
                should never walk here, checker
                */
                printf("Find error!\n");
            }
        }
        /*
        ids maybe NULL, therefore do not need to check.
        */
        // if (!checknode(rootnode->right))
        //     break;
        id_nodes = manipulatenodes(id_nodes, rootnode->right);
        /*
        only call ASTprocess should able reduce the exp by given global id_nodes (contain id-param pairs)
        and which need to be clear after the calling procedure
        */
        rootnode = ASTprocess(duplicatenodes(expnodes), rootnode->node_type);
        // rootnode = ASTprocess(duplicatenodes(expnodes), ast_fun_continue);
        id_nodes = NULL;
        break;
    case ast_params:
        break;
    case ast_ids:
        /*
        should never walk here, see above/first switch
        */
        break;
    case ast_define:
        if (!checknode(rootnode->left))
            break;
        returnnode(rootnode->left, ast_id, &tmpstring);
        if (!checknode(rootnode->right))
            break;
        /* first check ast_fun_continue (cause preprocess therefore will not be ast_fun) */
        if (rootnode->right->node_type == ast_fun_continue)
        {
            printf("DEFINE NAMED FUN:%s\n", tmpstring.c_str());
            printf("HIHIHIIHH------------\n");
            printASTtype(rootnode->left->node_type);
            printASTtype(rootnode->right->node_type);
            printf("HIHIHIIHH------------\n");
            insertmap(1, tmpstring, rootnode->right->right); // index 1: right, exp.
            insertmap(2, tmpstring, rootnode->right->left);  // index 2: left, ids.

            // printf("DEFINE NAMED FUN:%s\n", tmpstring.c_str());
            // printf("HIHIHIIHH------------\n");
            // printASTtype(rootnode->left->node_type);
            // printASTtype(rootnode->right->node_type);
            // printASTtype(rootnode->right->right->node_type);
            // printASTtype(rootnode->right->left->node_type);
            // printf("HIHIHIIHH------------\n");
            // insertmap(1, tmpstring, rootnode->right); // index 1: right, exp.
            // insertmap(2, tmpstring, rootnode->left);  // index 2: left, ids.
        }
        else
        {
            insertmap(0, tmpstring, rootnode->right);
        }
        break;
    case ast_if:
        if (!checknode(((ASTnode_if *)rootnode)->statement))
            break;
        ((ASTnode_if *)rootnode)->statement = ASTprocess(((ASTnode_if *)rootnode)->statement, rootnode->node_type);
        returnnode(((ASTnode_if *)rootnode)->statement, ast_bool, &ifbool, NULL);
        if (!checknode(rootnode->left))
            break;
        if (!checknode(rootnode->right))
            break;
        if (ifbool)
        {
            ASTnode *newnode = duplicatenodes(rootnode->left);
            rootnode = newnode;
            freenodes(rootnode, 4);
        }
        else
        {
            ASTnode *newnode = duplicatenodes(rootnode->right);
            rootnode = newnode;
            freenodes(rootnode, 4);
        }
        break;
    default:
        /*
        should never walk here
        */
        break;
    }
    return rootnode;
}

void printASTtype(ASTtype nowtype)
{
    switch (nowtype)
    {
    case ast_continue:
        /*
        should never walk here
        */
        printf("ast_continue\n");
        break;
    case ast_root:
        printf("ast_root\n");
        break;
    case ast_print_bool:
        printf("ast_print_bool\n");
        break;
    case ast_print_num:
        printf("ast_print_num\n");
        break;
    case ast_bool:
        printf("ast_bool\n");
        break;
    case ast_num:
        printf("ast_num\n");
        break;
    case ast_id:
        printf("ast_id\n");
        break;
    case ast_plus:
        printf("ast_plus\n");
        break;
    case ast_minus:
        printf("ast_minus\n");
        break;
    case ast_multiply:
        printf("ast_multiply\n");
        break;
    case ast_divide:
        printf("ast_divide\n");
        break;
    case ast_greater:
        printf("ast_greater\n");
        break;
    case ast_smaller:
        printf("ast_smaller\n");
        break;
    case ast_equal:
        printf("ast_equal\n");
        break;
    case ast_mod:
        printf("ast_mod\n");
        break;
    case ast_and:
        printf("ast_and\n");
        break;
    case ast_or:
        printf("ast_or\n");
        break;
    case ast_not:
        printf("ast_not\n");
        break;
    case ast_fun:
        printf("ast_fun\n");
        break;
    case ast_fun_continue:
        printf("ast_fun_continue\n");
        break;
    case ast_fun_call:
        printf("ast_fun_call\n");
    case ast_params:
        printf("ast_params\n");
        break;
    case ast_ids:
        printf("ast_ids\n");
        break;
    case ast_define:
        printf("ast_define\n");
        break;
    case ast_if:
        printf("ast_if\n");
        break;
    default:
        /*
        should never walk here
        */
        printf("No match\n");
        break;
    }
    return;
}
