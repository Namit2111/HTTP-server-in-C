#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "utils.c"
#include <stdlib.h>
#include <time.h>
#pragma comment(lib, "ws2_32.lib")

#define SIZE 1024
typedef void (*PostHandler)(SOCKET, QueryParams *, int); // define a function pointer usefull for post requests

typedef struct {
    const char *route;
    const char *file_path;
    PostHandler post_handler;
} Route;


void get_request_handler(SOCKET clientSocket,const char *url, Route *routes, int route_count) {
    int route_found = 0;
    for (int i = 0; i < route_count; i++) {
        if (strcmp(url, routes[i].route) == 0) {
            route_found = 1;
            FILE *fp = fopen(routes[i].file_path, "r");
            if (fp == NULL) {
                const char *response_not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
            }
            else {
                
                const char *file_type = getfiletype(routes[i].file_path);
                const char *content_type = get_content_type(file_type);
                if (send(clientSocket,content_type, strlen(content_type), 0) == SOCKET_ERROR) {
                    printf("Error sending response: %d\n", WSAGetLastError());
                    closesocket(clientSocket);
                } 
                send_file(fp, clientSocket);
                fclose(fp);
            }
        }
    }
        if (!route_found) {
        const char *response_not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        if (send(clientSocket, response_not_found, strlen(response_not_found), 0) == SOCKET_ERROR) {
            printf("Error sending response: %d\n", WSAGetLastError());
            closesocket(clientSocket);
        }
    }
}

void post_request_handler(SOCKET clientSocket, const char *url, QueryParams *queryParams, int queryParamCount, Route *routes, int route_count) {
    int route_found = 0;
    for (int i = 0; i < route_count; i++) {
        if (strcmp(url, routes[i].route) == 0 && routes[i].post_handler != NULL) {
            route_found = 1;
            routes[i].post_handler(clientSocket, queryParams, queryParamCount); // Call the POST handler for this route
            break;
        }
    }
    if (!route_found) {
        const char *response_not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        send(clientSocket, response_not_found, strlen(response_not_found), 0);
    }
}





void register_routes(Route routes[], int *route_count, const char *route, const char *file_path, PostHandler post_handler) {
    routes[*route_count].route = route;
    routes[*route_count].file_path = file_path;
    routes[*route_count].post_handler = post_handler;
    (*route_count)++;
}





void generate_random_numbers(SOCKET clientSocket, int count) {
    char response[2048];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n");

    // Start JSON object
    snprintf(response + strlen(response), sizeof(response) - strlen(response), "{ \"random_numbers\": [");

    // Generate random numbers and add to the JSON array
    for (int i = 0; i < count; i++) {
        int random_num = rand();
        snprintf(response + strlen(response), sizeof(response) - strlen(response), "%d", random_num);
        
        if (i < count - 1) {
            snprintf(response + strlen(response), sizeof(response) - strlen(response), ", ");
        }
    }

    // End JSON array and object
    snprintf(response + strlen(response), sizeof(response) - strlen(response), "] }");

    // Send the response
    if (send(clientSocket, response, strlen(response), 0) == SOCKET_ERROR) {
        printf("Error sending response: %d\n", WSAGetLastError());
        closesocket(clientSocket);
    }
}




void random_number_post_handler(SOCKET clientSocket, QueryParams *queryParams, int queryParamCount) {
    int count = 1;  

    
    for (int i = 0; i < queryParamCount; i++) {
        if (strcmp(queryParams[i].key, "count") == 0) {
            count = atoi(queryParams[i].value);  // Convert the value to an integer
            break;
        }
    }

    // Generate and send the random numbers response
    generate_random_numbers(clientSocket, count);
}





int main() {
    WSADATA wsaData;
    int socketconnec;
    struct sockaddr_in serverconfig, clientAddr;
    char buffer[1024];
    char *request_type, *url;
    // FILE *txtptr;
    // typedef struct {
    //     const char *key;
    //     const char *value;
    // } QueryParams;

    int clientAddrSize = sizeof(clientAddr);
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    socketconnec = socket(AF_INET, SOCK_STREAM, 0);
    if (socketconnec == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    serverconfig.sin_family = AF_INET;
    serverconfig.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverconfig.sin_port = htons(8080);

    int bindStatus = bind(socketconnec, (struct sockaddr *)&serverconfig, sizeof(serverconfig));
    if (bindStatus != 0) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    int listenstatus = listen(socketconnec, 4);
    if (listenstatus != 0) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    printf("Server is listening on port 8080\n");


    Route routes[10];
    int route_count = 0 ;

    register_routes(routes, &route_count,"/","index.html",NULL);
    register_routes(routes, &route_count,"/about","about.html",NULL);
    register_routes(routes,&route_count,"/contact","contact.html",NULL);
    register_routes(routes, &route_count, "/random_numbers", NULL, random_number_post_handler); 
    QueryParams queryParams[10];
    int queryParamCount;
    
    // QueryParams queryParams[10];
    // int queryParamCount = 0;

    while (1) {
        SOCKET clientSocket = accept(socketconnec, (struct sockaddr *)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            return 1;
        }
        printf("Client connected\n");

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            printf("Error receiving data: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            continue;
        }
        if (bytesReceived == 0) {
            printf("No data received\n");
            closesocket(clientSocket);
            continue;
        }

        buffer[bytesReceived] = '\0';
        // printf("Request: %s\n", buffer);

        request_type = trim(strtok(buffer, " "));
        // printf("Request Type: %s\n", request_type);

        url = trim(strtok(NULL, " "));
        // printf("Requested URL: %s\n", url);

        if (strcmp(request_type,"GET")==0){
            get_request_handler(clientSocket,url,routes,route_count);
        }
        if (strcmp(request_type, "POST") == 0) {
            char *temp = strtok(url, "?");
            char *query_string = strtok(NULL, "?");

            queryParamCount = parse_query_params(query_string, queryParams, 10);

             post_request_handler(clientSocket, url, queryParams, queryParamCount, routes, route_count);
            // Print the parsed query parameters for debugging
            // for (int i = 0; i < queryParamCount; i++) {
            //     printf("%s: %s\n", queryParams[i].key, queryParams[i].value);
            // }
        }


        if (closesocket(clientSocket) == SOCKET_ERROR) {
            printf("Error closing socket: %d\n", WSAGetLastError());
        }
    }

    printf("Exited\n");
    return 0;
}
