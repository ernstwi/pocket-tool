#include "main.h"

static void print_without_backslashes(char *cp) {
    while (*cp) {
        if (*cp != '\\')
            putchar(*cp);
        cp++;
    }
}

void print_result(action_t action, int mod_articles, article_t *articles,
    int num_articles, int quiet, int markdown) {
    char *action_str;
    switch (action) {
        case FAVORITE:
            action_str = "Favorited ";
            break;
        case UNFAVORITE:
            action_str = "Unfavorited ";
            break;
        case ARCHIVE:
            action_str = "Archived ";
            break;
        case UNARCHIVE:
            action_str = "Unarchived ";
            break;
        case DELETE:
            action_str = "Deleted ";
            break;
        case LIST:
            action_str = "";
            break;
        default:
            assert(!"'action' must match one of the previous cases.");
            // This assertion will always fail. As suggested by
            // http://blogs.operationaldynamics.com/pmiller/sw/making-assert0
            // -more-informative.
    }
    printf("%s%d %s%c\n", action_str, mod_articles,
        mod_articles == 1 ? "article" : "articles",
        quiet || mod_articles == 0 ? '.' : ':');

    if (quiet || mod_articles == 0)
        return;

    // I use iconv to turn so called universal character names like "\u2019"
    // into their corresponding Unicode code points encoded in UTF-8.
    iconv_t cd = iconv_open("UTF-8", "C99");

    char *inbuf;      // ← Will always point to first byte of input string.
    char *inbuf_end;  // ← Will after conversion point to the byte after the last
    size_t inbuf_len; //   byte used in the conversion.

    char *outbuf;
    char *outbuf_end;
    size_t outbuf_len;

    for (int i = 0; i < num_articles; i++) {
        if (!articles[i].match)
            continue;

        printf("* ");
        // If there's no title, print just the URL.
        if (articles[i].title == NULL) {
            if (markdown) {
                printf("[");
                print_without_backslashes(articles[i].url);
                printf("](");
                print_without_backslashes(articles[i].url);
                printf(")\n");
            } else {
                print_without_backslashes(articles[i].url);
                printf("\n");
            }
            continue;
        }

        inbuf = articles[i].title;
        inbuf_len = strlen(inbuf);

        outbuf_len = inbuf_len * 4; // A UTF-8 encoded code point is at most
        // 4 bytes in size, and each character (byte) in the input string
        // equals at most one code point.
        outbuf = malloc(outbuf_len + 1); // Add one for termination byte.

        inbuf_end = inbuf;
        outbuf_end = outbuf;

        iconv(cd, &inbuf_end, &inbuf_len, &outbuf_end, &outbuf_len);
        assert(inbuf_len == 0); // There are no bytes left to convert.
        *outbuf_end = '\0';

        if (markdown) {
            printf("[");
            print_without_backslashes(outbuf);
            printf("](");
            print_without_backslashes(articles[i].url);
            printf(")\n");
        } else {
            print_without_backslashes(outbuf);
            printf("\n");
        }

        free(outbuf);
    }
    iconv_close(cd);
}
