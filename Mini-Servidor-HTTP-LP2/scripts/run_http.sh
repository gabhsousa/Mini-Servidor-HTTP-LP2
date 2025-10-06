#!/usr/bin/env bash
set -e

PORT="${1:-8080}"

echo "Iniciando servidor HTTP na porta ${PORT}..."
# Executa o binário diretamente da raiz
"$(dirname "$0")/../http_server" "$PORT"
