#include "main.h"
#include "usage.h"

// Global to entire program
char *key;
char *token;
char *argv_0;

// Global to this file
action_t action;
state_t state;
int favorited = -1;
char **with_tags = NULL;
char **without_tags  = NULL;
int  with_tags_len = 0;
int  without_tags_len = 0;
time_t before_date = 0;
time_t since_date = 0;
int quiet = 0;

static void read_key_token(void) {
    char *abs_path = home(".pocket-tool");
    FILE *f = fopen(abs_path, "r");
    if (f == NULL) {
        fprintf(stderr, "%1$s: \x1b[31mError\x1b[0m: ~/.pocket-tool does not exist\n"
            "You must get a consumer key and run '%1$s auth <consumer-key>' before\n"
            "using any other command.\n", argv_0);
        exit(1);
    }
    free(abs_path);

    int key_len = 0;
    int token_len = 0;
    int newline_scanned = 0;

    {
        char c;
        while ((c = getc(f)) != EOF) {
            if (c == '\n')
                newline_scanned = 1;
            else if (!newline_scanned)
                key_len++;
            else
                token_len++;
        }
    }

    rewind(f); // Start reading from beginning of file again
    key = malloc(key_len + 1);
    token = malloc(token_len + 1);
    assert(fscanf(f, "%s\n%s\n", key, token) == 2);
}

static void usage(FILE *f, char *name) {
    char usage_txt_nul_term[usage_txt_len + 1];
    memcpy(usage_txt_nul_term, usage_txt, usage_txt_len);
    usage_txt_nul_term[usage_txt_len] = '\0';
    fprintf(f, usage_txt_nul_term, name);
}

static void version(void) {
    printf("pocket-tool version " VERSION_NO "\n");
}

static void parse_command(int argc, char *argv[]) {
    argv_0 = strdup(argv[0]);

    if (argc < 2) {
        usage(stderr, argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "help") == 0   ||
        strcmp(argv[1], "h") == 0      ||
        strcmp(argv[1], "--help") == 0 ||
        strcmp(argv[1], "-h") == 0) {
        // If help message is requested explicitly, print it on stdout. As
        // suggested by
        // https://www.jstorimer.com/blogs/workingwithcode/7766119-when-to-use
        // -stderr-instead-of-stdout.
        usage(stdout, argv[0]);
        exit(0);
    } else if (strcmp(argv[1], "version") == 0 ||
        strcmp(argv[1], "v") == 0              ||
        strcmp(argv[1], "--version") == 0      ||
        strcmp(argv[1], "-v") == 0) {
        version();
        exit(0);
    } else if (strcmp(argv[1], "auth") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s auth <consumer-key>\n", argv_0);
            exit(1);
        }
        auth(argv[2], argv[0]);
        exit(0);
    } else if (strcmp(argv[1], "list") == 0) {
        action = LIST;
        state  = ALL;
    } else if (strcmp(argv[1], "favorite") == 0) {
        action    = FAVORITE;
        state     = ALL;
        favorited = 0; // Only search articles not already favorited.
    } else if (strcmp(argv[1], "unfavorite") == 0) {
        action    = UNFAVORITE;
        state     = ALL;
        favorited = 1;
    } else if (strcmp(argv[1], "archive") == 0) {
        action = ARCHIVE;
        state  = NOT_ARCHIVED;
    } else if (strcmp(argv[1], "unarchive") == 0) {
        action = UNARCHIVE;
        state  = ARCHIVED;
    } else if (strcmp(argv[1], "delete") == 0) {
        action = DELETE;
        state  = ALL;
    } else {
        fprintf(stderr, "%s: unrecognized command `%s'\n", argv[0], argv[1]);
        exit(1);
    }

    struct option longopts[] = {
        { "version",        no_argument,        0,  'v' },
        { "help",           no_argument,        0,  'h' },
        { "with-tag",       required_argument,  0,  't' },
        { "without-tag",    required_argument,  0,  'T' },
        { "archived",       no_argument,        0,  'a' },
        { "not-archived",   no_argument,        0,  'A' },
        { "favorited",      no_argument,        0,  'f' },
        { "not-favorited",  no_argument,        0,  'F' },
        { "before",         required_argument,  0,  'b' },
        { "since",          required_argument,  0,  's' },
        { "quiet",          no_argument,        0,  'q' },
        { 0, 0, 0, 0 }
    };

    int c;
    int option_index = 0;
    struct tm tm_before;
    struct tm tm_since;

    while ((c = getopt_long(argc, argv, "vht:T:aAfFb:s:q", longopts, &option_index)) != -1)
        switch (c)
        {
            case 'v':
                version();
                exit(0);
            case 'h':
                usage(stdout, argv[0]);
                exit(0);
            case 't':
                with_tags_len++;
                with_tags = realloc(with_tags, with_tags_len * sizeof(char *));
                with_tags[with_tags_len-1] = strdup(optarg);
                break;
            case 'T':
                without_tags_len++;
                without_tags = realloc(without_tags, without_tags_len * sizeof(char *));
                without_tags[without_tags_len-1] = strdup(optarg);
                break;
            case 'a':
                if (action == ARCHIVE || action == UNARCHIVE) {
                    fprintf(stderr, "%s: ignored criterium: `%s'\n", argv_0, argv[optind-1]);
                    break;
                } else if (state == NOT_ARCHIVED) {
                    fprintf(stderr, "%s: contradictory criteria\n", argv_0);
                    exit(1);
                } else {
                    state = ARCHIVED;
                    break;
                }
            case 'A':
                if (action == ARCHIVE || action == UNARCHIVE) {
                    fprintf(stderr, "%s: ignored criterium: `%s'\n", argv_0, argv[optind-1]);
                    break;
                } else if (state == ARCHIVED) {
                    fprintf(stderr, "%s: contradictory criteria\n", argv_0);
                    exit(1);
                } else {
                    state = NOT_ARCHIVED;
                    break;
                }
            case 'f':
                if (favorited == 0) {
                    fprintf(stderr, "%s: contradictory criteria\n", argv_0);
                    exit(1);
                }
                favorited = 1;
                break;
            case 'F':
                if (favorited == 1) {
                    fprintf(stderr, "%s: contradictory criteria\n", argv_0);
                    exit(1);
                }
                favorited = 0;
                break;
            case 'b':
                strptime(optarg, "%F", &tm_before);
                before_date = mktime(&tm_before);
                break;
            case 's':
                strptime(optarg, "%F", &tm_since);
                since_date = mktime(&tm_since);
                break;
            case 'q':
                quiet = 1;
                break;
            default:
                // 'getopt_long()' will have already printed an error message.
                exit(1);
        }

#ifdef DEBUG
    // Print any remaining command line arguments (not options).
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
#endif
}

static void free_unless_null(void *ptr) {
    if (ptr != NULL)
        free(ptr);
}

static void free_articles(article_t *articles, int num_articles) {
    for (int i = 0; i < num_articles; i++) {
        article_t a = articles[i];

        for (int j = 0; j < a.num_tags; j++) {
            free(a.tags[j]);
        }

        free_unless_null(a.tags);
        free_unless_null(a.url);
        free_unless_null(a.id);
        free_unless_null(a.title);
    }
}

int main(int argc, char *argv[]) {
    parse_command(argc, argv);
    read_key_token();

#ifdef TEST
    test();
    exit(0);
#endif

    curl_global_init(CURL_GLOBAL_ALL);

    article_t *articles;
    int num_articles = pull(state, favorited, since_date, &articles);

    if (before_date != 0) { // --before was used
        // The pocket API only supports "since", so we must make a call with
        // before_date as "since" and unmatch those articles.
        article_t *to_unmatch;
        int num_to_unmatch = pull(state, favorited, before_date, &to_unmatch);

        // Both 'articles' and 'to_unmatch' are sorted newest articles first, so
        // it'll do to enumerate through 'articles' once.
        {
            int i = 0;
            int j = 0;
            while (i < num_articles && j < num_to_unmatch) {
                if (strcmp(articles[i].id, to_unmatch[j].id) == 0) {
                    articles[i].match = 0;
                    j++;
                }
                i++;
            }
        }

        free_articles(to_unmatch, num_to_unmatch);
    }

    int mod_articles = filter_all(articles, num_articles, with_tags, with_tags_len,
                       without_tags, without_tags_len);

    // The list action doesn't require any more requests.
    if (action != LIST)
        push(action, articles, num_articles);

    print_result(action, mod_articles, articles, num_articles, quiet);

    free_articles(articles, num_articles);
    free(key);
    free(token);
    free(argv_0);

    curl_global_cleanup();

    return 0;
}
