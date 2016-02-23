/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE

#include <string.h>
#include "check.h"
#include "check_helper.h"


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
  printf( "%s", renderer( res) );
  // free (str);
  // free (res.returnmsg);
  return res;
}

ret_t tif_fails_tag(tag_t tag, const char* expected, const char* value) {
  ret_t res = tif_returns( tag, expected, value);
   // call renderer
  printf( "%s", renderer( res) );
  // free (str);
  // free (res.returnmsg);
  return res;
}

ret_t tif_rules_tag(tag_t tag, const char *msg) {
  //printf("DEBUG: %s -> %s\n", tag, msg);
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
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for 4tif_fails\n");
    exit(EXIT_FAILURE);
  };
  p->rm_msg = strndup( msg, MAXSTRLEN);
  printf( "%s", renderer( res) );
  // free (str);
  // free (res.returnmsg);
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
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for 4tif_fails\n");
    exit(EXIT_FAILURE);
  };
  p->rm_msg = strndup( msg, MAXSTRLEN);
  printf( "%s", renderer( res) );
  // free (str);
  // free (res.returnmsg);
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
  p->rm_msg = malloc( sizeof(char) *MAXSTRLEN );
  if (NULL==p->rm_msg) {
    fprintf(stderr, "could not allocate memory for 4tif_fails\n");
    exit(EXIT_FAILURE);
  };
  p->rm_msg = strndup( msg, MAXSTRLEN);
  printf( "%s", renderer( res) );
  // free (str);
  // free (res.returnmsg);
  return res;
}

void tifp_check( TIFF * tif) {
  if (NULL == tif) { tif_fails("TIFF pointer is empty\n"); };
}

/* TODO: add tif_returns specialized for types */

ret_t tif_returns(tag_t tag, const char* expected, const char* value) {
  ret_t res;
  /*
     char * str =malloc( sizeof(char) *MAXSTRLEN );
     if (NULL==str) {
     fprintf(stderr, "could not allocate memory for tif_fails\n");
     exit(EXIT_FAILURE);
     };
     snprintf (str, MAXSTRLEN-1, "FAILTAG:tag >>%s<< should have value >>%s<<, but has count/value >>%s<<\n", tag, expected, value);
     */
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
 printf( "%s", renderer( ret) );
 return ret;
}


