#!/usr/bin/env bash
set -e

CXX=g++
CXXFLAGS="-std=c++17 -Wall -Wextra -pthread -O2"

# Logger (lib da Etapa 1)
SRC_LOG="Logger.cpp"
HDR_LOG="Logger.h"

# Binários
BIN_SERVER="chat_server"
BIN_CLIENT="chat_client"
BIN_LOGGER="test_logger"

# Compila servidor
$CXX $CXXFLAGS chat_server.cpp $SRC_LOG -o $BIN_SERVER

# Compila cliente
$CXX $CXXFLAGS chat_client.cpp $SRC_LOG -o $BIN_CLIENT

# Mantém o teste da Etapa 1
$CXX $CXXFLAGS main.cpp $SRC_LOG -o $BIN_LOGGER

echo "Build concluído:"
ls -lh $BIN_SERVER $BIN_CLIENT $BIN_LOGGER
