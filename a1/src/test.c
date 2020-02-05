#include "SVGParser.h"
#include "ParserHelper.h"

int main(int argc, char **argv) {
    
    SVGimage* cur_image;
    char* imgDescription = NULL;
    cur_image = createSVGimage(argv[1]);
    imgDescription = SVGimageToString(cur_image);
    printf("%s", imgDescription);
    free(imgDescription);
    deleteSVGimage(cur_image);

    return 0;
}