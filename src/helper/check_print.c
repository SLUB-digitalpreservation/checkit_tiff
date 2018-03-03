/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see file LICENSE)
 *
 */
#define DEBUG
#define _GNU_SOURCE

#include <assert.h>
#include <string.h>
#include "check.h"
#include "check_helper.h"

#ifdef WIN32
static inline char *strndup(const char *s, size_t n)
{
    char *result;
    size_t len = strlen (s);
    if (n < len) len = n;
    result = (char *) malloc (len + 1);
    if (!result) return 0;
    result[len] = '\0';
    return (char *) strncpy (result, s, len);
}
#endif

returncode_t __add_to_render_pipeline_via_strncpy (retmsg_t ** pointer, const char * src, rm_type_t src_type) {
   assert(pointer != NULL);
   retmsg_t * actual_render = NULL;
   actual_render = *pointer;
   assert(actual_render != NULL);
   assert(src != NULL);
   assert(actual_render->next==NULL);
   assert(actual_render->rm_msg != NULL);
   actual_render->next = malloc ( sizeof(retmsg_t));
   if (NULL == actual_render->next) {
     exit( could_not_allocate_memory);
     // return could_not_allocate_memory;
   }
   /*
   actual_render->next->rm_msg = malloc ( sizeof(char) * VALUESTRLEN );
   if (NULL == actual_render->next->rm_msg) {
     exit (could_not_allocate_memory);
     // return could_not_allocate_memory;
   }
   memset( actual_render->next->rm_msg, '\0', VALUESTRLEN);
   */
   actual_render->next->rm_msg = calloc (VALUESTRLEN, sizeof(char) );
   if (NULL == actual_render->next->rm_msg) {
     exit (could_not_allocate_memory);
     // return could_not_allocate_memory;
   }

   strncpy(actual_render->next->rm_msg, src, VALUESTRLEN-1 );
   actual_render->next->rm_type = src_type;
   // fprintf(stderr, "rendertype=%i rendermsg='%s'\n",actual_render->next->rm_type, actual_render->next->rm_msg );
   actual_render = actual_render->next;
   assert(actual_render != NULL);
   assert(actual_render->rm_msg != NULL);
   actual_render->next=NULL;
   *pointer = actual_render;
   return is_valid;
}

void __clean_render_pipeline( retmsg_t * pointer ) {
   assert(pointer != NULL);
   retmsg_t * next = pointer->next;
   if (NULL != pointer->rm_msg) {
     free( pointer->rm_msg);
     pointer->rm_msg = NULL;
   }
   if (NULL != next)
     __clean_render_pipeline( next );
   free( pointer );
   pointer = NULL;
}


/* 
ret_t tif_fails(const char* fail_message) {
  ret_t res;
  char * str =malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==str) {
    fprintf(stderr, "could not allocate memory for tif_fails\n");
    exit(EXIT_FAILURE);
  };
  snprintf (str, MAXSTRLEN-1, "%s", fail_message);
  res.returnmsg = NULL;
  res.returncode=1;
  res.returnmsg = malloc( sizeof( retmsg_t ) );
  if  (NULL==res.returnmsg) {
    fprintf(stderr, "could not allocate memory for tif_fails\n");
    exit(EXIT_FAILURE);
  };
  res.returnmsg->rm_type=rm_hard_error;
  res.count = 1;
  res.returnmsg->rm_msg = str;
  return res;
}

ret_t tif_fails_tag(tag_t tag, const char* expected, const char* value) {
  ret_t res = tif_returns( tag, expected, value);
  return res;
}

ret_t tif_rules_tag(tag_t tag, const char *msg) {
  ret_t res;
  res.returnmsg = NULL;
  res.returncode=0;
  res.count = 3;
  res.returnmsg = malloc( sizeof( retmsg_t ) * res.count  );
  if  (NULL==res.returnmsg) {
    fprintf(stderr, "could not allocate memory for tif_rules_tag\n");
    exit(EXIT_FAILURE);
  };
  retmsg_t * p =  res.returnmsg;
  // header
  p->rm_type=rm_rule;
  p->rm_msg = "check if ";
  // tag
  p++;
  p->rm_type=rm_tag;
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for 3tif_fails\n");
    exit(EXIT_FAILURE);
  };
  snprintf (p->rm_msg, MAXSTRLEN-1, "tag %s ", tag2str(tag));
  // rule
  p++;
  p->rm_type=rm_rule;
  p->rm_msg = strndup( msg, MAXSTRLEN);
  return res;
}

ret_t tif_no_tag(tag_t tag) {
  ret_t res;
  res.returnmsg = NULL;
  res.returncode=1;
  res.count = 4;
  res.returnmsg = malloc( sizeof( retmsg_t ) * res.count  );
  if  (NULL==res.returnmsg) {
    fprintf(stderr, "could not allocate memory for tif_rules_tag\n");
    exit(EXIT_FAILURE);
  };
  retmsg_t * p =  res.returnmsg;
  // header
  p->rm_type=rm_error;
  p->rm_msg = "";
  // tag
  p++;
  p->rm_type=rm_tag;
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for 3tif_fails\n");
    exit(EXIT_FAILURE);
  };
  snprintf (p->rm_msg, MAXSTRLEN-1, "tag %s ", tag2str(tag));
  // expected
  p++;
  p->rm_type=rm_expected;
  p->rm_msg = "is found";
  // value
  p++;
  p->rm_type=rm_value;
  p->rm_msg = ", but is not whitelisted (or rule has no matched dependency)";
  return res;
}

ret_t tif_rules(const char *msg) {
  ret_t res;
  res.returnmsg = NULL;
  res.returncode=0;
  res.count = 2;
  res.returnmsg = malloc( sizeof( retmsg_t ) * res.count  );
  if  (NULL==res.returnmsg) {
    fprintf(stderr, "could not allocate memory for tif_rules_tag\n");
    exit(EXIT_FAILURE);
  };
  retmsg_t * p =  res.returnmsg;
  // header
  p->rm_type=rm_default;
  p->rm_msg = "check if ";
  // rule
  p++;
  p->rm_type=rm_rule;
  p->rm_msg = strndup( msg, MAXSTRLEN);
  return res;
}
ret_t tif_files(const char *msg) {
  ret_t res;
  res.returnmsg = NULL;
  res.returncode=0;
  res.count = 2;
  res.returnmsg = malloc( sizeof( retmsg_t ) * res.count  );
  if  (NULL==res.returnmsg) {
    fprintf(stderr, "could not allocate memory for tif_rules_tag\n");
    exit(EXIT_FAILURE);
  };
  retmsg_t * p =  res.returnmsg;
  // header
  p->rm_type=rm_default;
  p->rm_msg = "tiff file=";
  // rule
  p++;
  p->rm_type=rm_file;
  p->rm_msg = strndup( msg, MAXSTRLEN);
  // printf( "%s", renderer( res) );
  return res;
}


ret_t tif_returns(tag_t tag, const char* expected, const char* value) {
  ret_t res;
  res.returnmsg = NULL;
  res.returncode=1;
  res.count = 4;
  res.returnmsg = malloc( sizeof( retmsg_t) * res.count );
  retmsg_t * p =  res.returnmsg;
  if  (NULL==res.returnmsg) {
    fprintf(stderr, "could not allocate memory for tif_fails\n");
    exit(EXIT_FAILURE);
  };
  // header
  p->rm_type=rm_error;
  p->rm_msg = "";
  // tag
  p++;
  p->rm_type=rm_tag;
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for tif_fails\n");
    exit(EXIT_FAILURE);
  };
  snprintf (p->rm_msg, MAXSTRLEN-1, "tag %s", tag2str( tag));
  // expected
  p++;
  p->rm_type=rm_expected;
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for tif_fails\n");
    exit(EXIT_FAILURE);
  };
  snprintf (p->rm_msg, MAXSTRLEN-1, " should have value %s", expected);
  // value
  p++;
  p->rm_type=rm_value;
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for tif_fails\n");
    exit(EXIT_FAILURE);
  };
  snprintf (p->rm_msg, MAXSTRLEN-1, ", but has value (values or count) %s", value);
  return res;
}

ret_t tif_fails_by_returns( ret_t ret ) {
  return ret;
}

ret_t _empty_result() {
  ret_t t;
  t.returncode=is_valid;
  t.returnmsg=NULL;
  t.returnmsg = malloc( sizeof( retmsg_t) * t.count );
  retmsg_t * p =  t.returnmsg;
  if  (NULL==t.returnmsg) {
    fprintf(stderr, "could not allocate memory for _empty_result\n");
    exit(EXIT_FAILURE);
  };
  return t;
}
*/
returncode_t tifp_check( ctiff_t * ctif) {
  if (NULL == ctif) { return code_error_ctif_empty; };
  if (0 > ctif->fd) { return code_error_filedescriptor_empty; };
  if (NULL == ctif->streamp) { return code_error_streampointer_empty; };
  return should_not_occur;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
