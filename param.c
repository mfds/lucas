#include <libxml/parser.h>
#include <libxml/tree.h>

#include "struct.h"
#include "param.h"

static struct lucas_ca
*parse_ca(uint8_t nr, xmlDocPtr doc, xmlNodePtr cur)
{
  struct lucas_ca *ca = NULL;
  
  int ok = 1;
  
  int tmp_int;
  long int tmp_lint;

  xmlChar *s_rule = NULL;
  xmlChar *s_size = NULL;
  xmlChar *s_moves = NULL;
  xmlChar *s_pattern = NULL;

  uint8_t rule;
  uint16_t size;
  uint32_t moves;
  uint8_t pattern;

  cur = cur->xmlChildrenNode;
  
  while (cur != NULL) {
    if (!s_rule && !xmlStrcmp(cur->name, (const xmlChar *) "rule")) {
      s_rule = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      if (s_rule == NULL) { 
	fprintf(stderr, "WARNING: [%d] undefined value for rule\n", nr);
	break;
      }
    }
    if (!s_size && !xmlStrcmp(cur->name, (const xmlChar *) "size")) {
      s_size = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      if (s_size == NULL) { 
	fprintf(stderr, "WARNING: [%d] undefined value for size\n", nr);
	break;
      }
    }
    if (!s_moves && !xmlStrcmp(cur->name, (const xmlChar *) "moves")) {
      s_moves = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      if (s_moves == NULL) { 
	fprintf(stderr, "WARNING: [%d] undefined value for moves\n", nr);
	break;
      }
    }
    if (!s_pattern && !xmlStrcmp(cur->name, (const xmlChar *) "pattern")) {
      s_pattern = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
      if (s_pattern == NULL) { 
	fprintf(stderr, "WARNING: [%d] undefined value for pattern\n", nr);
	break;
      }
    }

    cur = cur->next;
  }

  if (s_rule && s_moves && s_size) {
    tmp_int = atoi((char *) s_rule);
    if (tmp_int >= 0 && tmp_int <= 255)
      rule = (uint8_t) tmp_int;
    else {
      fprintf(stderr, "WARNING: [%d] Invalid rule. Must be 0 <= rule <= 255\n", nr);
      ok = 0;
    }
    
    tmp_int = atoi((char *) s_size);
    if (tmp_int > 0)
      size = (uint16_t) tmp_int;
    else {
      fprintf(stderr, "WARNING: [%d] Invalid size. Must be > 0\n", nr);
      ok = 0;
    }

    tmp_lint = atol((char *) s_moves);
    if (tmp_lint >= 0)
      moves = (uint32_t) tmp_lint;
    else {
      fprintf(stderr, "WARNING: [%d] Invalid moves. Must be > 0\n", nr);
      ok = 0;
    }

    if (!xmlStrcmp(s_pattern, (const xmlChar *) "none"))
      pattern = NONE;
    else if (!xmlStrcmp(s_pattern, (const xmlChar *) "one-central"))
      pattern = ONE_CENTRAL;
    else if (!xmlStrcmp(s_pattern, (const xmlChar *) "alternate"))
      pattern = ALTERNATE;
    else if (!xmlStrcmp(s_pattern, (const xmlChar *) "random"))
      pattern = RANDOM;
    else if (!xmlStrcmp(s_pattern, (const xmlChar *) "all"))
      pattern = ALL;
    else {
      fprintf(stderr, "WARNING: [%d] Unknown pattern. Choices are "
	      "'one-central' - 'random' - 'none' - 'all'\n", nr);
      ok = 0;
    }
    
    /* Creating CA */
    if (ok)
      ca = init_ca(rule, size, moves, pattern);
  
    xmlFree(s_rule);
    xmlFree(s_size);
    xmlFree(s_moves);
    xmlFree(s_pattern);
  }

  if (ca == NULL)
    fprintf(stderr, "WARNING: CA %d is not valid, rejected\n", nr);

  return ca;
  
}
 
struct params
*init_params(char *xml_file)
{
  struct params *pm;
  struct lucas_ca *cur_ca;

  xmlDocPtr doc;
  xmlNodePtr cur;

  int nr = 0;

  LIBXML_TEST_VERSION /* what's this? */

#ifdef LIBXML_SAX1_ENABLED
    doc = xmlParseFile(xml_file);
  if (doc == NULL) {
    fprintf(stderr, "ERROR: could not read xml file\n");
    return(NULL);
  }
#else
  fprintf(stderr, "ERROR: libxml compiled without SAX1 support\n");
  return(NULL);
#endif /* LIBXML_SAX1_ENABLED */
  
  cur = xmlDocGetRootElement(doc);
  if (cur == NULL) {
    fprintf(stderr,"ERROR: empty document\n");
    xmlFreeDoc(doc);
    return(NULL);
  }
  
  if (xmlStrcmp(cur->name, (const xmlChar *) PROG_NAME)) {
    fprintf(stderr,"ERROR: document of the wrong type, root node != %s",
	    PROG_NAME);
    xmlFreeDoc(doc);
    return(NULL);
  }
  
  pm = calloc(1, sizeof(struct params));
  
  if (pm == NULL) {
    fprintf(stderr,"ERROR: out of memory\n");
    xmlFreeDoc(doc);
    return(NULL);
  }

  /* walking xml */
  
  cur = cur->xmlChildrenNode;
  while (cur && xmlIsBlankNode(cur)) {
    cur = cur -> next;
  }
  
  if (cur == 0) {
    xmlFreeDoc(doc);
    /*destroy_params(params);*/
    printf("errore cur 0");
    return (NULL);
  }

  if (xmlStrcmp(cur->name, (const xmlChar *) "out")) {
    fprintf(stderr,"Error: document of the wrong type, was '%s'"
	    ", out expected\n", cur->name);
#ifdef LIBXML_OUTPUT_ENABLED
    fprintf(stderr,"xmlDocDump follows\n");
    xmlDocDump ( stderr, doc );
    fprintf(stderr,"xmlDocDump finished\n");
#endif /* LIBXML_OUTPUT_ENABLED */
    xmlFreeDoc(doc);
    /*destroy_params(params);*/
    
    return(NULL);
  }
  
  /* Second level is a list of Job, but be laxist */
  cur = cur->next;
  /* xmlChildrenNode; */ 
  
  while (cur != NULL) {
    if (!xmlStrcmp(cur->name, (const xmlChar *) "ca")) {
      cur_ca = parse_ca(++nr, doc, cur);
      
      if (cur_ca != NULL) {
	pm->cas = concat_ca(cur_ca, pm->cas);
	pm->nr_cas++;
      }
      
      if (pm->nr_cas >= 4)
	break;
    }
    
    cur = cur->next;
  }
 
  xmlFreeDoc(doc);
 
  if (pm->nr_cas > 0)
    return pm;
  else
    return NULL;
  
  /* clean up! */
}

void
destroy_params(struct params *pm)
{
  if (pm != NULL) {
    destroy_ca(pm->cas);
    
    free(pm);
  }
}

