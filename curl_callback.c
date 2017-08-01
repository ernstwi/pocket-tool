#include "main.h"

// Courtesy of libcurl example https://curl.haxx.se/libcurl/c/getinmemory.html
size_t callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  mem_struct *mem = (mem_struct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}
