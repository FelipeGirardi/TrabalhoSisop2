# Overview
University assignment for Operating Systems 2 class. This is a client/server Twitter-like application structured in `client_app` and `server_app` modules. Runs only on Unix/Linux.

| Class               | Course           | Semester | University               |
|:-------------------:|:----------------:|:--------:|:------------------------:|
| Operating Systems 2 | Computer Science | 2021/1   | [UFRGS](http://ufrgs.br) |

# Pre-requirements

| Name    | Description                  | Ubuntu Command             |
|---------|------------------------------|----------------------------|
| G++     | GNU Compiler for C++         | `sudo apt install g++`     |
| Make    | Build tool                   | `sudo apt install make`    |
| NCurses | Console manipulation library | `sudo apt install ncurses` |

# How to compile?
Run `make` in the project directory. You can also compile individual subprojects by running `make` in the subproject directory.

# How to run?
To run the server application:

``` bash
./<project-directory>/app_server/bin/serverApp
```

To run the client application:
``` bash
./<project-directory>/app_client/bin/clientApp
```
