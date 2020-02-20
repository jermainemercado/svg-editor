#include "SVGParser.h"
#include "ParserHelper.h"

int main(int argc, char **argv) {
    
    SVGimage* cur_image;
    //char* imgDescription = NULL;
    printf("%s\n",argv[1]);
    cur_image = createValidSVGimage(argv[1], "svg.xsd");
    //imgDescription = SVGimageToString(cur_image);
    //printf("%s", imgDescription);
    //free(imgDescription);
    deleteSVGimage(cur_image);
    xmlCleanupParser();
    xmlMemoryDump();
    return 0;
}