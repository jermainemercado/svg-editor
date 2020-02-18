#include "SVGParser.h"
#include "ParserHelper.h"
#include <ctype.h>
#include <math.h>
#include <unistd.h>

#define M_PI 3.14159265358979323846

SVGimage* createSVGimage(char* fileName) {
    
    if (fileName == NULL) {
        return NULL;
    }
    //Initial set up for SVGimage.
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *newImage = malloc(sizeof (SVGimage));
    newImage->otherAttributes = initializeList(&attributeToString, &deleteAttribute,&compareAttributes);
    newImage->rectangles = initializeList(&rectangleToString, &deleteRectangle,&compareRectangles);
    newImage->circles = initializeList(&circleToString, &deleteCircle,&compareCircles);
    newImage->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    newImage->groups = initializeList(&groupToString,&deleteGroup,&compareGroups);
    newImage->title[0] = '\0';
    newImage->namespace[0] = '\0';
    newImage->description[0] = '\0';
    

    if( access(fileName, F_OK ) != -1 ) {
        doc = xmlReadFile(fileName, NULL, 0);
    } else {
        return NULL;
    }

    if (doc == NULL) {
        printf("\nError: Could not parse file: %s\n", fileName);
        xmlCleanupParser();
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);


    //Populate namespace.
    if (root_element->ns->href == NULL) {
        printf("\nInvalid SVG file.\n");
        return NULL;
    }
    strncpy(newImage->namespace,(char *)root_element->ns->href,255);
    
    //Populate title, and description if there is any associated to the XML file.
    parseTree(root_element, newImage);
    //printf("\nAttribute Test:%s\n", newImage->otherAttributes->);
    
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return newImage;
}

void deleteSVGimage(SVGimage* img) {

    if (img == NULL) {
        return;
    }

    freeList(img->paths);
    freeList(img->circles);
    freeList(img->rectangles);
    freeList(img->groups);
    freeList(img->otherAttributes);
    //free(img->otherAttributes);
    free(img);
}

char* SVGimageToString(SVGimage* img) {

    ListIterator otherIter = createIterator(img->otherAttributes);
    ListIterator rectangleIter = createIterator(img->rectangles);
    ListIterator circleIter = createIterator(img->circles);
    ListIterator pathIter = createIterator(img->paths);
    ListIterator groupIter = createIterator(img->groups);
    char *returnedBuffer = NULL;
    int bytesNeeded = 1;
    SVGimage* cur_img = NULL;
    void *elem;
    char *backupBuffer = NULL;
    if (img != NULL) 
        cur_img = img;

    //Populating SVGimage* namespace.
    bytesNeeded = snprintf(NULL,0,"\n\nNamespace:\n\t%s\n",cur_img->namespace);
    returnedBuffer = malloc(bytesNeeded + 1);
    sprintf(returnedBuffer, "\n\nNamespace:\n\t%s\n", cur_img->namespace);
    
    if (img->title[0] != '\0') {
        returnedBuffer = svgCat(returnedBuffer,"Title",cur_img->title);
    }
    if (img->description[0] != '\0') {
        returnedBuffer = svgCat(returnedBuffer,"Description",cur_img->description);
    }
    while ((elem = nextElement(&otherIter)) != NULL) {
        char *elementString = NULL;
        elementString = attributeToString((Attribute *) elem);
        bytesNeeded += snprintf(NULL,0,"%s", elementString) + strlen(returnedBuffer) + 1;
        backupBuffer = realloc(backupBuffer, bytesNeeded);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded);
        sprintf(backupBuffer, "%s", elementString);
        strcat(returnedBuffer, backupBuffer);
        free(elementString);
    }
    while ((elem = nextElement(&rectangleIter)) != NULL) {
        char *elementString = NULL;
        elementString = rectangleToString((Rectangle*) elem);
        bytesNeeded += snprintf(NULL,0,"%s", elementString) + strlen(returnedBuffer) + 1;
        backupBuffer = realloc(backupBuffer, bytesNeeded);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded);
        sprintf(backupBuffer, "%s", elementString);
        strcat(returnedBuffer, backupBuffer);
        free(elementString);
    }
    while ((elem = nextElement(&circleIter)) != NULL) {
        char *elementString = NULL;
        elementString = circleToString((Circle*) elem);
        bytesNeeded += snprintf(NULL,0,"%s", elementString) + strlen(returnedBuffer) + 1;
        backupBuffer = realloc(backupBuffer, bytesNeeded); 
        returnedBuffer = realloc(returnedBuffer, bytesNeeded);
        sprintf(backupBuffer, "%s", elementString);
        strcat(returnedBuffer, backupBuffer);
        free(elementString);
    }
    while ((elem = nextElement(&pathIter)) != NULL) {
        char *elementString = NULL;
        elementString = pathToString((Path*) elem);
        bytesNeeded += snprintf(NULL,0,"%s",elementString) + strlen(returnedBuffer) + 1;
        backupBuffer = realloc(backupBuffer, bytesNeeded);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded);
        sprintf(backupBuffer, "%s", elementString);
        strcat(returnedBuffer, backupBuffer);
        free(elementString);
    }
    while ((elem = nextElement(&groupIter)) != NULL) {
        char *elementString = NULL;
        elementString = groupToString((Group*) elem);
        bytesNeeded += snprintf(NULL,0,"%s",elementString) + strlen(returnedBuffer) + 1;
        backupBuffer = realloc(backupBuffer, bytesNeeded);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded);
        sprintf(backupBuffer, "%s", elementString);
        strcat(returnedBuffer, backupBuffer);
        free(elementString);
    }
    if (backupBuffer != NULL) {
        free(backupBuffer);
    }

    return returnedBuffer;

}

void deleteAttribute( void* data) {

    Attribute* tempAttribute;

    if (data == NULL) {
        return;
    }
    
    tempAttribute = (Attribute*) data;

    free(tempAttribute->name);
    free(tempAttribute->value);
    free(tempAttribute);

    return;

}

char* attributeToString(void* data) {

    int bytesNeeded = 0;
    char* buffer = NULL;
    Attribute* tempAttribute;

    if (data == NULL) {
        return NULL;
    }

    tempAttribute = (Attribute *)data;

    bytesNeeded = snprintf(NULL,0,"\nAttribute Name:\n\t%s\nAttribute Content:\n\t%s\n", tempAttribute->name, tempAttribute->value);
    buffer = malloc(bytesNeeded+1);
    sprintf(buffer,"\nAttribute Name:\n\t%s\nAttribute Content:\n\t%s\n", tempAttribute->name, tempAttribute->value);

    return buffer;

}

int compareAttributes(const void *first, const void *second) {

    //Attribute* firstAttr = NULL;
    //Attribute* secondAttr = NULL;

    if (first == NULL || second == NULL) {
        return 0;
    }

    //return strcmp(firstAttr->value,secondAttr->value);
    return 0;
}

void deleteGroup(void* data) {
    
    Group* tmpGroup = (Group *)data;

    if (data == NULL) {
        return;
    }

    freeList(tmpGroup->rectangles);
    freeList(tmpGroup->paths);
    freeList(tmpGroup->circles);
    freeList(tmpGroup->groups);
    freeList(tmpGroup->otherAttributes);
    free(tmpGroup);
    return;
}

char* groupToString( void* data) {
    
    char* buffer = NULL;
    if (data == NULL) {
        return NULL;
    }
    Group* tmpGroup = (Group*)data;
    buffer = groupAttrToString(tmpGroup);
    return buffer;
}

int compareGroups(const void *first, const void *second){return 0;};

void deleteRectangle(void* data) {
    
    Rectangle* tempRect;

    if (data == NULL) {
        return;
    }
    
    tempRect = (Rectangle*) data;
    if(tempRect->otherAttributes != NULL) {
        freeList(tempRect->otherAttributes);
    } 
    free(tempRect);
    return;

}

char* rectangleToString(void* data) {

    char* buffer = NULL;
    if (data == NULL) {
        return NULL;
    }
    Rectangle* tmpRect = (Rectangle *)data;
    buffer = rectAttrToString(tmpRect);
    return buffer;
}

int compareRectangles(const void *first, const void *second) {
    return 0;
}

void deleteCircle(void* data){

    Circle* tempCircle;
    
    if (data == NULL) {
        return;
    }

    tempCircle = (Circle*)data;
    if (tempCircle->otherAttributes != NULL) {
        freeList(tempCircle->otherAttributes);
    }
    free(tempCircle);   
    return;
}

char* circleToString(void* data) {
    
    char* buffer = NULL;
    if (data == NULL) {
        return NULL;
    }
    Circle* tmpCircle = (Circle *)data;
    buffer = circleAttrToString(tmpCircle);
    return buffer;

    }
int compareCircles(const void *first, const void *second){return 0;}
void deletePath(void* data) {
    
    Path* tempPath = (Path *)data;

    if (data == NULL) {
        return;
    }

    if (tempPath->otherAttributes != NULL) {
        freeList(tempPath->otherAttributes);
    }
    free(tempPath->data);
    free(tempPath);
    return;
}

char* pathToString(void* data){

    char* buffer = NULL;
    if (data == NULL) {
        return NULL;
    }
    Path* tmpPath = (Path *)data;
    buffer = pathAttrToString(tmpPath);
    return buffer;

    }
int comparePaths(const void *first, const void *second){return 0;}



//HELPER FUNCTIONS

/*
* Implementation of XML parser is supplemented with code from example authored by Dodji Seketeli.
* Example code can be found at https://www.xmlsoft.org/examples/tree1.c
*/
void parseTree(xmlNode* xml_node, SVGimage* current_img)
{
    xmlNode *cur_node = NULL;
    parseSVGotherAttributes(xml_node,current_img);
    for (cur_node = xml_node->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {            
        if (!(strcmp((char *)cur_node->name,"title"))) {
            strncpy(current_img->title, (char*)cur_node->children->content, 255);
        } else if (!(strcmp((char *)cur_node->name,"desc"))) {
            strncpy(current_img->description, (char*)cur_node->children->content, 255);
        } else {
            parseAttributes(cur_node, current_img);
        }
        }
    }

}
        
void parseSVGotherAttributes(xmlNode* root_node, SVGimage* current_img) {

    xmlAttr* attr;

    for (attr = root_node->properties; attr != NULL; attr = attr->next) {
        Attribute* cur_attr;
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        cur_attr = malloc(sizeof(Attribute));
        cur_attr->name = malloc(strlen(attrName) + 1);
        cur_attr->value = malloc(strlen(cont) + 1);
        strcpy(cur_attr->name, attrName);
        strcpy(cur_attr->value,cont);
        insertBack(current_img->otherAttributes,cur_attr);
    }
}

/*
* parseAttributes parses the current xml_node and adds every attribute that is a child of that node
* to its associated List* in the SVGimage*
* Implementation of XML parser is supplemented with code from example authored by Dodji Seketeli.
* Example code can be found at https://www.xmlsoft.org/examples/tree1.c
*/

void parseAttributes(xmlNode* xml_node, SVGimage* current_img) {    

    if(!(strcasecmp((char*)xml_node->name,"RECT"))) {
        Rectangle* tmpRect = rectangleParser(xml_node);
        insertBack(current_img->rectangles,tmpRect);
        return;    
    } else if(!(strcasecmp((char*)xml_node->name,"CIRCLE"))) {
        Circle* tmpCircle = circleParser(xml_node);
        insertBack(current_img->circles,tmpCircle);
        return;
    } else if(!(strcasecmp((char*)xml_node->name,"PATH"))) {
        Path* tmpPath = pathParser(xml_node);
        insertBack(current_img->paths,tmpPath);
        return;
    } else if(!(strcasecmp((char*)xml_node->name,"G"))) {
        Group* tmpGroup = groupParser(xml_node);
        insertBack(current_img->groups,tmpGroup);
        return;
    }
    


}

Rectangle* rectangleParser(xmlNode* xml_node) {

    xmlAttr* attr;
    xmlNode* cur_node;
    cur_node = xml_node;
    Rectangle* cur_rect;

    cur_rect = malloc(sizeof(Rectangle));
    cur_rect->width = -1;
    cur_rect->height = -1;
    cur_rect->units[0] = '\0';
    cur_rect->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    char *units = NULL;
    //char *unitsBuffer = malloc(sizeof(char) * 5);
    //unitsBuffer[0] = '\0';

        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            
            if(!(strcasecmp(attrName,"X"))) {
                cur_rect->x = strtof(cont, &units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_rect->units,units);
                }
            }else if(!(strcasecmp(attrName,"Y"))) {
                cur_rect->y = strtof(cont, &units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_rect->units,units);
                }
            }else if(!(strcasecmp(attrName,"width"))) {
                cur_rect->width = strtof(cont,&units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_rect->units,units);
                }
            }else if(!(strcasecmp(attrName,"height"))) {
                cur_rect->height = strtof(cont,&units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_rect->units,units);
                }
            }else {
                Attribute* cur_attr = malloc(sizeof (Attribute));
                cur_attr->name = malloc(strlen(attrName) + 1);
                strcpy(cur_attr->name,attrName);
                cur_attr->value = malloc(strlen(cont) + 1);
                strcpy(cur_attr->value,cont);
                insertBack(cur_rect->otherAttributes,cur_attr);
            }
        }
        if(cur_rect->height < 0) {
            cur_rect->height = 0;
        }
        if(cur_rect->width < 0) {
            cur_rect->width = 0;
        }
    //free(unitsBuffer);
    return cur_rect;
}


Circle* circleParser(xmlNode* xml_node) {
    
    xmlAttr* attr;
    xmlNode* cur_node;
    cur_node = xml_node;
    Circle* cur_circle;
    char* units = NULL;

    cur_circle = malloc(sizeof(Circle));
    cur_circle->r = -1;
    cur_circle->units[0] = '\0';
    cur_circle->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    
        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);

            if(!(strcasecmp(attrName,"CX"))) {
                cur_circle->cx = strtof(cont, &units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_circle->units,units);
                }
            }else if(!(strcasecmp(attrName,"CY"))) {
                cur_circle->cy = strtof(cont, &units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_circle->units,units);
                }
            }else if(!(strcasecmp(attrName,"R"))) {
                cur_circle->r = strtof(cont,&units);
                if (isalpha(units[0]) != 0) {
                    strcpy(cur_circle->units,units);
                }
            }else {
                Attribute* cur_attr = malloc(sizeof (Attribute));
                cur_attr->name = malloc(strlen(attrName) + 1);
                strcpy(cur_attr->name,attrName);
                cur_attr->value = malloc(strlen(cont) + 1);
                strcpy(cur_attr->value,cont);
                insertBack(cur_circle->otherAttributes,cur_attr);
            }
        }
        if(cur_circle->r < 0) {
            cur_circle->r = 0;
        }
    
    //freeList(cur_circle->otherAttributes);
    return cur_circle;
}

Path* pathParser(xmlNode* xml_node) {

    xmlAttr* attr;
    xmlNode* cur_node;
    cur_node = xml_node;
    Path* cur_path;

    cur_path = malloc(sizeof(Path));
    cur_path->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);

        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            
            if(!(strcasecmp(attrName,"d"))) {
                cur_path->data = malloc(strlen(cont) + 1);
                strcpy(cur_path->data,cont);
            }else {
                Attribute* cur_attr = malloc(sizeof (Attribute));
                cur_attr->name = malloc(strlen(attrName) + 1);
                strcpy(cur_attr->name,attrName);
                cur_attr->value = malloc(strlen(cont) + 1);
                strcpy(cur_attr->value,cont);
                insertBack(cur_path->otherAttributes,cur_attr);
            }
        }
    return cur_path;
}

Group* groupParser(xmlNode* xml_node) {

    xmlAttr* attr;
    xmlNode* cur_node;
    cur_node = xml_node;
    Group* cur_group;

    cur_group = malloc(sizeof(Group));
    cur_group->rectangles = initializeList(&rectangleToString,&deleteRectangle,&compareRectangles);
    cur_group->circles = initializeList(&circleToString,&deleteCircle,&compareCircles);
    cur_group->paths = initializeList(&pathToString,&deletePath,&comparePaths);
    cur_group->groups = initializeList(&groupToString, &deleteGroup,&compareGroups);
    cur_group->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);


        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            Attribute* cur_attr = malloc(sizeof (Attribute));
            cur_attr->name = malloc(strlen((char*)attr->name) + 1);
            strcpy(cur_attr->name,(char*)attr->name);
            cur_attr->value = malloc(strlen((char*)attr->children->content) + 1);
            strcpy(cur_attr->value,(char*)attr->children->content);
            insertBack(cur_group->otherAttributes,cur_attr);
        }
        for (cur_node = cur_node->children; cur_node != NULL; cur_node = cur_node->next) {
            char *attrName = (char *)cur_node->name;
            
            if(!(strcasecmp(attrName,"RECT"))) {
                Rectangle *tmpRect = rectangleParser(cur_node);
                insertBack(cur_group->rectangles, tmpRect);
            }else if (!(strcasecmp(attrName,"CIRCLE"))) {
                Circle *tmpCircle = circleParser(cur_node);
                insertBack(cur_group->circles, tmpCircle);
            }else if (!(strcasecmp(attrName,"PATH"))) {
                Path *tmpPath = pathParser(cur_node);
                insertBack(cur_group->paths,tmpPath);
            }else if (!(strcasecmp(attrName,"G"))) {
                Group *tmpGroup = groupParser(cur_node);
                insertBack(cur_group->groups,tmpGroup);
            }
        }
    return cur_group;
}

char* rectAttrToString(Rectangle* rectangleToPrint) {

    void *elem;
    char *backupBuffer = NULL;
    char *returnedBuffer = NULL;
    int bytesNeeded = 0;

    //PRINT X COORDINATE
    bytesNeeded = snprintf(NULL,0,"\nRectangle X Coordinate:\n\t%.2f\n", rectangleToPrint->x) + 1;
    backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
    returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
    sprintf(backupBuffer, "\nRectangle X Coordinate:\n\t%.2f\n", rectangleToPrint->x);
    strcpy(returnedBuffer, backupBuffer);

    //PRINT Y COORDINATE
    bytesNeeded += snprintf(NULL,0,"\nRectangle Y Coordinate:\n\t%.2f\n", rectangleToPrint->y);
    backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
    returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
    sprintf(backupBuffer, "\nRectangle Y Coordinate:\n\t%.2f\n", rectangleToPrint->y);
    strcat(returnedBuffer, backupBuffer);

    if (rectangleToPrint->width >= 0 ) {
        bytesNeeded += snprintf(NULL,0,"\nRectangle Width:\n\t%.2f\n",rectangleToPrint->width);
        backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nRectangle Width:\n\t%.2f\n", rectangleToPrint->width);
        strcat(returnedBuffer, backupBuffer);
    }
    if (rectangleToPrint->height >= 0) {
        bytesNeeded += snprintf(NULL,0,"\nRectangle Height:\n\t%.2f\n",rectangleToPrint->height);
        backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nRectangle Height:\n\t%.2f\n", rectangleToPrint->height);
        strcat(returnedBuffer, backupBuffer);
    }
    if ((isalpha(rectangleToPrint->units[0])) != 0) {
        bytesNeeded += snprintf(NULL,0,"\nRectangle Measurements Units:\n\t%s\n", rectangleToPrint->units);
        backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nRectangle Measurements Units:\n\t%s\n", rectangleToPrint->units);
        strcat(returnedBuffer,backupBuffer);
    }
    if (rectangleToPrint->otherAttributes != NULL) {
        ListIterator otherIter = createIterator(rectangleToPrint->otherAttributes);
        while ((elem = nextElement(&otherIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem;
            bytesNeeded += snprintf(NULL,0,"\nRectangle Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
            backupBuffer = realloc(backupBuffer, bytesNeeded);           
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "\nRectangle Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
            strcat(returnedBuffer, backupBuffer);
        }
    }

    if (backupBuffer != NULL) {
        free(backupBuffer);
    }
    return returnedBuffer;
}

char* circleAttrToString(Circle* circleToPrint) {

    void *elem;
    char *backupBuffer = NULL;
    char *returnedBuffer = NULL;
    int bytesNeeded = 0;


    //PRINT X AND Y COORDINATES.
    bytesNeeded += snprintf(NULL,0,"\nCircle X Coordinate:\n\t%.2f\n", circleToPrint->cx) + 1;
    backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
    returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
    sprintf(backupBuffer, "\nCircle X Coordinate:\n\t%.2f\n", circleToPrint->cx);
    strcpy(returnedBuffer, backupBuffer);

    bytesNeeded += snprintf(NULL,0,"\nCircle Y Coordinate:\n\t%.2f\n", circleToPrint->cy) + 1;
    backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
    returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
    sprintf(backupBuffer, "\nCircle Y Coordinate:\n\t%.2f\n", circleToPrint->cy);
    strcat(returnedBuffer, backupBuffer);

    if (circleToPrint->r >= 0 ) {
        bytesNeeded += snprintf(NULL,0,"\nCircle Radius:\n\t%.2f\n",circleToPrint->r) + 1;
        backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nCircle Radius:\n\t%.2f\n", circleToPrint->r);
        strcat(returnedBuffer, backupBuffer);
    }
    if ((isalpha(circleToPrint->units[0])) != 0) {
        bytesNeeded += snprintf(NULL,0,"\nCircle Measurements Units:\n\t%s\n", circleToPrint->units);
        backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nCircle Measurements Units:\n\t%s\n", circleToPrint->units);
        strcat(returnedBuffer,backupBuffer);
    }
    if (circleToPrint->otherAttributes != NULL) {
        ListIterator otherIter = createIterator(circleToPrint->otherAttributes);
        while ((elem = nextElement(&otherIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem;
            bytesNeeded += snprintf(NULL,0,"\nCircle Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "\nCircle Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
            strcat(returnedBuffer, backupBuffer);
        }
    } 
    if (backupBuffer != NULL)
        free(backupBuffer);
    return returnedBuffer;

}

char* pathAttrToString(Path* pathToPrint) {
    
    void* elem;
    char* backupBuffer = NULL;
    char* returnedBuffer = NULL;
    int bytesNeeded = 0;

    bytesNeeded += snprintf(NULL, 0, "\nPath Data:\n\t%s\n", pathToPrint->data) + 1;
    backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
    returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
    sprintf(backupBuffer, "\nPath Data:\n\t%s\n", pathToPrint->data);
    strcpy(returnedBuffer, backupBuffer);

    if (pathToPrint->otherAttributes != NULL) {
        ListIterator otherIter = createIterator(pathToPrint->otherAttributes);
        while ((elem = nextElement(&otherIter)) != NULL) {
            Attribute* tmpAttr = (Attribute*)elem;
            bytesNeeded += snprintf(NULL,0,"\nPath Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "\nPath Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
            strcat(returnedBuffer, backupBuffer);
        }
    }
    if (backupBuffer != NULL) {
        free(backupBuffer);
    }
    return returnedBuffer;
}

char* groupAttrToString(Group* groupToPrint) {

    void *elem;
    char *backupBuffer = NULL;
    char *returnedBuffer = malloc(sizeof(char) * 2);
    int bytesNeeded = 0;
    strcpy(returnedBuffer, "");

    if (groupToPrint->rectangles != NULL) {
        ListIterator rectIter = createIterator(groupToPrint->rectangles);
        while ((elem = nextElement(&rectIter)) != NULL) {
            Rectangle* tmpRect = (Rectangle*)elem;
            char* elementString = rectangleToString(tmpRect);
            bytesNeeded += snprintf(NULL,0,"%s",elementString) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "%s", elementString);
            free(elementString);
            strcat(returnedBuffer, backupBuffer);
        }
    } 
    if (groupToPrint->circles != NULL) {
        ListIterator circleIter = createIterator(groupToPrint->circles);
        while ((elem = nextElement(&circleIter)) != NULL) {
            Circle* tmpCircle = (Circle*)elem;
            char* elementString = circleToString(tmpCircle);
            bytesNeeded += snprintf(NULL, 0,"%s", elementString) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "%s", elementString);
            free(elementString);
            strcat(returnedBuffer, backupBuffer);
        }
    }
    if (groupToPrint->paths != NULL) {
        ListIterator pathIter = createIterator(groupToPrint->paths);
        while ((elem = nextElement(&pathIter)) != NULL) {
            Path* tmpPath = (Path*)elem;
            char* elementString = pathToString(tmpPath);
            bytesNeeded += snprintf(NULL,0,"%s", elementString) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "%s", elementString);
            free(elementString);
            strcat(returnedBuffer, backupBuffer);
        }
    }
    if (groupToPrint->groups != NULL) {
        ListIterator groupIter = createIterator(groupToPrint->groups);
        while ((elem = nextElement(&groupIter)) != NULL) {
            Group* tmpGroup = (Group*)elem;
            char* elementString = groupToString(tmpGroup);
            bytesNeeded += snprintf(NULL,0,"%s", elementString) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "%s", elementString);
            free(elementString);
            strcat(returnedBuffer, backupBuffer);
        }
    }
    if (groupToPrint->otherAttributes != NULL) {
        ListIterator otherIter = createIterator(groupToPrint->otherAttributes);
        while ((elem = nextElement(&otherIter)) != NULL) {
            Attribute* tmpAttr = (Attribute*)elem;
            bytesNeeded += snprintf(NULL,0,"\nGroup Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value) + 1;
            backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
            returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
            sprintf(backupBuffer, "\nGroup Attribute %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
            strcat(returnedBuffer, backupBuffer);
        }
    }
    if (backupBuffer != NULL) {
        free(backupBuffer);
    }
    return returnedBuffer;

}

char* svgAttrToString(SVGimage* current_img) {

    ListIterator iter = createIterator(current_img->otherAttributes);
    void *elem;

    char *backupBuffer = NULL;
    char *returnedBuffer = malloc(sizeof(char) * 2);
    returnedBuffer[0] = '\0';
    int bytesNeeded = 0;

    while ((elem = nextElement(&iter)) != NULL) {

		Attribute* tmpAttr = (Attribute*)elem;
        bytesNeeded += snprintf(NULL,0,"\nSVG Image %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
        backupBuffer = realloc(backupBuffer, bytesNeeded);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nSVG Image %s:\n\t%s\n", tmpAttr->name, tmpAttr->value);
        strcat(returnedBuffer, backupBuffer);
	}
    if (backupBuffer != NULL) {
        free(backupBuffer);
    }
    return returnedBuffer;
}

/* Safely appends formatted strings to pointer passed to function. */
char* svgCat(char* stringToModify, char* nameOfElement, char* stringToAppend) {

    char *backupBuffer = NULL;
    int bytesNeeded = strlen(stringToModify + 1);

    bytesNeeded += snprintf(NULL,0,"\n%s:\n\t%s\n", nameOfElement, stringToAppend); //Calculates space needed to store string.
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

void deleteStub(void* stub){
    return;
}

/* MOD 2 FUNCTIONS */
List* getRects(SVGimage* img) {

    if(img == NULL) {
        return NULL;
    }

    List* rectList = initializeList(&rectangleToString, &deleteStub, &compareRectangles);
    ListIterator rectIter = createIterator(img->rectangles);
    ListIterator groupRectIter = createIterator(img->groups);
    void* elem = NULL;

    while ((elem = nextElement(&rectIter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        insertBack(rectList, tmpRect);
    }
    while ((elem = nextElement(&groupRectIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        getRectsHelper(tmpGroup, rectList);
    }
    return rectList;
}

void getRectsHelper(Group* group, List* rectList) {

    ListIterator rectIter = createIterator(group->rectangles);
    ListIterator groupRectIter = createIterator(group->groups);
    void* elem = NULL;

    while ((elem = nextElement(&rectIter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        insertBack(rectList, tmpRect);
    }
    while ((elem = nextElement(&groupRectIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        getRectsHelper(tmpGroup, rectList);
    }

}

List* getCircles(SVGimage* img) {

    if(img == NULL) {
        return NULL;
    }

    List* circleList = initializeList(&circleToString, &deleteStub, &compareCircles);
    ListIterator circleIter = createIterator(img->circles);
    ListIterator groupCircleIter = createIterator(img->groups);
    void* elem = NULL;

    while ((elem = nextElement(&circleIter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        insertBack(circleList, tmpCircle);
    }
    while ((elem = nextElement(&groupCircleIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        getCirclesHelper(tmpGroup, circleList);
    }
    return circleList;
}


void getCirclesHelper(Group* group, List* circleList) {

    ListIterator circleIter = createIterator(group->circles);
    ListIterator groupCircleIter = createIterator(group->groups);
    void* elem = NULL;

    while ((elem = nextElement(&circleIter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        insertBack(circleList, tmpCircle);
    }
    while ((elem = nextElement(&groupCircleIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        getCirclesHelper(tmpGroup, circleList);
    }

}


List* getPaths(SVGimage* img) {

    if(img == NULL) {
        return NULL;
    }

    List* pathList = initializeList(&pathToString, &deleteStub, &comparePaths);
    ListIterator mainIter = createIterator(img->paths);
    ListIterator groupPathIter = createIterator(img->groups);
    void* elem = NULL;


    while ((elem = nextElement(&mainIter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        insertBack(pathList,tmpPath);
    }
    while ((elem = nextElement(&groupPathIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        getPathsHelper(tmpGroup, pathList);
    }

    return pathList;

}

void getPathsHelper(Group* group, List* pathList) {

    ListIterator pathIter = createIterator(group->paths);
    ListIterator groupIter = createIterator(group->groups);
    void* elem = NULL;

    while ((elem = nextElement(&pathIter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        insertBack(pathList,tmpPath);
    }
    while ((elem = nextElement(&groupIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        getPathsHelper(tmpGroup, pathList);
    }
}

List* getGroups(SVGimage* img) {

    if (img == NULL) {
        return NULL;
    }
    List* groupList = initializeList(&groupToString, &deleteStub, &compareGroups);
    ListIterator mainIter = createIterator(img->groups);
    //ListIterator groupIter = createIterator(img)
    void* elem;

    while ((elem = nextElement(&mainIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        insertBack(groupList, tmpGroup);
        groupGetter(tmpGroup, groupList);
    }

    return groupList;

}

void groupGetter(Group* group, List* groupList) {
    
    ListIterator iter = createIterator(group->groups);
    void* elem = NULL;

    while ((elem = nextElement(&iter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        insertBack(groupList,tmpGroup);
        groupGetter(tmpGroup, groupList);
    }

}

int numRectsWithArea(SVGimage* img, float area) {
    
    if (area < 0 || img == NULL) {
        return 0;
    }

    ListIterator rectIter = createIterator(getRects(img));
    void* elem;
    int counter = 0;



    while ((elem = nextElement(&rectIter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        if (ceil(tmpRect->width * tmpRect->height) == ceil(area)) {
            counter++;
        }
    }
    return counter;
}

int numCirclesWithArea(SVGimage* img, float area) {
    
    if (area < 0 || img == NULL) {
        return 0;
    }

    ListIterator circleIter = createIterator(getCircles(img));
    void* elem;
    int counter = 0;


    while ((elem = nextElement(&circleIter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        if (ceil(tmpCircle->r * tmpCircle->r * M_PI) == ceil(area)) {
            counter++;
        }
    }
    return counter;
}

int numPathsWithdata(SVGimage* img, char* data) {

    if (data == NULL || img == NULL || data[0] == '\0') {
        return 0;
    }
    ListIterator pathIter = createIterator(getPaths(img));
    void* elem;
    int counter = 0;



    while ((elem = nextElement(&pathIter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        if ((strcasecmp(tmpPath->data,data)) == 0) {
            counter++;
        }
    }
    return counter;
}

int numGroupsWithLen(SVGimage* img, int len) {
    
    if (img == NULL || len < 0) {
        return 0;
    }

    ListIterator iter = createIterator(img->groups);
    void* elem = NULL;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        int tmpGroupTotal = 0;
        tmpGroupTotal += getLength(tmpGroup->rectangles) + getLength(tmpGroup->circles) + getLength(tmpGroup->paths) + getLength(tmpGroup->groups);
        if (tmpGroupTotal == len) {
            counter++;
        }
    }
    
    return counter;
}

int numAttr(SVGimage* img) {

    if (img == NULL) {
        return 0;
    }

    List* list = getRects(img);
    ListIterator iter = createIterator(list); //FREE LIST AFTERWARDS
    void* elem = NULL;
    int counter = 0;

    while ((elem = nextElement(&iter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        counter += getLength(tmpRect->otherAttributes);
    }
    freeList(list);

    list = getCircles(img);
    iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        counter += getLength(tmpCircle->otherAttributes);
    }
    freeList(list);

    list = getPaths(img);
    iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        counter += getLength(tmpPath->otherAttributes);
    }
    freeList(list);

    list = getGroups(img);
    iter = createIterator(list);
    while ((elem = nextElement(&iter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        counter += getLength(tmpGroup->otherAttributes);
    }
    freeList(list);

    counter += getLength(img->otherAttributes);

    return counter;

}

// A2 MODULE 1 FUNCTIONS.
/** Function to validating an existing a SVGimage object against a SVG schema file
 *@pre 
    SVGimage object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post SVGimage has not been modified in any way
 *@return the boolean aud indicating whether the SVGimage is valid
 *@param obj - a pointer to a GPXSVGimagedoc struct
 *@param obj - the name iof a schema file
 **/
bool validateSVGimage(SVGimage* image, char* schemaFile) {
    
    if (image == NULL || schemaFile == NULL || schemaFile[0] == '\0') {
        return false;
    }
    return true;

}

/** Function to create an SVG object based on the contents of an SVG file.
 * This function must validate the XML tree generated by libxml against a SVG schema file
 * before attempting to traverse the tree and create an SVGimage struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
       Schema file name is not NULL/empty, and represents a valid schema file
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, or SVG file was invalid, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createValidSVGimage(char* fileName, char* schemaFile);

/** Function to writing a SVGimage into a file in SVG format.
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post SVGimage has not been modified in any way, and a file representing the
    SVGimage contents in SVG format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    doc - a pointer to a SVGimage struct
 	fileName - the name of the output file
 **/
bool writeSVGimage(SVGimage* image, char* fileName);

xmlDocPtr convertSVGimage(SVGimage* image, char* fileName) {

    if (image == NULL || fileName == NULL || fileName[0] == '\0') {
        return NULL;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNsPtr docNamespace = NULL;

    //Create new xmlDoc. Set root_node to type SVG. Populate namespace of doc.
    doc = xmlNewDoc((xmlChar*) "1.0");
    root_node = xmlNewNode(NULL, (xmlChar*) "svg");
    docNamespace = xmlNewNs(root_node, (xmlChar*)image->namespace, NULL);
    xmlSetNs(root_node, docNamespace);

    //Set title and desc.
    if (image->title[0] != '\0') {
        xmlNewChild(root_node,NULL, (xmlChar*) "title", (xmlChar*) image->title);
    }
    if (image->desc[0] != '\0') {
        xmlNewChild(root_node,NULL, (xmlChar*) "desc", (xmlChar*) image->description);
    }

    //Parse Elements
    imgOtherToXml(image->otherAttributes, root_node);
    imgRectToXml(image->rectangles, root_node);
    imgCircleToXml(image->circles, root_node);
    imgPathToXml(image->paths, root_node);
    imgGroupToXml(image->groups, root_node);

    return doc;
}

void imgOtherToXml(List* imageAttributes, xmlNodePtr root_node) {

    List* attrList = initalizeList(&attributeToString, &deleteStub, &compareAttributes);
    ListIterator attrIter = createIterator(imageAttributes);
    void* elem = NULL;
    xmlNodePtr node = NULL;

    while((elem = nextElement(&attrIter)) != NULL) {
        Attribute* tmpAttr = (Attribute*)elem;
        insertBack(attrList, tmpAttr);
    }
    
    ListIterator iter = createIterator(attrList);
    while((elem = nextElement(&iter)) != NULL) {
        Attribute* tmpAttr = (Attribute*)elem;
        xmlNewProp(root_node, (xmlChar*) tmpAttr->name, (xmlChar*) tmpAttr->value);
    }
    freeList(attrList);
}

void imgRectToXml(List* imageRectangles, xmlNodePtr root_node) {

    List* rectList = initializeList(&rectangleToString, &deleteStub, &compareRectangles);
    ListIterator rectIter = createIterator(imageRectangles);
    void* elem = NULL;
    xmlNodePtr node = NULL;
    char* attrBuf = NULL;
    int bytesNeeded = 0;

    while ((elem = nextElement(&rectIter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        insertBack(rectList, tmpRect);
    }
    
    ListIterator iter = createIterator(rectList);
    while ((elem = nextElement(&iter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        node = xmlNewNode(NULL, (xmlChar*) "rect");

        if (tmpRect->units[0] != '\0' && tmpRect->units != NULL) {
            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpRect->x, tmpRect->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpRect->x,tmpRect->units);
            xmlNewProp(node, (xmlChar*) "x", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpRect->y, tmpRect->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpRect->y, tmpRect->units);
            xmlNewProp(node, (xmlChar*) "y", (xmlChar*) attrBuf);
            
            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpRect->width, tmpRect->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpRect->width, tmpRect->units);
            xmlNewProp(node, (xmlChar*) "width", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpRect->length, tmpRect->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpRect->length, tmpRect->units);
            xmlNewProp(node, (xmlChar*) "length", (xmlChar*) attrBuf);  

        } else {

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpRect->x);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpRect->x);
            xmlNewProp(node, (xmlChar*) "x", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpRect->y);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpRect->y);
            xmlNewProp(node, (xmlChar*) "y", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpRect->width);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpRect->width);
            xmlNewProp(node, (xmlChar*) "width", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpRect->length);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpRect->length);
            xmlNewProp(node, (xmlChar*) "length", (xmlChar*) attrBuf); 
        }
        if (getFromFront(tmpRect->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpRect->otherAttributes);
            void* otherElem = NULL;

            while((elem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) otherElem->name, (xmlChar*) otherElem->value); 
            }
        }
        xmlAddChild(root_node, node);
    }
    freeList(rectList);
}

void imgCircleToXml(List* imageCircles, xmlNodePtr root_node) {

    List* circleList = initializeList(&circleToString, &deleteStub, &compareCircles);
    ListIterator circleIter = createIterator(imageCircles);
    void* elem = NULL;
    xmlNodePtr node = NULL;
    char* attrBuf = NULL;
    int bytesNeeded = 0;

    while ((elem = nextElement(&circleIter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        insertBack(circleList, tmpCircle);
    }
    
    ListIterator iter = createIterator(circleList);
    while ((elem = nextElement(&iter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        node = xmlNewNode(NULL, (xmlChar*) "circle");

        if (tmpCircle->units[0] != '\0' && tmpCircle->units != NULL) {
            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpCircle->cx, tmpCircle->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpCircle->cx, tmpCircle->units);
            xmlNewProp(node, (xmlChar*) "cx", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpCircle->cy, tmpCircle->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpCircle->cy, tmpCircle->units);
            xmlNewProp(node, (xmlChar*) "cy", (xmlChar*) attrBuf);
            
            bytesNeeded = snprintf(NULL,0,"%.2f%s", tmpCircle->r, tmpCircle->units);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f%s", tmpCircle->r, tmpCircle->units);
            xmlNewProp(node, (xmlChar*) "r", (xmlChar*) attrBuf);        
        } else {

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpCircle->cx);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpCircle->cx);
            xmlNewProp(node, (xmlChar*) "cx", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpCircle->cy);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpCircle->cy);
            xmlNewProp(node, (xmlChar*) "cy", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%.2f", tmpCircle->r);
            attrBuf = realloc(attrBuf, bytesNeeded);
            sprintf(attrBuf, "%.2f", tmpCircle->r);
            xmlNewProp(node, (xmlChar*) "r", (xmlChar*) attrBuf);
        }
        if (getFromFront(tmpCircle->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpCircle->otherAttributes);
            void* otherElem = NULL;

            while((elem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) otherElem->name, (xmlChar*) otherElem->value); 
            }
        }
        xmlAddChild(root_node, node);   
    }
    freeList(circleList);
}

void imgPathToXml(List* imagePaths, xmlNodePtr root_node) {

    List* pathList = initializeList(&pathToString, &deleteStub, &comparePaths);
    ListIterator pathIter = createIterator(imagePaths);
    void* elem = NULL;
    xmlNodePtr node = NULL;

    while ((elem = nextElement(&pathIter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        insertBack(pathList, tmpPath);
    }
    
    ListIterator iter = createIterator(pathList);
    while ((elem = nextElement(&iter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        node = xmlNewNode(NULL, (xmlChar*) "path");
        xmlNewProp(node, (xmlChar*) "d", (xmlChar*) tmpPath->data);

        if (getFromFront(tmpPath->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpPath->otherAttributes);
            void* otherElem = NULL;

            while((elem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) otherElem->name, (xmlChar*) otherElem->value); 
            }
        }
        xmlAddChild(root_node, node);
    }
    freeList(pathList);
}

void imgGroupToXml(List* imageGroups, XmlNodePtr root_node) {

    List* groupList = initalizeList(&groupToString, &deleteStub, &compareGroups);
    ListIterator groupIter = createIterator(imageGroups);
    void* elem = NULL;
    xmlNodePtr node = NULL;

    while((elem = nextElement(&groupIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        insertBack(groupList,tmpGroup);
    }
    
    ListIterator iter = createIterator(groupList);
    while((elem = nextElement(&iter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        node = xmlNewNode(NULL, (xmlChar*) "g");

        if (getFromFront(tmpGroup->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpGroup->otherAttributes);
            void* otherElem = NULL;

            while((elem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) otherElem->name, (xmlChar*) otherElem->value); 
            }
        }
        if (getFromFront(tmpGroup->rectangles) != NULL) {
            imgRectToXml(tmpGroup->rectangles,node);
        }
        if (getFromFront(tmpGroup->circles) != NULL) {
            imgCircleToXml(tmpgroup->circles,node);
        }
        if (getFromFront(tmpGroup->paths) != NULL) {
            imgPathToXml(tmpGroup->paths,node);
        }
        if (getFromFront(tmpGroup->groups) != NULL) {
            imgGroupToXml(tmpGroup->groups,node);
        }
        xmlAddChild(root_node, node);        
    }
    freeList(groupList);
}