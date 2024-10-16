# Nodepp-WASM

Nodepp Port for Emscripten 

## Dependencies 
```bash
#emscripten
    🪟: pacman -S mingw-w64-ucrt-x86_64-emscripten
    🐧: sudo apt install emscripten
```

## Features

- 📌: **C++11 | C++14 | C++17 | c++20 | c++23** Compatible
- 📌: Allows writing **C++** code as if writing in **NodeJS** or **Javascript**
- 📌: Adds a high-level abstraction layer to simplify **C++** application development
- 📌: Supports **Coroutines**, which allows running multiple tasks concurrently on a single thread

## Bateries Included

- 📌: Include a **build-in JSON** parser / stringify system.
- 📌: Include a **build-in RegExp** engine for processing text strings.
- 📌: Include a **build-in System** that make every object **Async Task** safety.
- 📌: Include Support for **Poll** making it easy to handle multiple file descriptors.
- 📌: Include a **Smart Pointer** base **Garbage Collector** to avoid **Memory Leaks**.
- 📌: Include support for **HTTP | WS** making it easy to create networked applications.
- 📌: Include support for **Reactive Programming** based on **Events** and **Observers**.
- 📌: Include an **Event Loop** that can handle multiple events and tasks on a single thread.

## Build & Run

```bash
em++ -o www/index.html main.cpp \
     -I ./include -pthread -lwebsocket.js \
     -s WEBSOCKET_SUBPROTOCOL=1 \
     -s PTHREAD_POOL_SIZE=8     \
     -s WEBSOCKET_URL=1         \
     -s USE_PTHREADS=1          \
     -s ASYNCIFY=1              \
     -s FETCH=1                 \
     -s WASM=1                  \
    --embed-file ./www/index.html
```

## Examples
### Hello world
```cpp
#include <nodepp/nodepp.h>

using namespace nodepp;

void onMain() {
    console::log("Hello World!");
}
```

### HTTP Client
```cpp
#include <nodepp/nodepp.h>
#include <nodepp/fetch.h>

using namespace nodepp;

void onMain() {

    fetch_t args;
            args.url = "https://localhost:8000/";

    fetch::add( args )

    .then([=]( fetch_t res ){
        console::log( "->", res.status );
        console::log( res.headers["Content-Length"] );
    })

    .fail([=]( except_t rej ){
        console::log( rej );
    });

}
```

### Websocket Client
```cpp
#include <nodepp/nodepp.h>
#include <nodepp/ws.h>

using namespace nodepp;

void onMain() {

    auto srv = ws::connect( "wss://localhost:8000" );
    
    srv.onConnect([=]( ws_t cli ){

        cli.onData([]( string_t data ){
            console::log( data );
        });

        cli.onClose([](){
            console::log( "done" );
        });

    });

}
```

### More Examples [here](https://github.com/NodeppOficial/Nodepp/tree/main/examples)

## Compatibility
- 🔗: [NodePP for Window | Linux | Mac | Bsd ](https://github.com/NodeppOficial/nodepp)
- 🔗: [NodePP for Arduino](https://github.com/NodeppOficial/nodepp-arduino)
- 🔗: [Nodepp for WASM](https://github.com/NodeppOficial/nodepp-wasm)
 
## FAQ
- 🔗: [/r/Nodepp/](https://www.reddit.com/r/Nodepp/comments/1eaq1pu/faq_ask_anything_about_nodepp/)
  
## Contribution

If you want to contribute to **Nodepp**, you are welcome to do so! You can contribute in several ways:

- ☕ Buying me a Coffee
- 📢 Reporting bugs and issues
- 📝 Improving the documentation
- 📌 Adding new features or improving existing ones
- 🧪 Writing tests and ensuring compatibility with different platforms
- 🔍 Before submitting a pull request, make sure to read the contribution guidelines.

**NOTE:** We don't use STD here, just Pure C++.

## License

**Nodepp** is distributed under the MIT License. See the LICENSE file for more details.
