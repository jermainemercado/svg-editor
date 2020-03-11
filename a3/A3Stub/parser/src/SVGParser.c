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
    cur_rect->x = 0;
    cur_rect->y = 0;
    cur_rect->width = 0;
    cur_rect->height = 0;
    cur_rect->units[0] = '\0';
    cur_rect->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    char *units = NULL;

    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        
        if(!(strcasecmp(attrName,"X"))) {
            cur_rect->x = strtof(cont, NULL);
        }else if(!(strcasecmp(attrName,"Y"))) {
            cur_rect->y = strtof(cont, NULL);
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
    cur_circle->cx = 0;
    cur_circle->cy = 0;
    cur_circle->r = 0;
    cur_circle->units[0] = '\0';
    cur_circle->otherAttributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
    
        for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);

            if(!(strcasecmp(attrName,"CX"))) {
                cur_circle->cx = strtof(cont, NULL);
            }else if(!(strcasecmp(attrName,"CY"))) {
                cur_circle->cy = strtof(cont, NULL);
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
 //validateSVGimage is based on http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html
 //retrieved on Wednesday February 19, 2020.
 **/
bool validateSVGimage(SVGimage* image, char* schemaFile) {
    
    if (image == NULL || schemaFile == NULL || schemaFile[0] == '\0') {
        return false;
    }
    if (image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL || validateHelper(image) == false) {
        return false;
    }

    xmlDocPtr doc;
	xmlSchemaPtr schema = NULL;
	xmlSchemaParserCtxtPtr ctxt;
	char *XSDFileName = schemaFile;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(XSDFileName);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    doc = convertSVGimage(image);
	if (doc == NULL) {
		    return false;
	} else {
		xmlSchemaValidCtxtPtr ctxt;
		int ret;

		ctxt = xmlSchemaNewValidCtxt(schema);
		xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
		ret = xmlSchemaValidateDoc(ctxt, doc);
		if (ret != 0) {
            return false;
        }
		xmlSchemaFreeValidCtxt(ctxt);
		xmlFreeDoc(doc);
	}

	// free the resource
    if(schema != NULL) {
    	xmlSchemaFree(schema);
    }
    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

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
SVGimage* createValidSVGimage(char* fileName, char* schemaFile) {

    if (fileName == NULL || fileName[0] == '\0' || schemaFile == NULL || schemaFile[0] == '\0' || access(fileName, F_OK) != 0 || access(schemaFile, F_OK) != 0) {
        return NULL;
    }
    SVGimage* image = createSVGimage(fileName);
    if(validateSVGimage(image,schemaFile) != true) {
        return NULL;
    }
    xmlCleanupParser();
    return image;
}

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
bool writeSVGimage(SVGimage* image, char* fileName) {
    if (image == NULL || fileName == NULL || strcasecmp(strrchr(fileName,'.'),".svg") != 0 || validateHelper(image) == false) {
        return false;
    }
    xmlDocPtr doc = convertSVGimage(image);
    xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return true;
}

xmlDocPtr convertSVGimage(SVGimage* image) {

    if (image == NULL) {
        return NULL;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNsPtr docNamespace = NULL;

    //Create new xmlDoc. Set root_node to type SVG. Populate namespace of doc.
    doc = xmlNewDoc((xmlChar*) "1.0");
    root_node = xmlNewNode(NULL, (xmlChar*) "svg");
    xmlDocSetRootElement(doc,root_node);
    docNamespace = xmlNewNs(root_node, (xmlChar*)image->namespace, NULL);
    xmlSetNs(root_node, docNamespace);

    //Set title and desc.
    if (image->title[0] != '\0') {
        xmlNewChild(root_node, NULL, (xmlChar*) "title", (xmlChar*) image->title);
    }
    if (image->description[0] != '\0') {
        xmlNewChild(root_node, NULL, (xmlChar*) "desc", (xmlChar*) image->description);
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

    List* attrList = initializeList(&attributeToString, &deleteStub, &compareAttributes);
    ListIterator attrIter = createIterator(imageAttributes);
    void* elem = NULL;

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
        node = xmlNewChild(root_node, NULL, (xmlChar*) "rect", NULL);

        bytesNeeded = snprintf(NULL,0,"%f", tmpRect->x) + 1;
        attrBuf = realloc(attrBuf, bytesNeeded + 1);
        sprintf(attrBuf, "%f", tmpRect->x);
        xmlNewProp(node, (xmlChar*) "x", (xmlChar*) attrBuf);

        bytesNeeded = snprintf(NULL,0,"%f", tmpRect->y) + 1;
        attrBuf = realloc(attrBuf, bytesNeeded + 1);
        sprintf(attrBuf, "%f", tmpRect->y);
        xmlNewProp(node, (xmlChar*) "y", (xmlChar*) attrBuf);

        if (tmpRect->units[0] != '\0' && tmpRect->units != NULL) {            
            bytesNeeded = snprintf(NULL,0,"%f%s", tmpRect->width, tmpRect->units) + 1;
            attrBuf = realloc(attrBuf, bytesNeeded + 1);
            sprintf(attrBuf, "%f%s", tmpRect->width, tmpRect->units);
            xmlNewProp(node, (xmlChar*) "width", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%f%s", tmpRect->height, tmpRect->units) + 1;
            attrBuf = realloc(attrBuf, bytesNeeded + 1);
            sprintf(attrBuf, "%f%s", tmpRect->height, tmpRect->units);
            xmlNewProp(node, (xmlChar*) "height", (xmlChar*) attrBuf);  
        } else {
            bytesNeeded = snprintf(NULL,0,"%f", tmpRect->width) + 1;
            attrBuf = realloc(attrBuf, bytesNeeded + 1);
            sprintf(attrBuf, "%f", tmpRect->width);
            xmlNewProp(node, (xmlChar*) "width", (xmlChar*) attrBuf);

            bytesNeeded = snprintf(NULL,0,"%f", tmpRect->height) + 1;
            attrBuf = realloc(attrBuf, bytesNeeded + 1);
            sprintf(attrBuf, "%f", tmpRect->height);
            xmlNewProp(node, (xmlChar*) "height", (xmlChar*) attrBuf); 
        }

        if (getFromFront(tmpRect->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpRect->otherAttributes);
            void* otherElem = NULL;

            while((otherElem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) tmpAttr->name, (xmlChar*) tmpAttr->value); 
            }
        }
    }
    if (attrBuf != NULL) {
        free(attrBuf);
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
        node = xmlNewChild(root_node, NULL, (xmlChar*) "circle", NULL);

        bytesNeeded = snprintf(NULL,0,"%f", tmpCircle->cx) + 1;
        attrBuf = realloc(attrBuf, bytesNeeded + 1);
        sprintf(attrBuf, "%f", tmpCircle->cx);
        xmlNewProp(node, (xmlChar*) "cx", (xmlChar*) attrBuf);

        bytesNeeded = snprintf(NULL,0,"%f", tmpCircle->cy) + 1;
        attrBuf = realloc(attrBuf, bytesNeeded + 1);
        sprintf(attrBuf, "%f", tmpCircle->cy);
        xmlNewProp(node, (xmlChar*) "cy", (xmlChar*) attrBuf);

        if (tmpCircle->units[0] != '\0' && tmpCircle->units != NULL) {
            bytesNeeded = snprintf(NULL,0,"%f%s", tmpCircle->r, tmpCircle->units) + 1;
            attrBuf = realloc(attrBuf, bytesNeeded + 1);
            sprintf(attrBuf, "%f%s", tmpCircle->r, tmpCircle->units);
            xmlNewProp(node, (xmlChar*) "r", (xmlChar*) attrBuf);        
        } else {
            bytesNeeded = snprintf(NULL,0,"%f", tmpCircle->r) + 1;
            attrBuf = realloc(attrBuf, bytesNeeded + 1);
            sprintf(attrBuf, "%f", tmpCircle->r);
            xmlNewProp(node, (xmlChar*) "r", (xmlChar*) attrBuf);
        }

        if (getFromFront(tmpCircle->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpCircle->otherAttributes);
            void* otherElem = NULL;

            while((otherElem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) tmpAttr->name, (xmlChar*) tmpAttr->value); 
            }
        }  
    }
    if (attrBuf != NULL) {
        free(attrBuf);
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
        node = xmlNewChild(root_node, NULL, (xmlChar*) "path", NULL);
        xmlNewProp(node, (xmlChar*) "d", (xmlChar*) tmpPath->data);

        if (getFromFront(tmpPath->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpPath->otherAttributes);
            void* otherElem = NULL;

            while((otherElem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) tmpAttr->name, (xmlChar*) tmpAttr->value); 
            }
        }
    }
    freeList(pathList);
}

void imgGroupToXml(List* imageGroups, xmlNodePtr root_node) {

    List* groupList = initializeList(&groupToString, &deleteStub, &compareGroups);
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
        node = xmlNewChild(root_node, NULL, (xmlChar*) "g", NULL);

        if (getFromFront(tmpGroup->otherAttributes) != NULL) {
            ListIterator otherIter = createIterator(tmpGroup->otherAttributes);
            void* otherElem = NULL;

            while((otherElem = nextElement(&otherIter)) != NULL) {
                Attribute* tmpAttr = (Attribute*) otherElem;
                xmlNewProp(node, (xmlChar*) tmpAttr->name, (xmlChar*) tmpAttr->value); 
            }
        }
        if (getFromFront(tmpGroup->rectangles) != NULL) {
            imgRectToXml(tmpGroup->rectangles,node);
        }
        if (getFromFront(tmpGroup->circles) != NULL) {
            imgCircleToXml(tmpGroup->circles,node);
        }
        if (getFromFront(tmpGroup->paths) != NULL) {
            imgPathToXml(tmpGroup->paths,node);
        }
        if (getFromFront(tmpGroup->groups) != NULL) {
            imgGroupToXml(tmpGroup->groups,node);
        }    
    }
    freeList(groupList);
}

bool validateHelper(SVGimage* image) {


	if (validateAttrs(image->otherAttributes) == false || image->rectangles == NULL || image->paths == NULL || image->groups == NULL || image->circles == NULL) {
		return false;
	}

	void* elem = NULL;
    List* rectList = getRects(image);
    List* circleList = getCircles(image);
    List* pathList = getPaths(image);
    List* groupList = getGroups(image);

	ListIterator rectIter = createIterator(rectList);
	ListIterator circleIter = createIterator(circleList);
	ListIterator pathIter = createIterator(pathList);
	ListIterator groupIter = createIterator(groupList);
	while((elem = nextElement(&rectIter)) != NULL) {
		Rectangle* tmpRect = (Rectangle*)elem;
		if (validateRect(tmpRect) == false) {
			return false;
		}
	}
	while((elem = nextElement(&circleIter)) != NULL) {
		Circle* tmpCircle = (Circle*)elem;
		if (validateCircle(tmpCircle) == false) {
			return false;
		}
	}
	while((elem = nextElement(&pathIter)) != NULL) {
		Path* tmpPath = (Path*)elem;
		if (validatePath(tmpPath) == false) {
			return false;
		}
	}
	while((elem = nextElement(&groupIter)) != NULL)  {
		Group* tmpGroup = (Group*)elem;
		if (validateGroup(tmpGroup) == false) {
			return false;
		}
	}
    if (rectList != NULL) {
        freeList(rectList);
    }
    if (circleList != NULL) {
        freeList(circleList);
    }
    if (pathList != NULL) {
        freeList(pathList);
    }
    if (groupList != NULL) {
        freeList(groupList);
    }
	return true;

}

bool validateRect(Rectangle* imgRect) {

	if (imgRect->height < 0 || imgRect->width < 0 || imgRect->otherAttributes == NULL || validateAttrs(imgRect->otherAttributes) == false) {
		return false;
	} else {
		return true;
	}

}

bool validateCircle(Circle* imgCircle) {

	if (imgCircle->r < 0 || imgCircle->otherAttributes == NULL || validateAttrs(imgCircle->otherAttributes) == false) {
		return false;
	} else {
		return true;
	}
}

bool validatePath(Path* imgPath) {

	if (imgPath->data == NULL || imgPath->otherAttributes == NULL || validateAttrs(imgPath->otherAttributes) == false) {
		return false;
	} else {
		return true;
	}
}

bool validateGroup(Group* imgGroup) {

	if (imgGroup->rectangles == NULL || imgGroup->circles == NULL || imgGroup->paths == NULL || imgGroup->groups == NULL || imgGroup->otherAttributes == NULL || validateAttrs(imgGroup->otherAttributes) == false) {
		return false;
	} else {
		return true;
	}
}

bool validateAttrs(List* otherAttributes) {
	ListIterator iter = createIterator(otherAttributes);
	void* elem = NULL;

	while((elem = nextElement(&iter)) != NULL) {
		Attribute* tmpAttr = (Attribute*)elem;
		if (tmpAttr->name == NULL || tmpAttr->value == NULL) {
			return false;
		}
	}
	return true;
}

//A2 MODULE 2
/** Function to setting an attribute in an SVGimage or component
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newAttribute is not NULL
 *@post The appropriate attribute was set corectly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    elemIndex - index of thje lement to modify
    newAttribute - struct containing name and value of the updated attribute
 **/
void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {

    if (image == NULL || validateHelper(image) == false || newAttribute == NULL)  {
        return;
    }
    if (elemType == SVG_IMAGE) {
        setOtherAttr(image->otherAttributes, newAttribute);
        return;
    }
    if (elemType == CIRC || elemType == RECT || elemType == PATH || elemType == GROUP) {
        if (elemType == RECT) {
            setRectAttr(image, newAttribute, elemIndex);
            return;
        }
        if (elemType == CIRC) {
            setCircleAttr(image, newAttribute, elemIndex);
            return;
        }
        if (elemType == PATH) {
            setPathAttr(image, newAttribute, elemIndex);
            return;
        }
        if (elemType == GROUP) {
            setGroupAttr(image, newAttribute, elemIndex);
            return;
        }
    }
}


void setRectAttr(SVGimage* image, Attribute* newAttribute, int elemIndex) {

    if (elemIndex >= getLength(image->rectangles) || getLength(image->rectangles) < 0 || elemIndex < 0) {
        return;
    }

    ListIterator rectIter = createIterator(image->rectangles);
    void* elem = NULL;
    int counter = 0;

    while((elem = nextElement(&rectIter)) != NULL) {
        if (counter == elemIndex) {
            Rectangle* tmpRect = (Rectangle*)elem;
            if(strcasecmp(newAttribute->name, "x") == 0) {
                tmpRect->x = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            }
            else if(strcasecmp(newAttribute->name, "y") == 0) {
                tmpRect->y = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            }
            else if(strcasecmp(newAttribute->name, "width") == 0) {
                tmpRect->width = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            }
            else if(strcasecmp(newAttribute->name, "height") == 0) {
                tmpRect->height = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            }
            else {
                setOtherAttr(tmpRect->otherAttributes, newAttribute);
                return;
            }
        }
        counter++;
    }
}

void setCircleAttr(SVGimage* image, Attribute* newAttribute, int elemIndex) {

    if (elemIndex >= getLength(image->circles) || getLength(image->circles) < 0 || elemIndex < 0) {
        return;
    }
    ListIterator circleIter = createIterator(image->circles);
    void* elem = NULL;
    int counter = 0;

    while((elem = nextElement(&circleIter)) != NULL) {
        if (counter == elemIndex) {
            Circle* tmpCircle = (Circle*)elem;
            if(strcasecmp(newAttribute->name, "cx") == 0) {
                tmpCircle->cx = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            } else if(strcasecmp(newAttribute->name, "cy") == 0) {
                tmpCircle->cy = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            } else if(strcasecmp(newAttribute->name, "r") == 0) {
                tmpCircle->r = strtof(newAttribute->value, NULL);
                deleteAttribute(newAttribute);
                return;
            } else {
                setOtherAttr(tmpCircle->otherAttributes, newAttribute);
                return;
            }
        }
        counter++;
    }
}

void setPathAttr(SVGimage* image, Attribute* newAttribute, int elemIndex) {

    if (elemIndex >= getLength(image->paths) || getLength(image->paths) < 0 || elemIndex < 0) {
        return;
    }

    ListIterator pathIter = createIterator(image->paths);
    void* elem = NULL;
    int counter = 0;

    while((elem = nextElement(&pathIter)) != NULL) {
        if (counter == elemIndex) {
            Path* tmpPath = (Path*)elem;
            if(strcasecmp(newAttribute->name,"d") == 0) {
                int bytesNeeded = snprintf(NULL,0,"%s", newAttribute->value) + 1;
                tmpPath->data = realloc(tmpPath->data, bytesNeeded + 1);
                sprintf(tmpPath->data, "%s", newAttribute->value);
                deleteAttribute(newAttribute);
                return;
            } else {
                setOtherAttr(tmpPath->otherAttributes, newAttribute);
                return;
            }
        }
        counter++;
    }
}

void setGroupAttr(SVGimage* image, Attribute* newAttribute, int elemIndex) {

    if (elemIndex >= getLength(image->paths) || getLength(image->paths) < 0 || elemIndex < 0) {
        return;
    }

    ListIterator groupIter = createIterator(image->groups);
    void* elem = NULL;
    int counter = 0;

    while((elem = nextElement(&groupIter)) != NULL) {
        if (counter == elemIndex) {
            Group* tmpGroup = (Group*)elem;
            setOtherAttr(tmpGroup->otherAttributes, newAttribute);
            deleteAttribute(newAttribute);
            return;
        }
        counter++;
    }

}

void setOtherAttr(List* otherAttributes, Attribute* newAttribute) {
    ListIterator otherIter = createIterator(otherAttributes);
    void* otherElem = NULL;
    while((otherElem = nextElement(&otherIter)) != NULL) {
        Attribute* tmpAttr = (Attribute*)otherElem;
        if (strcasecmp(tmpAttr->name,newAttribute->name) == 0) {
            int bytesNeeded = snprintf(NULL,0,"%s",newAttribute->value) + 1;
            tmpAttr->value = realloc(tmpAttr->value, bytesNeeded + 1);
            sprintf(tmpAttr->value, "%s", newAttribute->value);
            deleteAttribute(newAttribute);
            return;
        }
    }
    insertBack(otherAttributes,newAttribute);
    return;
}


void addComponent(SVGimage* image, elementType type, void* newElement){
    if (image == NULL || newElement == NULL) {
        return;
    }
    if (type == CIRC || type == RECT || type == PATH) {
        if (type == CIRC) {
            Circle* tmpCircle = (Circle*)newElement;
            insertBack(image->circles, tmpCircle);
            return;
        }
        if (type == RECT) {
            Rectangle* tmpRectagnle = (Rectangle*)newElement;
            insertBack(image->rectangles, tmpRectagnle);
            return;
        }
        if (type == PATH) {
            Path* tmpPath = (Path*)newElement;
            insertBack(image->paths, tmpPath);
            return;
        }
    }
}
//MODULE 3 FUNCTIONS

char* attrToJSON(const Attribute *a){

    char* stringBuf = NULL;
    int bytesNeeded = 0;
    if(a == NULL || a->name == NULL || a->value == NULL || a->name[0] == '\0' || a->value[0] == '\0') {
        stringBuf = malloc(sizeof(char) * 4);
        sprintf(stringBuf, "{}");
        return stringBuf;
    }


    bytesNeeded = snprintf(NULL, 0, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value) + 1;
    stringBuf = malloc(bytesNeeded + 1);
    sprintf(stringBuf, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
    
    return stringBuf;
}

char* circleToJSON(const Circle *c){

    char* stringBuf = NULL;
    int bytesNeeded = 0;
    if(c == NULL) {
        stringBuf = malloc(sizeof(char) * 4);
        sprintf(stringBuf, "{}");
        return stringBuf;
    }


    bytesNeeded = snprintf(NULL, 0, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, getLength(c->otherAttributes), c->units) + 1;
    stringBuf = malloc(bytesNeeded + 1);
    sprintf(stringBuf, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, getLength(c->otherAttributes), c->units);

    return stringBuf;
}

char* rectToJSON(const Rectangle *r){

    char* stringBuf = NULL;
    int bytesNeeded = 0;
    if(r == NULL) {
        stringBuf = malloc(sizeof(char) * 4);
        sprintf(stringBuf, "{}");
        return stringBuf;
    }


    bytesNeeded = snprintf(NULL, 0, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units) + 1;
    stringBuf = malloc(bytesNeeded + 1);
    sprintf(stringBuf, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, getLength(r->otherAttributes), r->units);
    
    return stringBuf;
}

char* pathToJSON(const Path *p){

    char* stringBuf;
    int bytesNeeded = 0;
    char* truncateBuf = malloc(sizeof(char) * 65);

    if(p == NULL || p->data == NULL || p->data[0] == '\0') {
        stringBuf = malloc(sizeof(char) * 4);
        sprintf(stringBuf, "{}");
        free(truncateBuf);
        return stringBuf;
    }
    
    truncateBuf = strncpy(truncateBuf, p->data, 65);
    if (truncateBuf != NULL) {
        bytesNeeded = snprintf(NULL, 0, "{\"d\":\"%s\",\"numAttr\":%d}", truncateBuf, getLength(p->otherAttributes)) + 1;
        stringBuf = malloc(bytesNeeded + 1);
        sprintf(stringBuf, "{\"d\":\"%s\",\"numAttr\":%d}", truncateBuf, getLength(p->otherAttributes));
    }


    if(truncateBuf != NULL) {
        free(truncateBuf);
    }
    
    return stringBuf;
}

char* groupToJSON(const Group *g) {

    char* stringBuf = NULL;
    int bytesNeeded = 0;
    int numberOfChildren = 0;
    if(g == NULL) {
        stringBuf = malloc(sizeof(char) * 4);
        sprintf(stringBuf, "{}");
        return stringBuf;        
    } 
    numberOfChildren = getLength(g->rectangles) + getLength(g->circles) + getLength(g->paths) + getLength(g->groups);

    bytesNeeded = snprintf(NULL, 0, "{\"children\":%d,\"numAttr\":%d}", numberOfChildren, getLength(g->otherAttributes)) + 1;
    stringBuf = malloc(bytesNeeded + 1);
    sprintf(stringBuf, "{\"children\":%d,\"numAttr\":%d}", numberOfChildren, getLength(g->otherAttributes));

    return stringBuf;
    


}

char* attrListToJSON(const List *list){
    
    int bytesNeeded = 0;
    char* stringBuf = NULL;
    char* backupBuffer = NULL;
    void* elem = NULL;
    int counter = 0;

    if ((List*)list == NULL || getFromFront((List*)list) == NULL) {
        stringBuf = malloc(4);
        sprintf(stringBuf, "[]");
        return stringBuf;
    }

    ListIterator attrIter = createIterator((List*)list);
    stringBuf = malloc(2);
    sprintf(stringBuf, "[");

    while((elem = nextElement(&attrIter)) != NULL) {
        Attribute* tmpAttr = (Attribute*)elem;
        if (counter == (getLength((List*)list) - 1)) {
            backupBuffer = attrToJSON(tmpAttr);
            bytesNeeded = snprintf(NULL, 0, "%s%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, "]");
            free(backupBuffer);
        } else {
            backupBuffer = attrToJSON(tmpAttr);
            bytesNeeded = snprintf(NULL, 0, "%s,%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, ",");
            free(backupBuffer);
        }
        counter++;
    }
    return stringBuf;

}

char* circListToJSON(const List *list){

    int bytesNeeded = 0;
    char* stringBuf = NULL;
    char* backupBuffer = NULL;
    void* elem = NULL;
    int counter = 0;

    if ((List*)list == NULL || getFromFront((List*)list) == NULL) {
        stringBuf = malloc(4);
        sprintf(stringBuf, "[]");
        return stringBuf;
    }

    ListIterator circleIter = createIterator((List*)list);
    stringBuf = malloc(2);
    sprintf(stringBuf, "[");
    
    while((elem = nextElement(&circleIter)) != NULL) {
        Circle* tmpCircle = (Circle*)elem;
        if (counter == (getLength((List*)list) - 1)) {
            backupBuffer = circleToJSON(tmpCircle);
            bytesNeeded = snprintf(NULL, 0, "%s%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, "]");
            free(backupBuffer);
        } else {
            backupBuffer = circleToJSON(tmpCircle);
            bytesNeeded = snprintf(NULL, 0, "%s,%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, ",");
            free(backupBuffer);
        }
        counter++;
    }
    return stringBuf;
}

char* rectListToJSON(const List *list){

    int bytesNeeded = 0;
    char* stringBuf = NULL;
    char* backupBuffer = NULL;
    void* elem = NULL;
    int counter = 0;

    if ((List*)list == NULL || getFromFront((List*)list) == NULL) {
        stringBuf = malloc(4);
        sprintf(stringBuf, "[]");
        return stringBuf;
    }

    ListIterator rectIter = createIterator((List*)list);
    stringBuf = malloc(2);
    sprintf(stringBuf, "[");
    
    while((elem = nextElement(&rectIter)) != NULL) {
        Rectangle* tmpRect = (Rectangle*)elem;
        if (counter == (getLength((List*)list) - 1)) {
            backupBuffer = rectToJSON(tmpRect);
            bytesNeeded = snprintf(NULL, 0, "%s%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, "]");
            free(backupBuffer);
        } else {
            backupBuffer = rectToJSON(tmpRect);
            bytesNeeded = snprintf(NULL, 0, "%s,%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, ",");
            free(backupBuffer);
        }
        counter++;
    }
    return stringBuf;
}

char* pathListToJSON(const List *list){

    int bytesNeeded = 0;
    char* stringBuf = NULL;
    char* backupBuffer = NULL;
    void* elem = NULL;
    int counter = 0;

    if ((List*)list == NULL || getFromFront((List*)list) == NULL) {
        stringBuf = malloc(4);
        sprintf(stringBuf, "[]");
        return stringBuf;
    }

    ListIterator pathIter = createIterator((List*)list);
    stringBuf = malloc(2);
    sprintf(stringBuf, "[");
    
    while((elem = nextElement(&pathIter)) != NULL) {
        Path* tmpPath = (Path*)elem;
        backupBuffer = pathToJSON(tmpPath);
        if (counter == (getLength((List*)list) - 1)) {
            bytesNeeded = snprintf(NULL, 0, "%s%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, "]");
            free(backupBuffer);
        } else {
            bytesNeeded = snprintf(NULL, 0, "%s,%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, ",");
            free(backupBuffer);
        }
        counter++;
    }
    return stringBuf;
}

char* groupListToJSON(const List *list){


    int bytesNeeded = 0;
    char* stringBuf = NULL;
    char* backupBuffer = NULL;
    void* elem = NULL;
    int counter = 0;

    if ((List*)list == NULL || getFromFront((List*)list) == NULL) {
        stringBuf = malloc(4);
        sprintf(stringBuf, "[]");
        return stringBuf;
    }

    ListIterator groupIter = createIterator((List*)list);
    stringBuf = malloc(2);
    sprintf(stringBuf, "[");
    
    while((elem = nextElement(&groupIter)) != NULL) {
        Group* tmpGroup = (Group*)elem;
        if (counter == (getLength((List*)list) - 1)) {
            backupBuffer = groupToJSON(tmpGroup);
            bytesNeeded = snprintf(NULL, 0, "%s%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, "]");
            free(backupBuffer);
        } else {
            backupBuffer = groupToJSON(tmpGroup);
            bytesNeeded = snprintf(NULL, 0, "%s,%s", stringBuf, backupBuffer) + 1;
            stringBuf = realloc(stringBuf, bytesNeeded + 1);
            strcat(stringBuf, backupBuffer);
            strcat(stringBuf, ",");
            free(backupBuffer);
        }
        counter++;
    }
    return stringBuf;

}


char* SVGtoJSON(const SVGimage* imge, char* fileName){

    char* stringBuf = NULL;
    float fileSize = 0;
    int bytesNeeded = 0;
    int rectNum = 0;
    int circleNum = 0;
    int pathNum = 0;
    int groupNum = 0;
    List* rectList;
    List* circleList;
    List* pathList;
    List* groupList;
    if (imge == NULL) {
        stringBuf = malloc(sizeof(char) * 4);
        sprintf(stringBuf, "{}");
        return stringBuf;            
    }
    //Calculate fileSize.
    FILE *fp = fopen(fileName, "r");
    fseek(fp, 0L, SEEK_END);
    fileSize = ftell(fp)/1000;
    fclose(fp);

    rectList = getRects((SVGimage*) imge);
    circleList = getCircles((SVGimage*) imge);
    pathList = getPaths((SVGimage*) imge);
    groupList = getGroups((SVGimage*) imge);

    rectNum = getLength(rectList);
    circleNum = getLength(circleList);
    pathNum = getLength(pathList);
    groupNum = getLength(groupList);
    
    freeList(rectList);
    freeList(circleList);
    freeList(pathList);
    freeList(groupList);

    bytesNeeded = snprintf(NULL, 0, "{\"file\":\"%s\",\"size\":%f,\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", fileName, fileSize, rectNum, circleNum, pathNum, groupNum) + 1;
    stringBuf = malloc(bytesNeeded + 1);
    sprintf(stringBuf, "{\"file\":\"%s\",\"size\":%f,\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", fileName,fileSize, rectNum, circleNum, pathNum, groupNum);

    return stringBuf;
}


//FUNCTION VALIDATES SVG IMAGE AND RETURNS JSON STRING IF VALID.
char* imageToJSON(char* fileName) {

    SVGimage* image = createValidSVGimage(fileName, "parser/svg.xsd");

    if (image == NULL) {
        return NULL;
    }
    char* stringBuf = SVGtoJSON(image, fileName);
    return stringBuf;
}
