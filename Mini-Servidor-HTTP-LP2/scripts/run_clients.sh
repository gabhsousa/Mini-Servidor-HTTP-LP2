#!/usr/bin/env bash
set -e

HOST="${1:-127.0.0.1}"
PORT="${2:-8080}"
N="${3:-5}"

echo "Iniciando $N clientes simultâneos (host=$HOST, porta=$PORT)..."

for i in $(seq 1 "$N"); do
  echo "→ Cliente $i fazendo requisição..."
  curl -s "http://$HOST:$PORT/" -o /dev/null &
done

wait
echo "✅ Todos os $N clientes concluíram as requisições."
