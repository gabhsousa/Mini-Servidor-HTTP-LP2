# Mini Servidor Web (HTTP) 

## Etapa 1 — Biblioteca de Logging (v1-logging)

### Objetivos
- Implementar biblioteca de logging **thread-safe** com API clara.
- Teste CLI simulando múltiplas threads gravando logs.

### Implementação
- Arquivos principais:
  - `Logger.h` / `Logger.cpp`: biblioteca de logging baseada em Singleton + `std::mutex`.
  - `main.cpp`: programa de teste que cria múltiplas threads e grava mensagens de log concorrentes.

### Funcionalidades
- Níveis de log: **INFO, ERROR, DEBUG**.
- Saída para **console** e para arquivo **server.log**.
- Garantia de segurança em concorrência via `std::mutex`.

### Teste
```bash
./test_logger
```
Saída esperada: mensagens intercaladas de várias threads no console e no arquivo `server.log`.

---

## Etapa 2 — Protótipo CLI de Comunicação (v2-cli)

### Objetivos
- Implementar cliente/servidor mínimo em rede (exemplo: chat).
- Integrar logging da Etapa 1.
- Fornecer scripts de teste para múltiplos clientes.

### Implementação
- Arquivos principais:
  - `chat_server.cpp`: servidor TCP concorrente. Aceita múltiplos clientes (thread por conexão), retransmite mensagens para todos e registra logs.
  - `chat_client.cpp`: cliente TCP que conecta ao servidor, envia e recebe mensagens.
  - `Logger.h` / `Logger.cpp`: logging thread-safe integrado.
  - Scripts: 
    - `build.sh`: compila os binários.
    - `run_server.sh`: executa servidor na porta padrão (5555).
    - `run_clients.sh`: executa múltiplos clientes de teste.
    - `soak_test.sh`: simula clientes automáticos gerando carga contínua.

### Teste Rápido
1. Compile:
```bash
./build.sh
```
2. Inicie servidor:
```bash
./run_server.sh
```
3. Inicie clientes (exemplo: 3 clientes na porta 5555):
```bash
./run_clients.sh 127.0.0.1 5555 3
```
4. Opcional: rodar teste de carga
```bash
./soak_test.sh
```

### Logs
- Todas as conexões e mensagens são registradas em `server.log` e exibidas no console.

---

## Status
- Etapa 1 concluída (biblioteca de logging + teste multi-thread).
- Etapa 2 concluída (protótipo cliente/servidor + logging + scripts).

---
