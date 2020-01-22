#ifndef PARSERHELPER_H
#define PARSERHELPER_H

#include "SVGParser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libxml/xmlstring.h>

/*
* Implementation of XML parser is supplemented with code from example authored by Dodji Seketeli.
* Example code can be found at https://www.xmlsoft.org/examples/tree1.c
*/

static void parseTree(xmlNode * xml_node, SVGimage* current_img);
static void parseTreeTitle(xmlNode* xml_node, SVGimage* current_img);
static void parseTreeDesc(xmlNode* xml_node, SVGimage* current_img);

static void parseTree(xmlNode* xml_node, SVGimage* current_img)
{
    xmlNode *cur_node = NULL;

    for (cur_node = xml_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            parseTreeTitle(cur_node, current_img);
            parseTreeDesc(cur_node, current_img);
        }
    }
        parseTree(cur_node->children, current_img);
}

static void parseTreeTitle(xmlNode* xml_node, SVGimage* current_img) {
    
    xmlNode *cur_node = NULL;
    char *buffer = NULL;
    int bytesNeeded;

    cur_node = xml_node;
    if ((!xmlStrcmp(cur_node->name,(xmlChar*)"title"))) {
        if (cur_node->content != NULL ){
                printf("\nFOUND TITLE:\n\t%s\n", (char *)xmlNodeGetContent);
                bytesNeeded = snprintf(NULL, 0, (char *)xmlNodeGetContent(cur_node));
                buffer = malloc(bytesNeeded+1);
                sprintf(buffer, (char *)xmlNodeGetContent(cur_node));
                strcpy(current_img->title,buffer);
                free(buffer);
        }
    }
}



static void parseTreeDesc(xmlNode* xml_node, SVGimage* current_img) {

    xmlNode *cur_node = NULL;
    char *buffer = NULL;
    int bytesNeeded;

    cur_node = xml_node;
    if ((!xmlStrcmp(cur_node->name,(xmlChar*)"desc"))) {
        if (cur_node->content != NULL ){
                printf("\nFOUND TITLE:\n\t%s\n", (char *)xmlNodeGetContent);
                bytesNeeded = snprintf(NULL, 0, (char *)xmlNodeGetContent(cur_node));
                buffer = malloc(bytesNeeded+1);
                sprintf(buffer, (char *)xmlNodeGetContent(cur_node));
                strcpy(current_img->title,buffer);
                free(buffer);
        }
    }

}
#endif