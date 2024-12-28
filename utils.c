#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>


#define SIZE 1024
char *trim(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';

    return str;
}

bool isfile(char *url) {
    const char *ext = strrchr(url, '.');
    return (ext != NULL);
}

const char *getfiletype(const char *url) {
    const char *ext = strrchr(url, '.');
    return ext + 1;
}



void send_file(FILE *fp, SOCKET clientSocket) {
    int n;
    char data[SIZE] = {0};

    while (fgets(data, SIZE, fp) != NULL) {
        if (send(clientSocket, data, strlen(data), 0) == -1) {
            perror("Error in sending file.");
            exit(1);
        }
        memset(data, 0, SIZE);
    }
}

const char* get_content_type(const char* file_extension) {
    if (strcmp(file_extension, "html") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    } else if (strcmp(file_extension, "txt") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    } else if (strcmp(file_extension, "css") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n";
    } else if (strcmp(file_extension, "js") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: application/javascript\r\n\r\n";
    } else if (strcmp(file_extension, "json") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
    } else if (strcmp(file_extension, "jpg") == 0 || strcmp(file_extension, "jpeg") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\n\r\n";
    } else if (strcmp(file_extension, "png") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n";
    } else if (strcmp(file_extension, "gif") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n";
    } else if (strcmp(file_extension, "pdf") == 0) {
        return "HTTP/1.1 200 OK\r\nContent-Type: application/pdf\r\n\r\n";
    } else { 
        return "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n";
    }
}


typedef struct {
    const char *key;
    const char *value;
} QueryParams;

int parse_query_params(const char *query_string, QueryParams *queryParams, int max_params) {
    int queryParamCount = 0;
    if (query_string == NULL || queryParams == NULL || max_params <= 0) {
        return 0;
    }

    const char *param_start = query_string;
    while (param_start != NULL && *param_start != '\0' && queryParamCount < max_params) {
        const char *param_end = strchr(param_start, '&');
        size_t length = (param_end != NULL) ? (size_t)(param_end - param_start) : strlen(param_start);

        char param[length + 1];
        strncpy(param, param_start, length);
        param[length] = '\0';

        char *equal_sign = strchr(param, '=');
        if (equal_sign != NULL) {
            *equal_sign = '\0'; // Split key and value

            const char *key = param;
            const char *value = equal_sign + 1;

            queryParams[queryParamCount].key = strdup(key);
            queryParams[queryParamCount].value = strdup(value);
            queryParamCount++;
        }

        param_start = (param_end != NULL) ? param_end + 1 : NULL;
    }

    return queryParamCount;
}
