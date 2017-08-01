#include "main.h"

static void push_helper(action_t action, article_t *articles, int num_articles, CURL *handle) {
    char *action_str;
    switch (action) {
        case FAVORITE:
            action_str = "favorite";
            break;
        case UNFAVORITE:
            action_str = "unfavorite";
            break;
        case ARCHIVE:
            action_str = "archive";
            break;
        case UNARCHIVE:
            action_str = "readd";
            break;
        case DELETE:
            action_str = "delete";
            break;
        default:
            assert(!"'action' must match one of the previous cases.");
    }

    int url_size = num_articles * 500 + 200;
    int url_offs = 0;
    char *url = malloc(url_size);

    SNPRINTF(url, url_size, url_offs, "https://getpocket.com/v3/send?actions=%%5B")

    // For each do
    for (int i = 0; i < num_articles; i++) {
        if (articles[i].match) {
            SNPRINTF(url, url_size, url_offs,
                "%%7B%%22action%%22%%3A%%22%s%%22%%2C%%22item_id%%22%%3A%%22%s"
                "%%22%%7D%%2C", action_str, articles[i].id)
        }
    }
    url_offs -= 3;
    url[url_offs] = '\0'; // Remove last ,

    SNPRINTF(url, url_size, url_offs, "%%5D&access_token=%s&consumer_key=%s",
        token, key)

    mem_struct response;
    response.memory = malloc(1);
    response.size = 0;

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *) &response);

    Curl_easy_perform(handle);

    free(url);
    free(response.memory);
}

void push(action_t action, article_t *articles, int num_articles) {
    CURL *handle = curl_easy_init(); assert(handle);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, callback);

    int passed_filter = 0;
    int processed = 0;

    for (int i = 0; i < num_articles; i++) {
        if (articles[i].match)
            passed_filter++;

        // If passed_filter == 100, 200, 300, ... or if this is the last
        // iteration.
        if (passed_filter > 0 && (passed_filter % ARTICLES_PER_REQUEST == 0 || i == num_articles - 1)) {
            // Push a subset of the articles (the Pocket server can only handle
            // ~100 articles per request).
            push_helper(action, articles + processed, i - processed, handle);
            processed += i - processed;
        }
    }
    curl_easy_cleanup(handle);
}
