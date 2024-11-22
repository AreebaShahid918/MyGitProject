#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to get the current timestamp
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

// Function to process the raw data and extract required fields
void process_data(const char *raw_file, const char *processed_file) {
    FILE *raw = fopen(raw_file, "r");
    FILE *processed = fopen(processed_file, "a");

    if (!raw || !processed) {
        printf("Error opening files.\n");
        return;
    }

    char line[1024];
    char timestamp[20];
    char city[] = "Karachi";
    char province[] = "Sindh";
    char country[] = "Pakistan";
    double temperature = 0;
    int humidity = 0;
    double wind_speed = 0;
    int wind_deg = 0;
    double pressure = 0;

    get_timestamp(timestamp, sizeof(timestamp));

    // Read the entire raw data (JSON) into a buffer
    char raw_data[4096] = {0};  // Initialize raw_data to avoid undefined behavior
    size_t i = 0;
    while (fgets(line, sizeof(line), raw)) {
        strcat(raw_data, line);
        i += strlen(line);
    }

    // Parse relevant fields from the raw JSON data
    if (strstr(raw_data, "\"temp\":")) {
        sscanf(strstr(raw_data, "\"temp\":") + 7, "%lf", &temperature);
    }
    if (strstr(raw_data, "\"humidity\":")) {
        sscanf(strstr(raw_data, "\"humidity\":") + 11, "%d", &humidity);
    }
    if (strstr(raw_data, "\"speed\":")) {
        sscanf(strstr(raw_data, "\"speed\":") + 8, "%lf", &wind_speed);
    }
    if (strstr(raw_data, "\"deg\":")) {
        sscanf(strstr(raw_data, "\"deg\":") + 6, "%d", &wind_deg);
    }
    if (strstr(raw_data, "\"pressure\":")) {
        sscanf(strstr(raw_data, "\"pressure\":") + 11, "%lf", &pressure);
    }

    // Display alert for high temperature or humidity
    if (temperature > 38 || humidity > 70) {
        printf("\n*** ALERT! Critical Environmental Conditions Detected! ***\n");
    }

    // Display the processed data
    printf("\n--- Environmental Data ---\n");
    printf("Timestamp: %s\n", timestamp);
    printf("Location: %s, %s, %s\n", city, province, country);
    printf("Temperature: %.2lf°C\n", temperature);
    printf("Humidity: %d%%\n", humidity);
    printf("Wind Speed: %.2lf m/s\n", wind_speed);
    printf("Wind Direction: %d°\n", wind_deg);
    printf("Atmospheric Pressure: %.2lf hPa\n", pressure);
    printf("--------------------------\n");

    // Check if the file is empty, and write the header if it is
    fseek(processed, 0, SEEK_SET);  // Move to the start of the file
    char ch = fgetc(processed);     // Read the first character
    if (ch == EOF) {
        // File is empty, write the header
        fprintf(processed, "Timestamp, City, Province, Country, Temperature (°C), Humidity (%%), Wind Speed (m/s), Wind Direction (°), Pressure (hPa)\n");
    }

    // Write the actual data to the file
    fprintf(processed, "%s, %s, %s, %s, %.2lf, %d, %.2lf, %d, %.2lf\n",
            timestamp, city, province, country, temperature, humidity, wind_speed, wind_deg, pressure);

    fclose(raw);
    fclose(processed);
}

