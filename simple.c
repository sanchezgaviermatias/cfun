#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/timerfd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024
#define DEFAULT_PORT 3000

// Simple callback structure to mimic JavaScript callbacks
typedef struct
{
    void (*fn)(void *data);
    void *data;
} Callback;

// Event loop structure
typedef struct
{
    int epoll_fd;
    int running;
} EventLoop;

// Timer structure
typedef struct
{
    int fd;
    Callback callback;
} Timer;

// Basic HTTP server structure
typedef struct
{
    int server_fd;
    void (*request_handler)(char *request, char *response);
} HttpServer;

// Initialize the event loop
EventLoop *create_event_loop()
{
    EventLoop *loop = malloc(sizeof(EventLoop));
    if (!loop)
        return NULL;

    loop->epoll_fd = epoll_create1(0);
    printf("the file descriptor is epoll_fd: %d\n", loop->epoll_fd);
    loop->epoll_fd = epoll_create1(0);
    printf("the file descriptor is epoll_fd: %d\n", loop->epoll_fd);
    loop->epoll_fd = epoll_create1(0);
    printf("the file descriptor is epoll_fd: %d\n", loop->epoll_fd);
    loop->epoll_fd = epoll_create1(0);
    printf("the file descriptor is epoll_fd: %d\n", loop->epoll_fd);
    if (loop->epoll_fd == -1)
    {
        free(loop);
        return NULL;
    }

    loop->running = 0;
    return loop;
}

// Add file descriptor to event loop
void add_to_event_loop(EventLoop *loop, int fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    epoll_ctl(loop->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

// Simple implementation of setTimeout
Timer *set_timeout(EventLoop *loop, Callback callback, int ms)
{
    Timer *timer = malloc(sizeof(Timer));
    if (!timer)
        return NULL;

    timer->fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer->fd == -1)
    {
        free(timer);
        return NULL;
    }

    timer->callback = callback;

    struct itimerspec ts = {0};
    ts.it_value.tv_sec = ms / 1000;
    ts.it_value.tv_nsec = (ms % 1000) * 1000000;

    if (timerfd_settime(timer->fd, 0, &ts, NULL) == -1)
    {
        close(timer->fd);
        free(timer);
        return NULL;
    }

    add_to_event_loop(loop, timer->fd, EPOLLIN);
    return timer;
}

// Create HTTP server
HttpServer *create_http_server(EventLoop *loop, void (*handler)(char *, char *))
{
    HttpServer *server = malloc(sizeof(HttpServer));
    if (!server)
        return NULL;

    server->request_handler = handler;

    // Create socket
    server->server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server->server_fd == -1)
    {
        free(server);
        return NULL;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(server->server_fd);
        free(server);
        return NULL;
    }

    // Set up server address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEFAULT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind and listen
    if (bind(server->server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        close(server->server_fd);
        free(server);
        return NULL;
    }

    if (listen(server->server_fd, SOMAXCONN) == -1)
    {
        close(server->server_fd);
        free(server);
        return NULL;
    }

    // Add to event loop
    add_to_event_loop(loop, server->server_fd, EPOLLIN);
    return server;
}

// Accept new connection
int accept_connection(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    return accept4(server_fd, (struct sockaddr *)&client_addr,
                   &client_len, SOCK_NONBLOCK);
}

// Handle events in the event loop
void handle_events(EventLoop *loop, HttpServer *server)
{
    struct epoll_event events[MAX_EVENTS];
    int n = epoll_wait(loop->epoll_fd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++)
    {
        int fd = events[i].data.fd;

        if (fd == server->server_fd)
        {
            // New connection
            int client_fd = accept_connection(server->server_fd);
            if (client_fd != -1)
            {
                add_to_event_loop(loop, client_fd, EPOLLIN | EPOLLET);
            }
        }
        else
        {
            // Handle client request
            char request[BUFFER_SIZE];
            char response[BUFFER_SIZE];
            ssize_t bytes = read(fd, request, sizeof(request) - 1);

            if (bytes > 0)
            {
                request[bytes] = '\0';
                server->request_handler(request, response);
                write(fd, response, strlen(response));
            }

            close(fd);
        }
    }
}

// Run the event loop
void run_event_loop(EventLoop *loop, HttpServer *server)
{
    loop->running = 1;
    printf("Server running on port %d...\n", DEFAULT_PORT);

    while (loop->running)
    {
        handle_events(loop, server);
    }
}

// Example HTTP request handler
void handle_http_request(char *request, char *response)
{
    (void)request; // Unused parameter
    sprintf(response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!");
}

// Timer callback example
void timer_callback(void *data)
{
    printf("Timer fired! Message: %s\n", (char *)data);
}

int main()
{
    // Create and initialize event loop
    EventLoop *loop = create_event_loop();
    if (!loop)
    {
        fprintf(stderr, "Failed to create event loop\n");
        return 1;
    }

    // Create HTTP server
    HttpServer *server = create_http_server(loop, handle_http_request);
    if (!server)
    {
        fprintf(stderr, "Failed to create HTTP server\n");
        free(loop);
        return 1;
    }

    // Set up a timer
    Callback cb = {timer_callback, "Hello from timer!"};
    Timer *timer = set_timeout(loop, cb, 1000);
    if (!timer)
    {
        fprintf(stderr, "Failed to create timer\n");
        free(server);
        free(loop);
        return 1;
    }

    // Run the event loop
    run_event_loop(loop, server);

    // Cleanup
    close(timer->fd);
    free(timer);
    close(server->server_fd);
    free(server);
    close(loop->epoll_fd);
    free(loop);

    return 0;
}