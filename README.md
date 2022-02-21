# HTTP server in C
Minimal example of a HTTP server using the POSIX *sockets* library, written in C.

I created this project simply to see how difficult it is to write a HTTP server using C. Would I recommend it? Unless it is a simple server, I would say... "no".

However I think that this repo is useful as a template for a modern C project that includes unit testing for test-driven development.

I organised this project with a more modern, yet minimal project structure; featuring unit testing using the lightweight library [Unity](https://github.com/ThrowTheSwitch/Unity).

To remain minimalistic, this project only uses [Make](https://www.gnu.org/software/make/) for it's buildsystem. The `makefile` makes use of Make's "[pattern rules](https://www.gnu.org/software/make/manual/html_node/Pattern-Rules.html)" so that you don't need to individually include a file in both the *runtime* and *unit-testing* builds every time you add a new file to the project.

# Dependencies
- A POSIX-compliant shell for commands used in `makefile` and `get-dependencies.sh`.
- *Make* for building the project.
- *Git* for downloading the external libraries used in the project.
- A POSIX-compliant C compiler (duh!) - edit `makefile` to call your compiler of choice.

# First-Time Setup - Downloading External Libraries
1. Run `get-dependencies.sh` to download the external unit-testing library *Unity*. It will be extracted and saved to the `lib/` directory.

# Building
After that, all that is required is to run `make` in the root directory! The final executable will be saved in `bin/`.

To build the unit tests, run `make test`.

# Server Behaviour
The server listens for a single endpoint at `http://localhost:8080/readyz` where it simply returns a `200 OK` response.

If any other path is accessed on port `8080`, the server returns `404 Not Found` status.

# TO-DO
- Write a client (in C, of course) that can make calls to the server.
