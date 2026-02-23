# Personal-Shell (`mysh`)

A custom Unix-like shell written in C.

`mysh` supports interactive command execution, custom builtins, variable expansion, pipelines, background jobs, signal handling, and simple TCP messaging commands.

---

## What this project does

This repository contains a small shell implementation that can:

- Run built-in commands implemented in C.
- Run external programs via `fork + execvp`.
- Store and expand shell-style variables with `$name`.
- Pipe commands with `|`.
- Run commands in the background using `&` and inspect/kill jobs.
- Handle `Ctrl+C` without killing the shell.
- Start a basic TCP server/client and send messages between shells.

---

## Feature overview

### 1) Built-in commands

The shell includes these builtins:

- `echo`
- `ls`
- `cd`
- `cat`
- `wc`
- `ps`
- `kill`
- `start-server`
- `send`
- `start-client`
- `close-server`

If the command is not a builtin, `mysh` tries to execute it as a normal system command.

### 2) Variables

- Define variables with:
  - `name=value`
- Use variables with:
  - `$name`

Examples:

```sh
mysh$ greeting=hello
mysh$ echo $greeting world
hello world
```

### 3) Pipes

Commands can be chained with `|`.

Examples:

```sh
mysh$ echo hello world | wc
mysh$ ls | cat
```

### 4) Background jobs

Append `&` to run a command in the background:

```sh
mysh$ sleep 10 &
```

Use:

- `ps` to list running background jobs tracked by the shell.
- `kill <pid> [signal]` to terminate/signals jobs.

### 5) Networking commands

The shell includes simple socket-based messaging support:

- `start-server <port>`: start a TCP server loop.
- `send <port> <host> <message...>`: connect and send one message.
- `start-client <port> <host>`: interactive client mode.
- `close-server`: close the currently running server socket.

---

## Build and run

From the repository root:

```sh
cd personal_shell/src
make
./mysh
```

To clean artifacts:

```sh
make clean
```

The provided `Makefile` compiles with:

- debug symbols (`-g`)
- warnings as errors (`-Wall -Werror`)
- AddressSanitizer (`-fsanitize=address`)

---

## Usage examples

### Basic command execution

```sh
mysh$ pwd
mysh$ ls
mysh$ /bin/echo works too
```

### Variables

```sh
mysh$ dir=.
mysh$ ls $dir
```

### `ls` options supported in this shell

Custom `ls` supports:

- `--rec` for recursive listing
- `--d <depth>` for recursion depth
- `--f <substring>` for filtering names by substring

Example:

```sh
mysh$ ls --rec . --d 2 --f src
```

### Networking quick demo (two terminals)

Terminal 1:

```sh
mysh$ start-server 9090
```

Terminal 2:

```sh
mysh$ send 9090 127.0.0.1 hello from client
```

---

## Project layout

```text
personal_shell/
├── src/
│   ├── mysh.c            # main REPL loop and dispatch
│   ├── builtins.c/.h     # builtin command implementations
│   ├── commands.c/.h     # piping, bg jobs, socket read/write helpers
│   ├── variables.c/.h    # variable storage and lookup
│   ├── io_helpers.c/.h   # input/output helpers and tokenization
│   ├── other_helpers.c/.h# server/client socket setup helpers
│   └── Makefile
└── testlog.txt           # historical grading/test run log
```

---

## Notes and constraints

- Maximum input line length is 64 characters (`MAX_STR_LEN`).
- Tokenization is whitespace-based; quoting/escaping behavior is minimal.
- Variable declarations are simple (`name=value`) and do not support spaces around `=`.
- This is an educational shell project and does not aim to be POSIX-complete.

---

## Why this project is useful

This codebase is a practical reference for learning shell fundamentals in C:

- process creation (`fork`, `execvp`, `wait`)
- redirection through pipes (`pipe`, `dup2`)
- job control basics
- signal handling (`sigaction`)
- socket programming (`socket`, `bind`, `listen`, `accept`, `connect`)

If you're studying systems programming, this project gives you a compact end-to-end shell implementation to explore and extend.
