#include "main.h"

static jsmntok_t *parse(char *json) {
    jsmn_parser parser;
    jsmn_init(&parser);

    int num_tokens = jsmn_parse(&parser, json, strlen(json), NULL, 0);
    jsmntok_t *tokens = malloc(num_tokens * sizeof(jsmntok_t));

    jsmn_init(&parser);
    jsmn_parse(&parser, json, strlen(json), tokens, num_tokens);
    return tokens;
}

static char *request(state_t state, int favorited, time_t since) {
    mem_struct response;
    response.memory = malloc(1);
    response.size = 0;

    char *url = "https://getpocket.com/v3/get";

    char *favorited_str;
    switch (favorited) {
        case 0:
            favorited_str = "\"favorite\":\"0\",";
            break;
        case 1:
            favorited_str = "\"favorite\":\"1\",";
            break;
        case -1:
            favorited_str = "";
            break;
        default:
            assert(!"'favorited_str' must match one of the previous cases.");
    }

    char *state_str;
    switch (state) {
        case ARCHIVED:
            state_str = "\"state\":\"archive\"";
            break;
        case NOT_ARCHIVED:
            state_str = "\"state\":\"unread\"";
            break;
        case ALL:
            state_str = "\"state\":\"all\"";
            break;
        default:
            assert(!"'state_str' must match one of the previous cases.");
    }

    char *body_head;
    asprintf(&body_head,
        "{"
        "\"consumer_key\":\"%s\","
        "\"access_token\":\"%s\","
        "\"detailType\":\"complete\","
        "\"sort\":\"newest\","
        "\"since\":\"%ld\",",
        key, token, since);
    char *body_tail = "}";

    int body_size = strlen(body_head) + strlen(favorited_str)
        + strlen(state_str) + strlen(body_tail) + 1;
    char *body = malloc(body_size);

    {
        int res = snprintf(body, body_size, "%s%s%s%s", body_head, favorited_str, state_str,
            body_tail);
        assert(res < body_size);
    }

    free(body_head);

#ifdef DEBUG
    printf("Body:\n%s\n\n", body);
#endif

    CURL *handle = curl_easy_init();
    assert(handle);

    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept:");
    chunk = curl_slist_append(chunk, "Another: yes");
    chunk = curl_slist_append(chunk, "Host: getpocket.com");
    chunk = curl_slist_append(chunk, "Content-Type: application/json;");

    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *) &response);

    Curl_easy_perform(handle);
    curl_easy_cleanup(handle);

    free(body);
    return response.memory;
}

int pull(state_t state, int favorited, time_t since, article_t **articles) {
    char *json = request(state, favorited, since);
#ifdef DEBUG
    LOG_JSON("%s\n", json);
#endif
    jsmntok_t *tokens = parse(json);
    int num_articles = pack(tokens, json, articles);

    free(json);

    return num_articles;
}
