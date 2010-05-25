/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include "xml.h"
#include <expat.h>

typedef struct xml_attr
{
    struct xml_attr *next;
    char *data;
    char name[1]; /* should be last element in struct */
} xml_attr_t;

typedef struct xml_elm
{
    char *name;
    char *cdata;
    xml_attr_t *attr;

    struct xml_elm* next;
    struct xml_elm* child;
    struct xml_elm* parent;
} xml_elm_t;

static XML_Parser  parser;
static xml_elm_t  *root;
static xml_elm_t  *curr;
static jbool_t      curr_end;
static jbool_t      error;

static void xml_start_element(void *ctx, const char *name, const char **atts);
static void xml_end_element(void *ctx, const char *name);
static void xml_cdata(void *ctx, const XML_Char *s, int len);
    
jresult_t xml_init(void)
{
    DBG_I(DUTIL_XML, ("XML: xml_init\n"));

    parser = XML_ParserCreate(NULL);
    if (!parser)
    {
        DBG_E(DUTIL_XML, ("XML: XML_ParserCreate failed\n"));
        return JEINVAL;
    }

    return 0;
}

void xml_uninit(void)
{
    DBG_I(DUTIL_XML, ("XML: xml_uninit\n"));
    
    XML_ParserFree(parser);
}

xml_elm_h xml_parse(char *buff, juint32_t size)
{
    enum XML_Status rc;

    DBG_V(DUTIL_XML, ("XML: xml_parse, buff=%p size=%ld\n", buff, size));
    
    root     = NULL;
    curr     = NULL;
    curr_end = 0;
    error    = 0;

    XML_SetElementHandler(parser, xml_start_element, xml_end_element);
    XML_SetCharacterDataHandler(parser, xml_cdata);
    
    rc = XML_Parse(parser, buff, size, 1);
    if (rc != XML_STATUS_OK) 
    {
        error = 1;
        DBG_E(DUTIL_XML, ("XML: xml_parse failed: '%s' at line %lu\n",
            XML_ErrorString(XML_GetErrorCode(parser)),
            XML_GetCurrentLineNumber(parser)));
    }
    if (error)
    {
        xml_free(root);
        root = NULL;
    }
    
    XML_ParserReset(parser, NULL);

    return root;
}

static void attr_free(xml_attr_t *attr)
{
    xml_attr_t *next;

    for (; attr; attr = next)
    {
        next = attr->next;
        
        if (attr->data)
            jfree(attr->data);
        
        jfree(attr);
    }
}

static jresult_t attr_alloc_copy(char **attr, xml_attr_t **head)
{
    xml_attr_t **new = head;

    *head = NULL;

    for (;attr[0] && attr[1]; attr+=2, new = &(*new)->next)
    {
        *new = jmalloc(sizeof(xml_attr_t) + j_strlen(attr[0]), 0);
        if (!*new)
            goto Error;

        (*new)->next = NULL;
        
        (*new)->data = jmalloc(j_strlen(attr[1]) + 1, 0);
        if (!(*new)->data)
            goto Error;
        
        j_strlcpy((*new)->name, attr[0], j_strlen(attr[0]) + 1);
        j_strlcpy((*new)->data, attr[1], j_strlen(attr[1]) + 1);
    }

    return 0;    

Error:
    attr_free(*head);
    return JENOMEM;
}

void xml_free(xml_elm_h elm)
{
    if (!elm)
        return;
    
    xml_free(elm->child);
    xml_free(elm->next);

    DBG_V(DUTIL_XML, ("XML: xml_free, elm=%p\n", elm));

    if (elm->name)
        jfree(elm->name);

    if (elm->cdata)
        jfree(elm->cdata);
    
    if (elm->attr)
        attr_free(elm->attr);
    
    jfree(elm);
}

static void xml_start_element(void *ctx, const char *name, const char **atts)
{
    xml_elm_t *elm;
    jresult_t rc;

    DBG_V(DUTIL_XML, ("XML: xml_start_element '%s'\n", name));

    if (error)
        return;

    elm = (xml_elm_t *)jmalloc(sizeof(xml_elm_t), M_ZERO);
    if (!elm)
        goto Error;

    elm->name = (char *)jmalloc(j_strlen(name) + 1, 0);
    if (!elm->name)
        goto Error;

    j_strlcpy(elm->name, name, j_strlen(name) + 1);

    rc = attr_alloc_copy((char **)atts, &elm->attr);
    if (rc)
        goto Error;

    if (!root)
        root = elm;
    else
    {
        if (curr_end)
        {/* elm at the same level */
            curr->next = elm;
            elm->parent = curr->parent;
        }
        else
        {/* child elm */
            curr->child = elm;
            elm->parent = curr;
        }
    }
    
    curr_end = 0;
    curr = elm;

    return;

Error:
    if (elm && elm->name)
        jfree(elm->name);
    
    if (elm && elm->attr)
        attr_free(elm->attr);
    
    if (elm)
        jfree(elm);

    DBG_E(DUTIL_XML, ("XML: xml_start_element fail, no memory\n"));
    error = 1;
    XML_StopParser(parser, 0);
}

static void xml_end_element(void *ctx, const char *name)
{
    DBG_V(DUTIL_XML, ("XML: xml_end_element, '%s'\n", name));

    if (error)
        return;

    if (curr_end)
        curr = curr->parent;

    curr_end = 1;

    if (!curr)
    {
        DBG_E(DUTIL_XML, ("XML: xml_end_element fail, current element "
            "is NULL\n"));
        error = 1;
        XML_StopParser(parser, 0);
    }
}

static char* concat_alloc_new(char *instr, const char *addstr,
    juint32_t addstr_size)
{
    char *newstr = NULL;
    juint32_t instr_size = 0;

    if (!addstr || !addstr_size)
        return NULL;

    if (instr)
        instr_size = j_strlen(instr);

    newstr = (char*)jmalloc(instr_size+addstr_size+1, 0);
    if (!newstr)
        return NULL;

    if (instr)
    {
        j_memcpy(newstr, instr, instr_size);
        jfree(instr);
    }

    j_memcpy(newstr+instr_size, addstr, addstr_size);
    newstr[instr_size+addstr_size] = 0;
    return newstr;
}

static void xml_cdata(void *ctx, const XML_Char *s, int len)
{
    xml_elm_t *elm = curr;
    char *new_cdata;
    
    if (error)
        return;

    /* handle cases when text comes after a child */
    if (curr_end)
        elm = elm->parent;

    DBG_V(DUTIL_XML, ("XML: xml_cdata: elm=%p, name='%s'\n", elm,
        elm->name));

    new_cdata = (char *)concat_alloc_new(elm->cdata, s, len);
    if (!new_cdata)
    {
        if (elm->cdata)
            jfree(elm->cdata);
        DBG_E(DUTIL_XML, ("XML: xml_cdata fail, no memory\n"));
        error = 1;
        XML_StopParser(parser, 0);
        return;
    }

    elm->cdata = new_cdata;
}

#ifdef JDEBUG
static void dump_attrs(xml_attr_t *attr)
{
    xml_attr_t **a;

    for (a = &attr; *a; a = &(*a)->next)
        jprintf(" %s=%s", (*a)->name, (*a)->data);
}

static void dump_element(xml_elm_t *e, juint32_t depth)
{
    juint32_t i = depth;

    if (!e)
        return;
    
    while (i--)
        jprintf("  ");
    
    jprintf("<%s", e->name);
    dump_attrs(e->attr);
    
    if (e->cdata)
        jprintf("> = '%s'\n", e->cdata);
    else
        jprintf(">\n");

    dump_element(e->child, depth+1);
    dump_element(e->next, depth);
}

void xml_dump(xml_elm_h root_elm)
{
    jprintf("xml_dump root=%p\n", root_elm);
    dump_element((xml_elm_t *)root_elm, 0);
}
#endif

xml_elm_h xml_get_child_first(xml_elm_h parent_elm)
{
    return parent_elm->child;
}

xml_elm_h xml_get_parent(xml_elm_h elm)
{
    return elm->parent;
}

xml_elm_h xml_get_next(xml_elm_h elm)
{
    return elm->next;
}

char *xml_get_name(xml_elm_h elm)
{
    return elm->name;
}

char *xml_get_cdata(xml_elm_h elm)
{
    return elm->cdata;
}

char *xml_get_attr_data(xml_elm_h elm, char *attr_name)
{
    xml_attr_t **a;

    for (a = &elm->attr; *a; a = &(*a)->next)
    {
        if (!j_strcmp((*a)->name, attr_name))
            return (*a)->data;
    }
    return NULL;
}

