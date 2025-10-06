#!/usr/bin/env bash
set -e
HOST="${1:-127.0.0.1}"
PORT="${2:-8080}"

( while true; do echo "GET / HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n" | nc $HOST $PORT; sleep 0.5; done ) &
( while true; do echo "GET / HTTP/1.1\r\nHost: $HOST:$PORT\r\n\r\n" | nc $HOST $PORT; sleep 0.7; done ) &
