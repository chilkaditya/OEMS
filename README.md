# OEMS for Derbit

This project implements a C++ client for interacting with the Deribit trading platform's API. It enables users to authenticate with the API, retrieve the order book, and place buy/sell orders on the Deribit test network using JSON-RPC over Websocket.


## Features

- **Authentication**: OAuth 2.0 authentication to obtain an access token using `client_id` and `client_secret`.
- **Public API**: Fetches order book details for a specified instrument.
- **Private API**: Places buy, sell,modify and cancel orders.

## Dependencies

This project uses the following libraries:

- `nlohmann/json`: For JSON parsing.
- `libssl-dev`: Required for WebSocket TLS encryption.
- `libboost-all-dev`: Provides Boost.Asio for WebSocket handling.


### Installing vcpkg
   1. Package manager for c++.
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

### Installing Dependencies with vcpkg
   1. Install `vcpkg` following the instructions on [vcpkg GitHub](https://github.com/microsoft/vcpkg).
   2. Install dependencies:

      ```bash
      vcpkg install curl
      vcpkg install nlohmann-json
      ```

## Setup & Run
1. Clone the repo.
   ```bash
   git clone https://github.com/chilkaditya/DeribitTrader.git
   cd DeribitTrader
   ```
2. Build using make (you can use your fav build system).
   ```bash
   make
   ./your_exe_file.exe
   ```



