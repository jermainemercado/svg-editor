#include "SVGParser.h"
#include "ParserHelper.h"

SVGimage* createSVGimage(char* fileName) {

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *newImage = malloc(sizeof (SVGimage));
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
    printf("\nNamespace:\n\t%s\n",newImage->namespace);

    //Populate title, and description if there is any associated to the XML file.
    parseTree(root_element, newImage);
    
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return newImage;
}

void deleteSVGimage(SVGimage* img) {

    free(img);
}

char* SVGimageToString(SVGimage* img) {

    char *backupBuffer = NULL;
    char *returnedBuffer = NULL;
    int bytesNeeded = 0;

    bytesNeeded = snprintf(NULL,0,"\nNamespace:\n\t%s\n",img->namespace);
    returnedBuffer = malloc(bytesNeeded + 1);
    sprintf(returnedBuffer, "\nNamespace:\n\t%s\n", img->namespace);
    
    if (img->title[0] != '\0') {
        /*bytesNeeded += snprintf(NULL,0,"\nTitle:\n\t%s\n", img->title);
        backupBuffer = malloc(bytesNeeded+1);
        strcpy(backupBuffer, returnedBuffer);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nTitle:\n\t%s\n", img->title);
        strcat(returnedBuffer,backupBuffer);*/
        returnedBuffer = (char *)svgCat(returnedBuffer,"Title",img->title);
    }
    if (img->description[0] != '\0') {
        /*bytesNeeded += snprintf(NULL,0,"\nDescription:\n\t%s\n", img->description);
        backupBuffer = realloc(backupBuffer, bytesNeeded + 1);
        strcpy(backupBuffer, returnedBuffer);
        returnedBuffer = realloc(returnedBuffer, bytesNeeded + 1);
        sprintf(backupBuffer, "\nDescription:\n\t%s\n", img->description);
        strcat(returnedBuffer, backupBuffer); */
        returnedBuffer = (char *)svgCat(returnedBuffer,"Description",img->description);
    }
    if (backupBuffer != NULL){
        free(backupBuffer);
    }
    return returnedBuffer;
}