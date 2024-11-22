#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "process.h"

#define API_URL "https://api.openweathermap.org/data/2.5/weather?q=Karachi,PK&appid=7f4b02c31406e686a3ffac42d7aa4b0e&units=metric"
#define RAW_DATA_FILE "data_raw.csv"
#define PROCESSED_DATA_FILE "data_processed.csv"

typedef struct {
    char *memory;
    size_t size;
} MemoryBlock;

// Callback function for handling API response
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    MemoryBlock *mem = (MemoryBlock *)userp;

    char *temp = realloc(mem->memory, mem->size + total_size + 1);
    if (temp == NULL) {
        printf("Failed to allocate memory.\n");
        return 0;
    }

    mem->memory = temp;
    memcpy(&(mem->memory[mem->size]), data, total_size);
    mem->size += total_size;
    mem->memory[mem->size] = '\0';

    return total_size;
}

// Function to fetch data from the API
void fetch_data() {
    CURL *curl;
    CURLcode res;
    MemoryBlock chunk = {NULL, 0};

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error initializing cURL.\n");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, API_URL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "cURL error: %s\n", curl_easy_strerror(res));
    } else {
        FILE *file = fopen(RAW_DATA_FILE, "w");
        if (file) {
            fprintf(file, "%s", chunk.memory);
            fclose(file);
            printf("Raw data saved to %s.\n", RAW_DATA_FILE);
        } else {
            printf("Failed to open file for writing.\n");
        }
    }

    curl_easy_cleanup(curl);
    free(chunk.memory);
}

int main() {
    printf("Fetching data for Karachi...\n");
    fetch_data();
    process_data(RAW_DATA_FILE, PROCESSED_DATA_FILE);
    return 0;
}

