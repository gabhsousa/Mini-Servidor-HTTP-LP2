#!/usr/bin/env bash
set -e
PORT="${1:-8080}"
./chat_server "$PORT"
