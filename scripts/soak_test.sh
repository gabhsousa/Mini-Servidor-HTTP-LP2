#!/usr/bin/env bash
set -e
HOST="${1:-127.0.0.1}"
PORT="${2:-5555}"

# dois clientes “robôs”
( while true; do echo "alpha: $(date +%H:%M:%S)"; sleep 0.7; done ) | ./chat_client "$HOST" "$PORT" &
( while true; do echo "beta:  $(date +%H:%M:%S)"; sleep 1.1; done ) | ./chat_client "$HOST" "$PORT" &

echo "Soak test rodando. Pare com: killall chat_client