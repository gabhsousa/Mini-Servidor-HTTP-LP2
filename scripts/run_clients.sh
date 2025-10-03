#!/usr/bin/env bash
set -e
HOST="${1:-127.0.0.1}"
PORT="${2:-5555}"
N="${3:-3}"

# inicia N clientes; você pode digitar em qualquer terminal
for i in $(seq 1 "$N"); do
  stdbuf -o0 -e0 ./chat_client "$HOST" "$PORT" &
done

echo "Clientes iniciados: $N (HOST=$HOST PORT=$PORT)."
echo "Dica: digite mensagens em qualquer uma das janelas/terminais de cliente."
