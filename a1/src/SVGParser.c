#include "SVGParser.h"
#include "ParserHelper.h"

SVGimage* createSVGimage(char* fileName) {
    
    //Initial set up for SVGimage.
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *newImage = malloc(sizeof (SVGimage));
    newImage->otherAttributes = initializeList(&attributeToString, &deleteAttribute,&compareAttributes);
    newImage->rectangles = initializeList(&rectangleToString, &deleteRectangle,&compareRectangles);
    newImage->circles = initializeList(&circleToString, &deleteCircle,&compareCircles);
    newImage->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    newImage->title[0] = '\0';
    newImage->namespace[0] = '\0';
    newImage->description[0] = '\0';
    

    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        printf("\nError: Could not parse file %s\n", fileName);
    }

    root_element = xmlDocGetRootElement(doc);

    //Populate namespace.
    strcpy(newImage->namespace,(char *)root_element->ns->href);
    
    //Populate title, and description if there is any associated to the XML file.
    parseTree(root_element, newImage);
    //printf("\nAttribute Test:%s\n", newImage->otherAttributes->);
    
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return newImage;
}

void deleteSVGimage(SVGimage* img) {

    freeList(img->paths);
    freeList(img->circles);
    freeList(img->rectangles);
    freeList(img->otherAttributes);
    //free(img->otherAttributes);
    free(img);
}

char* SVGimageToString(SVGimage* img) {

    char *returnedBuffer = NULL;
    int bytesNeeded = 0;

    //Populating SVGimage* namespace.
    bytesNeeded = snprintf(NULL,0,"\n\nNamespace:\n\t%s\n",img->namespace);
    returnedBuffer = malloc(bytesNeeded + 1);
    sprintf(returnedBuffer, "\n\nNamespace:\n\t%s\n", img->namespace);
    
    if (img->title[0] != '\0') {
        returnedBuffer = (char *)svgCat(returnedBuffer,"Title",img->title);
    }
    if (img->description[0] != '\0') {
        returnedBuffer = (char *)svgCat(returnedBuffer,"Description",img->description);
    }
    return returnedBuffer;
}

void deleteAttribute( void* data) {

    Attribute* tempAttribute;

    if (data == NULL) {
        return;
    }
    
    /*tempAttribute = (Attribute*) data;
    free(tempAttribute->name);
    free(tempAttribute->value);
    free(tempAttribute);*/
    return;

}

char* attributeToString( void* data) {

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

    Attribute* firstAttr = NULL;
    Attribute* secondAttr = NULL;

    if (first == NULL || second == NULL) {
        return;
    }

    return strcmp(firstAttr->value,secondAttr->value);

}

void deleteRectangle(void* data){
    return;
}

char* rectangleToString(void* data) {
    return;
}

int compareRectangles(const void *first, const void *second) {
    return;
}

void deleteCircle(void* data){return;}
char* circleToString(void* data){return;}
int compareCircles(const void *first, const void *second){return;}
void deletePath(void* data){return;}
char* pathToString(void* data){return;}
int comparePaths(const void *first, const void *second){return;}