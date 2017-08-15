#include "main.h"

static int jsmn_strcmp(jsmntok_t token, char *str, char *json) {
    if (token.type != JSMN_STRING) {
        return -1;
    } else {
        json[token.end] = '\0';
        return strcmp(json + token.start, str);
    }
}

static char *consume_string(jsmntok_t token, char *json) {
    assert(token.type == JSMN_STRING);

    // Treat empty strings as non-existent.
    if (token.end == token.start)
        return NULL;

    json[token.end] = '\0';
    return strdup(json + token.start);
}

static char **consume_tags(jsmntok_t *tokens, char *json, int pi, int t_offs) {
    int i = t_offs;
    int ti = 0;
    char **tags = malloc(tokens[pi].size * sizeof(char *));
    while (tokens[i].end < tokens[pi].end) {
        if (tokens[i].parent == pi) {
            json[tokens[i].end] = '\0';
            tags[ti] = strdup(json + tokens[i].start);
            ti++;
        }
        i++;
    }
    return tags;
}

static article_t consume_article(jsmntok_t *tokens, char *json, int pi, int t_offs) {
    article_t a;
    a.url      = NULL;
    a.id       = NULL;
    a.title    = NULL;
    a.num_tags = 0;
    a.tags     = NULL;
    a.match    = 1;

    // Must skip items whose 'status' is 2.
    {
        int i = t_offs;
        while (tokens[i].end < tokens[pi].end) {
            if (tokens[i].parent == pi) {
                if (jsmn_strcmp(tokens[i], "status", json) == 0) {
                    assert(tokens[i].size = 1);
                    if (json[tokens[i+1].start] == '2')
                        return a;
                }
            }
            i++;
        }
    }

    {
        int i = t_offs;
        while (tokens[i].end < tokens[pi].end) {
            if (tokens[i].parent == pi) {
                if (jsmn_strcmp(tokens[i], "item_id", json) == 0)
                    a.id = consume_string(tokens[i+1], json);

                else if (jsmn_strcmp(tokens[i], "resolved_url", json) == 0)
                    a.url = consume_string(tokens[i+1], json);
                else if (jsmn_strcmp(tokens[i], "given_url", json) == 0 &&
                    a.url == NULL)
                    a.url = consume_string(tokens[i+1], json);

                else if (jsmn_strcmp(tokens[i], "resolved_title", json) == 0)
                    a.title = consume_string(tokens[i+1], json);
                else if (jsmn_strcmp(tokens[i], "given_title", json) == 0 &&
                    a.title == NULL)
                    a.title = consume_string(tokens[i+1], json);

                else if (jsmn_strcmp(tokens[i], "tags", json) == 0) {
                    a.num_tags = tokens[i+1].size;
                    a.tags = consume_tags(tokens, json, i+1, i+2);
                }
            }
            i++;
        }
    }

    return a;
}

static int consume_list(jsmntok_t *tokens, char *json, int pi, int t_offs,
    article_t **out) {
    int num_articles_max = tokens[pi].size;
    article_t *articles = malloc(num_articles_max * sizeof(article_t));

    int i = t_offs;
    int ai = 0;
    while (tokens[i].end < tokens[pi].end) {
        if (tokens[i].parent == pi) {
            assert(tokens[i].size == 1);
            article_t a = consume_article(tokens, json, i+1, i+2);
            if (a.id != NULL) {
                articles[ai] = a;
                ai++;
            }
        }
        i++;
    }
    *out = articles;
    return ai;
}

int pack(jsmntok_t *tokens, char *json, article_t **out) {
    int i = 0;
    while (jsmn_strcmp(tokens[i], "list", json) != 0)
        i++;
    return consume_list(tokens, json, i+1, i+2, out);
}
