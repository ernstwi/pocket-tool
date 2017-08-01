#include "main.h"

void Curl_easy_perform(CURL *handle) {
    char *err = malloc(CURL_ERROR_SIZE); err = NULL;
    curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, err);

    CURLcode res = curl_easy_perform(handle);
    if (res != CURLE_OK) {
        fprintf(stderr, "%s: \x1b[31mError\x1b[0m: %s\n", argv_0, err != NULL ? err :
            "Unknown error");
        exit(1);
    }
    free(err);
}

// Turn ~/<path> into an absolute path
char *home(char *path) {
    size_t out_size = strlen(getenv("HOME")) + 1 + strlen(path) + 1;
    char *out = malloc(out_size);
    {
        int res = snprintf(out, out_size, "%s/%s", getenv("HOME"), path);
        assert(res < out_size); // Make sure buffer was big enough (redundant)
    }
    return out;
}
