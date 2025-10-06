#!/usr/bin/env bash
set -e

PORT="${1:-8080}"

echo "🌐 Iniciando servidor HTTP na porta $PORT..."
../http_server "$PORT"
