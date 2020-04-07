
/*
 * MIT License
 *
 * Copyright (c) 2018 Whuer_XiaoJie <1939346428@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <stdio.h>
#include <string.h>

int parse_xml_file(char *buf, int len) {
    xmlDocPtr doc;
    xmlNodePtr root, node, detail, test;
    xmlChar *name, *value;
    int count = 0;
    doc = xmlParseMemory(buf, len);  //parse xml in memory
    if (doc == NULL) {
        printf("doc == null\n");
        return -1;
    }
    root = xmlDocGetRootElement(doc);
    //fprintf(stderr, "%s():root->children.line=%d\n", __FUNCTION__,root->children->line);
    test = root->xmlChildrenNode;
    while (test != NULL) {
        if (xmlStrcasecmp(node->name, BAD_CAST "content") == 0)
            count++;
        test = test->next;
    }
    fprintf(stderr, "%s():root children conut is  %d\n", __FUNCTION__, count);
    for (node = root->children; node; node = node->next) {
        if (xmlStrcasecmp(node->name, BAD_CAST "content") == 0)
            break;
    }

    if (node == NULL) {
        printf("no node = content\n");
        return -1;
    }
    //int count=node->
    while (node != NULL) {
        for (node = node->children; node; node = node->next) {
            if (xmlStrcasecmp(node->name, BAD_CAST "pro") == 0) {  //get pro node
                name = xmlGetProp(node, BAD_CAST "id");
                value = xmlNodeGetContent(node);
                printf("this is %s:\n%s\n", (char *)name, (char *)value);  //get value, CDATA is not parse and don't take into value
                xmlFree(name);
                xmlFree(value);
            } else if (xmlStrcasecmp(node->name, BAD_CAST "details") == 0) {  //get details node

                test = node->children;
                count = 0;
                while (test != NULL) {
                    if (xmlStrcasecmp(node->name, BAD_CAST "detail") == 0)
                        count++;
                    test = test->next;
                }
                fprintf(stderr, "%s():detels count =%d\n", __FUNCTION__, count);
                for (detail = node->children; detail; detail = detail->next) {  //traverse detail node
                    if (xmlStrcasecmp(detail->name, BAD_CAST "detail") == 0) {
                        name = xmlGetProp(detail, BAD_CAST "name");
                        value = xmlNodeGetContent(detail);
                        if (strlen((char *)value) != 0) {
                            printf("%s : %s\n", (char *)name, (char *)value);
                        } else {
                            printf("%s has no value\n", (char *)name);
                        }
                        xmlFree(name);
                        xmlFree(value);
                    }
                }
            }
        }
        node = node->next;
    }
    xmlFreeDoc(doc);
    return 0;
}
int main(void) {
    char *content;
    unsigned long filesize;
    FILE *file;
    if ((file = fopen("testxml", "r")) == NULL) {
        perror("openf file error");
    }
    fseek(file, 0, SEEK_END);
    filesize = ftell(file);
    rewind(file);
    content = (char *)malloc(filesize + 1);
    memset(content, 0, filesize + 1);
    fread(content, 1, filesize, file);
    fclose(file);
    printf("content:\n%s\n", content);
    if (parse_xml_file(content, filesize) < 0) {
        perror("parse xml failed");
    }
    return 0;
}
