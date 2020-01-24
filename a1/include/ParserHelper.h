#ifndef PARSERHELPER_H
#define PARSERHELPER_H

#include "SVGParser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>





static void parseTree(xmlNode* xml_node, SVGimage* current_img);
static void parseAttributes(xmlNode* xml_node, SVGimage* current_img);
void* svgCat(char* stringToModify, char* nameOfElement, char* stringToAppend);
static void attrInserter(xmlNode* xml_node, SVGimage* current_img);

/*
* Implementation of XML parser is supplemented with code from example authored by Dodji Seketeli.
* Example code can be found at https://www.xmlsoft.org/examples/tree1.c
*/
static void parseTree(xmlNode* xml_node, SVGimage* current_img)
{
    xmlNode *cur_node = NULL;
    for (cur_node = xml_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("\nElement Name:\n\t%s",cur_node->name);
            
            if (cur_node->children != NULL) {
                if (cur_node->children->content != NULL) {
                    if (!(strcmp((char *)cur_node->name,"title"))) {
                        strcpy(current_img->title, (char*)cur_node->children->content);
                    } else if (!(strcmp((char *)cur_node->name,"desc"))) {
                        strcpy(current_img->description, (char*)cur_node->children->content);
                    }
                }
            }
            parseAttributes(cur_node,current_img);
            parseTree(cur_node->children, current_img);
        }
    }
        
}

/*
* parseAttributes parses the current xml_node and adds every attribute that is a child of that node
* to its associated List* in the SVGimage*
* Implementation of XML parser is supplemented with code from example authored by Dodji Seketeli.
* Example code can be found at https://www.xmlsoft.org/examples/tree1.c
*/
static void parseAttributes(xmlNode* xml_node, SVGimage* current_img) {
    xmlAttr* attr;
    xmlNode* cur_node;
    cur_node = xml_node;
    Attribute* cur_attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            
            if(!(strcmp((char *)attr->parent->name,"svg"))) {
                cur_attr = malloc(sizeof(Attribute));
                cur_attr->name = NULL;
                cur_attr->value = NULL;
                cur_attr->name = malloc(strlen(attrName) + 1);
                cur_attr->value = malloc(strlen(cont) + 1);
                strcpy(cur_attr->name, attrName);
                strcpy(cur_attr->value,cont);
                insertBack(current_img->otherAttributes,cur_attr);
                printf("\nInserted Attr: %s\n",attrName);
                free(cur_attr->name);
                free(cur_attr->value);
                free(cur_attr);
            }
        }
}

/* Safely appends formatted strings to pointer passed to function. */
void* svgCat(char* stringToModify, char* nameOfElement, char* stringToAppend) {

    char *backupBuffer = NULL;
    int bytesNeeded = strlen(stringToModify + 1);

    bytesNeeded += snprintf(NULL,0,"\n%s:\n\t%s\n", nameOfElement, stringToAppend);
    backupBuffer = malloc(bytesNeeded + 1);
    strcpy(backupBuffer, stringToModify);
    stringToModify = realloc(stringToModify, bytesNeeded + 10); //10 extra bytes to be safe.
    sprintf(backupBuffer, "\n%s:\n\t%s\n", nameOfElement, stringToAppend);
    strcat(stringToModify,backupBuffer);

    if (backupBuffer != NULL) {
        free(backupBuffer);
    }
    return stringToModify;
}
#endif