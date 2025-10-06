#include "Logger.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <csignal>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <unordered_set>

static std::atomic<bool> g_running{true};

struct ClientSet {
    std::mutex mtx;
    std::unordered_set<int> sockets;

    void add(int fd) {
        std::lock_guard<std::mutex> lk(mtx);
        sockets.insert(fd);
    }
    void remove(int fd) {
        std::lock_guard<std::mutex> lk(mtx);
        sockets.erase(fd);
    }
    std::vector<int> snapshot() {
        std::lock_guard<std::mutex> lk(mtx);
        return std::vector<int>(sockets.begin(), sockets.end());
    }
};

void broadcast(ClientSet& clients, int from_fd, const std::string& msg) {
    auto list = clients.snapshot();
    for (int fd : list) {
        if (fd == from_fd) continue;
        send(fd, msg.data(), msg.size(), MSG_NOSIGNAL);
    }
}

void client_thread(ClientSet& clients, int client_fd, sockaddr_in addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    int port = ntohs(addr.sin_port);

    Logger::getInstance().info("Cliente conectado: " + std::string(ip) + ":" + std::to_string(port));

    clients.add(client_fd);

    char buf[4096];
    while (g_running.load()) {
        ssize_t r = recv(client_fd, buf, sizeof(buf), 0);
        if (r <= 0) {
            Logger::getInstance().info("Cliente desconectado.");
            break;
        }
        std::string msg(buf, buf + r);
        Logger::getInstance().info("Mensagem recebida: " + msg);
        broadcast(clients, client_fd, msg);
    }

    clients.remove(client_fd);
    close(client_fd);
}

void handle_sigint(int) {
    g_running.store(false);
    Logger::getInstance().info("Servidor encerrando...");
}

int main(int argc, char** argv) {
    int port = 5555;
    if (argc >= 2) port = std::stoi(argv[1]);

    std::signal(SIGINT, handle_sigint);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons(port);

    if (bind(server_fd, (sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 128) < 0) {
        perror("listen");
        return 1;
    }

    Logger::getInstance().info("Servidor rodando na porta " + std::to_string(port));

    ClientSet clients;

    while (g_running.load()) {
        sockaddr_in cli{};
        socklen_t len = sizeof(cli);
        int cfd = accept(server_fd, (sockaddr*)&cli, &len);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }
        std::thread(client_thread, std::ref(clients), cfd, cli).detach();
    }

    close(server_fd);
    Logger::getInstance().info("Servidor finalizado.");
    return 0;
}
