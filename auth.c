#include "main.h"

#define REDIRECT_URI "http://example.com"

static char *get_access_token_and_username(CURL *handle, char *key, char *request_token) {
    char *postfields;
    asprintf(&postfields, "consumer_key=%s&code=%s", key, request_token);

    mem_struct response;
    response.memory = malloc(1);
    response.size = 0;

    curl_easy_setopt(handle, CURLOPT_URL, "https://getpocket.com/v3/oauth/authorize");
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postfields);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *) &response);

    Curl_easy_perform(handle);

    long http_code = 0;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &http_code);

    if (http_code != 200) {
        fprintf(stderr, "\x1b[31mError\x1b[0m: Authorization failed. Make sure to complete "
            "the online authorization\n"
            "before pressing ENTER.\n");
        exit(1);
    }

    free(postfields);

    return response.memory;
}

static char *get_request_token(CURL *handle, char *key) {
    char *postfields;
    asprintf(&postfields, "consumer_key=%s&redirect_uri=%s", key, REDIRECT_URI);

    mem_struct response;
    response.memory = malloc(1);
    response.size = 0;

    curl_easy_setopt(handle, CURLOPT_URL, "https://getpocket.com/v3/oauth/request");
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, postfields);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *) &response);

    Curl_easy_perform(handle);

    char *request_token = malloc(response.size - 4);
    memcpy(request_token, response.memory + 5, response.size - 4);
        // Copy from +5 to cut 'code=' from beginning

    free(response.memory);
    free(postfields);

    return request_token;
}

void auth(char *key, char *name) {
    char *abs_path = home(".pocket-tool");
    if ( access(abs_path, F_OK ) != -1 ) {
        printf("Configuration file ~/.pocket-tool already exists. If you wish to "
            "redo\nauthentication, please remove this file.\n");
        exit(0);
    }

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *handle = curl_easy_init(); assert(handle);

    // Step 2: Obtain a request token.
    char *request_token = get_request_token(handle, key);

    // Step 3: Redirect user to Pocket to continue authorization
    printf(
        "Please open the following URL in a web browser to authorize %s:\n"
        "\n"
        "https://getpocket.com/auth/authorize?request_token=%s&redirect_uri=%s\n"
        "\n"
        "After having completed authorization, press ENTER to continue.",
        name, request_token, REDIRECT_URI
        );
    getchar();

    // Step 5: Convert a request token into a Pocket access token and username
    char *atu = get_access_token_and_username(handle, key,
        request_token);

    // ---- Get length of token and username -----------------------------------
    int token_len = 0;
    int username_len = 0;
    int ampersand_scanned = 0;

    assert(strncmp(atu, "access_token=", strlen("access_token=")) == 0);

    {
        int i = strlen("access_token=");
        while (atu[i] != '\0') {
            if (atu[i] == '&') {
                ampersand_scanned = 1;
                assert(strncmp(atu + i, "&username=", strlen("&username=")) == 0);
                i += strlen("username=");
            }
            else if (!ampersand_scanned)
                token_len++;
            else
                username_len++;
            i++;
        }
    }
    // -------------------------------------------------------------------------

    char *access_token = malloc(token_len + 1);
    char *username = malloc(username_len + 1);
    char *formatstring;

    asprintf(&formatstring, "access_token=%%%ds&username=%%%ds", token_len,
        username_len);
    sscanf(atu, formatstring, access_token, username);

    free(formatstring);
    free(atu);

    printf("Username: %s\n", username);
    printf("Access token: %s\n", access_token);

    // Print to file
    FILE *out = fopen(abs_path, "w");
    fprintf(out, "%s\n%s\n", key, access_token);
    fclose(out);
    free(abs_path);

    printf("Key and token written to ~/.pocket-tool.\n");

    free(username);
    free(access_token);
    free(request_token);

    curl_easy_cleanup(handle);
    curl_global_cleanup();
}
