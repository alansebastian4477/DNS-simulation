#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to simulate DNS lookup
const char* lookup_ip(const char* domain) {
    if (strcmp(domain, "example.com") == 0) return "93.184.216.34";
    if (strcmp(domain, "google.com") == 0) return "142.250.182.206";
    if (strcmp(domain, "facebook.com") == 0) return "157.240.22.35";
    return "Domain Not Found";
}

// Handle client requests in separate threads
void *handle_client(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE] = {0};

    // Read domain name from client
    read(client_sock, buffer, BUFFER_SIZE);
    printf("Received request for domain: %s\n", buffer);

    // Lookup IP for the given domain
    const char* ip = lookup_ip(buffer);

    // Send IP back to client
    send(client_sock, ip, strlen(ip), 0);
    printf("Sent IP: %s\n\n", ip);

    close(client_sock);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(client_addr);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind and listen
    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 3);

    printf("DNS Server listening on port %d...\n", PORT);

    // Accept and handle clients
    while (1) {
        client_sock = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handle_client, (void*)new_sock);
        pthread_detach(client_thread);
    }

    return 0;
}

