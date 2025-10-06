#include "Logger.h"
#include <thread>
#include <vector>

void worker(int id) {
    for (int i = 0; i < 5; i++) {
        Logger::getInstance().info("Thread " + std::to_string(id) + " -> log " + std::to_string(i));
    }
}

int main() {
    Logger::getInstance().info("=== Teste de Logging Thread-Safe ===");

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back(worker, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    Logger::getInstance().info("=== Fim do Teste ===");
    return 0;
}
