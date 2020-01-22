#include "SVGParser.h"
#include "ParserHelper.h"

SVGimage* createSVGimage(char* fileName) {

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    SVGimage *newImage = malloc(sizeof (SVGimage));

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

    if(newImage->title == NULL) {
        newImage->title[0] = (char *) calloc(0,1);
    }
    if(newImage->description == NULL) {
        newImage->description[0] = (char *) calloc(0,1);
    }
    
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return newImage;
}

void deleteSVGimage(SVGimage* img) {

    free(img->title);
    free(img->description);
    free(img->namespace);
    free(img);

}


//char* SVGimageToString(SVGimage* img) {