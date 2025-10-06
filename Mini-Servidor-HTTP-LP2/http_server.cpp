#include "Logger.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <csignal>

static std::atomic<bool> g_running{true};
static std::atomic<int> active_connections{0};

// Função auxiliar compatível com C++17
bool ends_with(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string get_mime_type(const std::string &path) {
    if (ends_with(path, ".html")) return "text/html";
    if (ends_with(path, ".txt"))  return "text/plain";
    if (ends_with(path, ".jpg") || ends_with(path, ".jpeg")) return "image/jpeg";
    if (ends_with(path, ".png"))  return "image/png";
    return "application/octet-stream";
}

std::string read_file(const std::string &path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void handle_client(int client_fd, sockaddr_in addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    Logger::getInstance().info("Conexão iniciada de " + std::string(ip));

    active_connections++;
    Logger::getInstance().info("Conexões ativas: " + std::to_string(active_connections.load()));

    // Timeout opcional (10 s sem tráfego)
    struct timeval timeout {10, 0};
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    bool keep_alive = true;
    char buf[8192];

    while (keep_alive && g_running.load()) {
        ssize_t r = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (r <= 0) break;

        buf[r] = '\0';
        std::string req(buf);
        Logger::getInstance().info("Requisição recebida:\n" + req);

        std::istringstream iss(req);
        std::string method, path, version;
        iss >> method >> path >> version;

        if (method != "GET") {
            std::string resp = "HTTP/1.1 405 Method Not Allowed\r\nConnection: close\r\n\r\n";
            send(client_fd, resp.c_str(), resp.size(), 0);
            break;
        }

        // Se o cabeçalho não contém “Connection: close”, mantém a conexão viva
        keep_alive = req.find("Connection: close") == std::string::npos;

        std::string full_path = "www" + path;
        if (full_path == "www/") full_path = "www/index.html";

        std::string content = read_file(full_path);
        std::ostringstream resp;

        if (content.empty()) {
            std::string msg = "<h1>404 Not Found</h1>";
            resp << "HTTP/1.1 404 Not Found\r\n"
                 << "Content-Type: text/html\r\n"
                 << "Content-Length: " << msg.size() << "\r\n"
                 << "Connection: " << (keep_alive ? "keep-alive" : "close") << "\r\n\r\n"
                 << msg;
        } else {
            resp << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: " << get_mime_type(full_path) << "\r\n"
                 << "Content-Length: " << content.size() << "\r\n"
                 << "Connection: " << (keep_alive ? "keep-alive" : "close") << "\r\n\r\n"
                 << content;
        }

        std::string response = resp.str();
        send(client_fd, response.c_str(), response.size(), 0);

        if (!keep_alive) break;
    }

    close(client_fd);
    active_connections--;
    Logger::getInstance().info("Conexão encerrada: " + std::string(ip) +
                               " | Restantes: " + std::to_string(active_connections.load()));
}

void handle_sigint(int) {
    g_running.store(false);
    Logger::getInstance().info("Encerrando servidor HTTP...");
}

int main(int argc, char **argv) {
    int port = 8080;
    if (argc >= 2) port = std::stoi(argv[1]);

    std::signal(SIGINT, handle_sigint);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in srv {};
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = htonl(INADDR_ANY);
    srv.sin_port = htons(port);

    if (bind(server_fd, (sockaddr *)&srv, sizeof(srv)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 128) < 0) {
        perror("listen");
        return 1;
    }

    Logger::getInstance().info("Servidor HTTP rodando na porta " + std::to_string(port));

    while (g_running.load()) {
        sockaddr_in cli {};
        socklen_t len = sizeof(cli);
        int cfd = accept(server_fd, (sockaddr *)&cli, &len);
        if (cfd < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            continue;
        }
        std::thread(handle_client, cfd, cli).detach();
    }

    close(server_fd);
    Logger::getInstance().info("Servidor HTTP finalizado.");
    return 0;
}
