#define JSMN_PARENT_LINKS
#include <assert.h>
#include <curl/curl.h>
#include <errno.h>
#include <getopt.h>
#include <iconv.h>
#include <jsmn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARTICLES_PER_REQUEST 100
#define VERSION_NO "1.0.1"

#define LOG(...) FILE *f = fopen("/var/tmp/pocket-tool_log.txt", "a"); fprintf(f, __VA_ARGS__); fclose(f);
#define LOG_JSON(...) FILE *f = fopen("/var/tmp/pocket-tool_response.json", "a"); fprintf(f, __VA_ARGS__); fclose(f);

#define SNPRINTF(buf, size, offs, ...)\
    {\
        int res = snprintf(buf + offs, size - offs, __VA_ARGS__);\
        assert(res < size - offs);\
        offs += res;\
    }

typedef struct {
    char *memory;
    size_t size;
} mem_struct;

typedef struct {
    char *url;
    char *id;
    char *title;
    int num_tags;
    char **tags;
    int match; // Indicates whether article matched search criteria.
} article_t;

typedef enum {
    LIST,
    FAVORITE,
    UNFAVORITE,
    ARCHIVE,
    UNARCHIVE,
    DELETE
} action_t;

typedef enum {
    ARCHIVED,
    NOT_ARCHIVED,
    ALL
} state_t;

extern char *key;
extern char *token;
extern char *argv_0;

void    auth(char *, char *);
int     filter_all(article_t *, int, char **, int, char **, int);
char   *home(char *);
int     pack(jsmntok_t *, char *, article_t **);
int     pull(state_t, int, time_t, article_t **);
void    Curl_easy_perform(CURL *);
void    push(action_t, article_t *, int);
void    print_result(action_t, int, article_t *, int);
size_t  callback(void *, size_t, size_t, void *);

#ifdef TEST
void    test(void);
#endif
