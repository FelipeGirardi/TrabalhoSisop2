# Overview
University assignment for Operating Systems 2 class. This is a client/server Twitter-like application structured in `client_app`, `server_app` and `frontend_app` modules. Runs only on Unix/Linux. Video-demos are available at: [YouTube: Part 1](https://www.youtube.com/watch?v=kvTAINipn80) and [YouTube: Part 2](https://youtu.be/wTaDFyPwLgo).

| Class               | Course           | Semester | University               |
|:-------------------:|:----------------:|:--------:|:------------------------:|
| Operating Systems 2 | Computer Science | 2021/1   | [UFRGS](http://ufrgs.br) |

# Pre-requirements

| Name    | Description                         | Ubuntu Command                 |
|---------|-------------------------------------|--------------------------------|
| G++     | GNU Compiler for C++                | `sudo apt install g++`         |
| Make    | Build tool                          | `sudo apt install make`        |
| NCurses | Console manipulation library        | `sudo apt install ncurses-bin` |
| UUID    | Unique identifier generator library | `sudo apt install uuid-dev`    |

# How to compile?
Run `make` in the project directory. You can also compile individual subprojects by running `make` in the subproject directory.

# How to run?
**It's required to run at least one instance of `frontendApp` before the other apps!**

To run the frontend application:
``` bash
./app_frontend/bin/frontendApp <host> <port-for-servers> <port-for-clients>

# -- Example --
# ./app_frontend/bin/frontendApp 127.0.0.10 4001 5000
```

To run the server application:
``` bash
./app_server/bin/serverApp <instance-id> <server-port>

# -- Example --
# ./app_server/bin/serverApp 0 4000
```

To run the client application:
``` bash
./app_client/bin/clientApp <profile> <frontend-host> <frontend-port-for-clients>

# -- Example --
# ./app_client/bin/clientApp @johndoe 127.0.0.10 5000
```
