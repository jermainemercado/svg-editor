#include "SVGParser.h"


SVGimage* createSVGimage(char* fileName) {

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *cur_node = NULL;
    SVGimage *newImage;

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        printf("Error: Could not parse file %s\n", fileName);
    }

    root_element = xmlDocGetRootElement(doc);

    //Populate namespace.
    strcpy(newImage->namespace,root_element->ns->href);

    //
    for (cur_node = root_element; cur_node != NULL; cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            if(strcmp())
        }
    }


    return newImage;
}