#!/usr/bin/env bash
set -e

CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -pthread -O2"

# Logger (lib da Etapa 1)
SRC_LOG="Logger.cpp"
HDR_LOG="Logger.h"

# Binários
BIN_LOGGER="test_logger"

# Compila servidor HTTP
BIN_HTTP="http_server"
$CXX $CXXFLAGS http_server.cpp $SRC_LOG -o $BIN_HTTP

echo "Build concluído:"
ls -lh $BIN_LOGGER $BIN_HTTP
