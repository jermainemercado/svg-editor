#ifndef PARSERHELPER_H
#define PARSERHELPER_H

#include "SVGParser.h"
#include <string.h>
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
//ASSIGNMENT 2
//MODULE 1
xmlDocPtr convertSVGimage(SVGimage* image);
void imgOtherToXml(List* imageAttributes, xmlNodePtr root_node);
void imgRectToXml(List* imageRectangles, xmlNodePtr root_node);
void imgCircleToXml(List* imageCircles, xmlNodePtr root_node);
void imgPathToXml(List* imagePaths, xmlNodePtr root_node);
void imgGroupToXml(List* imageGroups, xmlNodePtr root_node);
bool validateHelper(SVGimage* image);
bool validateAttrs(List* otherAttributes);
bool validateRect(Rectangle* imgRect);
bool validateCircle(Circle* imgCircle);
bool validatePath(Path* imgPath);
bool validateGroup(Group* imgGroup);
//MODULE 2
void setOtherAttr(List* otherAttributes, Attribute* newAttribute);
void setRectAttr(SVGimage* image, Attribute* newAttribute, int elemIndex);
void setCircleAttr(SVGimage* image, Attribute* newAttribute, int elemIndex);
void setPathAttr(SVGimage* image, Attribute* newAttribute, int elemIndex);
void setGroupAttr(SVGimage* image, Attribute* newAttribute, int elemIndex);

#endif