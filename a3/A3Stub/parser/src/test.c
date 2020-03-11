#include "SVGParser.h"
#include "ParserHelper.h"

int main(int argc, char **argv) {
    
    SVGimage* cur_image;
    char* imgDescription = NULL;
    List* pathList = NULL;
    printf("%s\n",argv[1]);
    cur_image = createValidSVGimage(argv[1], "svg.xsd");
    //imgDescription = SVGimageToString(cur_image);
    //printf("%s", imgDescription);
    //free(imgDescription);
    writeSVGimage(cur_image, "test.svg");
    pathList = getPaths(cur_image);
    imgDescription = pathListToJSON(pathList);
    printf("%s", imgDescription);
    //Attribute* newAttr = malloc(sizeof(Attribute));
    //setAttribute(image, RECT, 0, )
    freeList(pathList);
    free(imgDescription);
    imgDescription = SVGtoJSON(cur_image);
    printf("%s", imgDescription);
    free(imgDescription);
    deleteSVGimage(cur_image);

    xmlCleanupParser();
    xmlMemoryDump();
    printf("\n\nhello\n\n");
    return 0;
}