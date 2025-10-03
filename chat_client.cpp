#include "Logger.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

static std::atomic<bool> g_running{true};

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <host> <porta>\n";
        return 1;
    }

    std::string host = argv[1];
    int port = std::stoi(argv[2]);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &srv.sin_addr);

    if (connect(fd, (sockaddr*)&srv, sizeof(srv)) < 0) {
        perror("connect");
        return 1;
    }

    Logger::getInstance().info("Conectado a " + host + ":" + std::to_string(port));

    std::thread reader([&]() {
        char buf[4096];
        while (g_running.load()) {
            ssize_t r = recv(fd, buf, sizeof(buf), 0);
            if (r <= 0) {
                std::cout << "[Servidor desconectou]\n";
                g_running.store(false);
                break;
            }
            std::cout.write(buf, r);
            std::cout.flush();
        }
    });

    std::thread writer([&]() {
        std::string line;
        while (g_running.load() && std::getline(std::cin, line)) {
            line.push_back('\n');
            send(fd, line.data(), line.size(), MSG_NOSIGNAL);
        }
        g_running.store(false);
    });

    reader.join();
    writer.join();
    close(fd);
    Logger::getInstance().info("Cliente finalizado.");
    return 0;
}
