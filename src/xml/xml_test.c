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
#include <stdio.h>
#include <string.h>

static int xml_get_next_count(xmlNodePtr pCount) {
    int count = 0;
    while (pCount != NULL) {
        if (xmlStrcmp(pCount->name, (const xmlChar *)"detail") == 0) {
            count++;
        }
        pCount = pCount->next;
    }
    fprintf(stderr, "%s():details count = %d \n", __FUNCTION__, count);
    return 0;
__outfailed:
    return -1;
}
static int xml_get_next(xmlDocPtr doc, xmlNodePtr cur) {
    xmlNodePtr pNode = NULL;
    xmlNodePtr pCount = NULL;

    xmlChar *name = NULL;
    xmlChar *attr_value = NULL;
    char s1[1024];
    name = xmlGetProp(cur, (const xmlChar *)"name");
    if (name != NULL) {
        //strcpy(s1, (const char*)name);
        int len = xmlStrlen(name);
        int i;
        int j = 0;
        for (i = 0; i < len; i++) {
            s1[j++] = name[i];
        }
        s1[j] = '\0';
        fprintf(stderr, "%s():s1_len=%d   s[2]=%s\n", __FUNCTION__, strlen(s1), s1[2]);
        fprintf(stderr, "%s():the content name is  %s \n", __FUNCTION__, name);
        xmlFree(name);
        name = NULL;
    }
    pNode = cur->xmlChildrenNode;
    pCount = cur->xmlChildrenNode;
    if (xml_get_next_count(pCount) != 0) {
        fprintf(stderr, "%s():get the details count failed \n", __FUNCTION__);
        goto __outfailed;
    }
    while (pNode != NULL) {
        if (xmlStrcmp(pNode->name, (const xmlChar *)"detail") == 0) {
            name = xmlGetProp(pNode, (const xmlChar *)"name");
            if (name != NULL) {
                fprintf(stderr, "%s():the detail name :%s\n", __FUNCTION__, name);
                xmlFree(name);
                name = NULL;
            }
            attr_value = xmlNodeGetContent(pNode);
            if (attr_value != NULL) {
                fprintf(stderr, "%s():the detel value is:%s\n", __FUNCTION__, attr_value);
                xmlFree(attr_value);
                attr_value = NULL;
            }
        }
        pNode = pNode->next;
    }
    return 0;
__outfailed:
    return -1;
}
static int xml_get_first_count(xmlNodePtr pCount) {
    int count = 0;
    while (pCount != NULL) {
        if (xmlStrcmp(pCount->name, (const xmlChar *)"details") == 0) {
            count++;
        }
        pCount = pCount->next;
    }
    fprintf(stderr, "%s():details count = %d \n", __FUNCTION__, count);
    return 0;
__outfailed:
    return -1;
}
static int xml_get_first(xmlDocPtr doc, xmlNodePtr cur) {
    xmlNodePtr pNode = NULL;
    xmlNodePtr pCount = NULL;

    xmlChar *name = NULL;
    xmlChar *attr_value = NULL;
    name = xmlGetProp(cur, (const xmlChar *)"name");
    if (name != NULL) {
        fprintf(stderr, "%s():the content name is  %s \n", __FUNCTION__, name);
        xmlFree(name);
        name = NULL;
    }
    pNode = cur->xmlChildrenNode;
    pCount = cur->xmlChildrenNode;
    if (xml_get_first_count(pCount) != 0) {
        fprintf(stderr, "%s():get the details count failed \n", __FUNCTION__);
        goto __outfailed;
    }
    while (pNode != NULL) {
        if (xmlStrcmp(pNode->name, (const xmlChar *)"pro") == 0) {
            name = xmlGetProp(pNode, (const xmlChar *)"id");
            if (name != NULL) {
                fprintf(stderr, "%s():the pro id is :%s\n", __FUNCTION__, name);
                xmlFree(name);
                name = NULL;
            }
            attr_value = xmlNodeGetContent(pNode);
            if (attr_value != NULL) {
                fprintf(stderr, "%s():the pro value is:%s\n", __FUNCTION__, attr_value);
                xmlFree(attr_value);
                attr_value = NULL;
            }
        } else if (xmlStrcmp(pNode->name, (const xmlChar *)"details") == 0) {
            if (xml_get_next(doc, pNode) != 0) {
                fprintf(stderr, "%s():get the detelis failed \n", __FUNCTION__);
                goto __outfailed;
            }
        }
        pNode = pNode->next;
    }
    return 0;
__outfailed:
    return -1;
}
static int xml_get_root_children_count(xmlDocPtr pDoc, xmlNodePtr pCount) {
    int count = 0;
    fprintf(stderr, "%s():pCount->name=%s\n", __FUNCTION__, pCount->name);
    while (pCount != NULL) {
        if (xmlStrcmp(pCount->name, (const xmlChar *)"content") == 0) {
            count++;
        }
        pCount = pCount->next;
    }
    fprintf(stderr, "%s():content count = %d \n", __FUNCTION__, count);
    return 0;
__outfailed:
    return -1;
}
static int xml_get_root(char *fileName) {
    xmlDocPtr pDoc = NULL;
    xmlNodePtr pRoot = NULL;
    xmlNodePtr pNode = NULL;
    xmlNodePtr pCount = NULL;

    xmlChar *type = NULL;
    xmlKeepBlanksDefault(0);       /* ignore the blank text symbol */
    pDoc = xmlParseFile(fileName); /* get the xml file tree structure */
    if (pDoc == NULL) {
        fprintf(stderr, "%s():parse xml file failed \n", __FUNCTION__);
        goto __outfailed;
    }
    pRoot = xmlDocGetRootElement(pDoc); /* get the root element */
    if (pRoot == NULL) {
        fprintf(stderr, "%s():get root element failed \n", __FUNCTION__);
        goto __outfailed;
    }
    if ((xmlStrcmp(pRoot->name, (const xmlChar *)"root")) != 0) /* the root element not PlayProject */
    {
        fprintf(stderr, "%s():the root element not root  is :%s \n", __FUNCTION__, pRoot->name);
        goto __outfailed;
    }

    pCount = pRoot->xmlChildrenNode;
    pNode = pRoot->xmlChildrenNode;
    if (xml_get_root_children_count(pDoc, pCount) != 0) {
        fprintf(stderr, "%s():get root children count failed \n", __FUNCTION__);
        goto __outfailed;
    }
    while (pNode != NULL) {
        if (xmlStrcmp(pNode->name, (const xmlChar *)"content") == 0) {
            if (xml_get_first(pDoc, pNode) != 0) {
                fprintf(stderr, "%s():get content children failed\n", __FUNCTION__);
                goto __outfailed;
            }
        }
        pNode = pNode->next;
    }
    if (pDoc != NULL) {
        xmlFreeDoc(pDoc);
        pDoc = NULL;
    }
    return 0;
__outfailed:
    if (pDoc != NULL) {
        xmlFreeDoc(pDoc);
        pDoc = NULL;
    }
    return -1;
}
int main() {
    char *filename = "testxml";
    if (xml_get_root(filename) != 0) {
        fprintf(stderr, "%s():get xml failed \n", __FUNCTION__);
        return -1;
    }
    return 0;
}