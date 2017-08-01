#include "main.h"

// 0 = didn't pass
// 1 = passed
static int filter(article_t a, char **with, int with_len, char **without, int without_len) {
    // Do not let this article through if it has already been excluded.
    if (a.match == 0)
        return 0;

    // without
    for (int i = 0; i < without_len; i++) {
        for (int j = 0; j < a.num_tags; j++) {
            if (strcmp(without[i], a.tags[j]) == 0)
                return 0;
        }
    }

    // with
    int has[with_len];
    for (int i = 0; i < with_len; i++) {
        has[i] = 0;
        for (int j = 0; j < a.num_tags; j++) {
            if (strcmp(with[i], a.tags[j]) == 0) {
                has[i] = 1;
            }
            // TODO: Optimization ↓
            // if (has[i])
            //     break;
        }
    }

    for (int i = 0; i < with_len; i++) {
        if (!has[i])
            return 0;
    }

    return 1;
}

int filter_all(article_t *articles, int a_len, char **with, int with_len,
    char **without, int without_len) {
    int *include = malloc(a_len * sizeof(int));
    int passed_filter = 0;
    for (int i = 0; i < a_len; i++) {
        articles[i].match =  filter(articles[i], with, with_len, without, without_len);
        if (articles[i].match)
            passed_filter++;
    }
    return passed_filter;
}
