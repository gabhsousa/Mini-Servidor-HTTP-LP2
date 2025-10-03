#!/usr/bin/env bash
set -e
PORT="${1:-5555}"
./chat_server "$PORT"
