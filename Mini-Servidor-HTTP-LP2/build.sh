#!/usr/bin/env bash
set -e

echo "Compilando servidor HTTP..."
g++ -std=c++17 -pthread http_server.cpp Logger.cpp -o http_server

echo "Build concluído com sucesso!"
ls -lh http_server
