# Overview
University assignment for Operating Systems 2 class. This is a client/server Twitter-like application structured in `client_app` and `server_app` modules. Runs only on Unix/Linux. A video-demo can be seen on [this link](https://www.youtube.com/watch?v=kvTAINipn80).

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
To run the server application:
``` bash
./app_server/bin/serverApp
```

To run the client application:
``` bash
./app_client/bin/clientApp <profile> 127.0.0.1 4000

# -- Example --
# ./app_client/bin/clientApp @johndoe 127.0.0.1 4000
```
