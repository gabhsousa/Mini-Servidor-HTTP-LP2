#!/bin/bash
# Script de build e execução do projeto

echo "Compilando..."
g++ -std=c++17 -Wall -pthread main.cpp Logger.cpp -o test_logger

if [ $? -eq 0 ]; then
  echo "Compilação concluída com sucesso!"
  echo "Executando..."
  ./test_logger
else
  echo "Erro na compilação."
fi
