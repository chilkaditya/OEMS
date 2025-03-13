# OEMS for Derbit

This project implements a C++ client for interacting with the Deribit trading platform's API. It enables users to authenticate with the API, retrieve the order book, and place buy/sell orders on the Deribit test network using JSON-RPC over Websocket.


## Features

- **Authentication**: OAuth 2.0 authentication to obtain an access token using `client_id` and `client_secret`.
- **Public API**: Fetches order book details for a subscribed instrument,fetches current position and market data.
- **Private API**: Places buy, sell,modify and cancel orders.

## Dependencies

This project uses the following libraries:

- `nlohmann/json`: For JSON parsing.
- `libssl-dev`: Required for WebSocket TLS encryption.
- `libboost-all-dev`: Provides Boost.Asio for WebSocket handling.


### Installing dependencies for Ubuntu
   1. Package manager for c++.
   ```bash
    sudo apt update && sudo apt upgrade -y
    sudo apt install -y build-essential cmake git curl wget libssl-dev libboost-all-dev
    sudo apt install -y nlohmann-json3-dev
    sudo apt install -y libboost-dev

   ```
## Setup & Run
1. Clone the repo.
   ```bash
   git clone https://github.com/chilkaditya/OEMS.git
   cd OEMS
   ```
2. Build using Cmake (you can use your fav build system).
   ```bash
   makdir build
   cd build
   cmake ..
   make -j$(nproc)
   ./your_exe_file.exe
   ```



