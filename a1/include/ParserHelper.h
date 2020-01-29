#ifndef PARSERHELPER_H
#define PARSERHELPER_H

#include "SVGParser.h"
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>




void parseSVGotherAttributes(xmlNode* root_node, SVGimage* current_img);
void parseTree(xmlNode* xml_node, SVGimage* current_img);
void parseAttributes(xmlNode* xml_node, SVGimage* current_img);
char* svgCat(char* stringToModify, char* nameOfElement, char* stringToAppend);
char* svgAttrToString(SVGimage* current_img);
Rectangle* rectangleParser(xmlNode* xml_node);
Circle* circleParser(xmlNode* xml_node);
Path* pathParser(xmlNode* xml_node);
Group* groupParser(xmlNode* xml_node);
char* circleAttrToString(Circle* circleToPrint);
char* rectAttrToString(Rectangle* rectangleToPrint);
char* pathAttrToString(Path* pathToPrint);
char* groupAttrToString(Group* groupToPrint);
void deleteStub();
void groupGetter(Group* group, List* groupList);
void getPathsHelper(Group* group, List* pathList);
void getCirclesHelper(Group* group, List* circleList);
void getRectsHelper(Group* group, List* rectList);

#endif