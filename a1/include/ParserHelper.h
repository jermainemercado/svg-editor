#ifndef PARSERHELPER_H
#define PARSERHELPER_H

#include "SVGParser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*
* Implementation of XML parser is supplemented with code from example authored by Dodji Seketeli.
* Example code can be found at https://www.xmlsoft.org/examples/tree1.c
*/

static void parseTree(xmlNode* xml_node, SVGimage* current_img);
void* svgCat(char* stringToModify, char* nameOfElement, char* stringToAppend);
//static void parseTreeTitle(xmlNode* xml_node, SVGimage* current_img);
//static void parseTreeDesc(xmlNode* xml_node, SVGimage* current_img);

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
                    }
                    if (!(strcmp((char *)cur_node->name,"desc"))) {
                        strcpy(current_img->description, (char*)cur_node->children->content);
                    }
                }
                if (cur_node->content != NULL) {
                    printf("\nNode Name:\n\t%s\nNode Content:\n\t%s\n", cur_node->name, cur_node->content);
                }
                parseTree(cur_node->children, current_img);

            }
            
        }
    }
        
}

static void parseTreeTitle(xmlNode* xml_node, SVGimage* current_img) {
    
    xmlNode *cur_node = NULL;
    char *buffer = NULL;
    int bytesNeeded;

    cur_node = xml_node;
    if ((strcmp((char*)cur_node->name,"title"))) {
        if (cur_node->children != NULL) {
                if (cur_node->children->content != NULL) {
                printf("Content:\n\t%s", (char *)cur_node->children->content);
                bytesNeeded = snprintf(NULL, 0, (char *)cur_node->children->content);
                buffer = malloc(bytesNeeded+1);
                sprintf(buffer, (char *)cur_node->children->content);
                strcpy(current_img->title,buffer);
                free(buffer);
                }
        }
    }
}



/*static void parseTreeDesc(xmlNode* xml_node, SVGimage* current_img) {

    xmlNode *cur_node = NULL;
    char *buffer = NULL;
    int bytesNeeded;

    cur_node = xml_node;
    if ((strcmp((char*)cur_node->name,"desc"))) {
        if (cur_node->children != NULL) {
                if (cur_node->children->content != NULL) {
                printf("Content:\n\t%s", (char *)cur_node->children->content);
                bytesNeeded = snprintf(NULL, 0, (char *)cur_node->children->content);
                buffer = malloc(bytesNeeded+1);
                sprintf(buffer, (char *)cur_node->children->content);
                strcpy(current_img->title,buffer);
                free(buffer);
                }
        }
    }

}*/

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