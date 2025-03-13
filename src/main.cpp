#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include <mutex>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = asio::ip::tcp;
using json = nlohmann::json;

const std::string CLIENT_ID = "3EEJ0UIl";
const std::string CLIENT_SECRET = "atuJ9LDP8mvi-d4y4qyy0OtkUkCUavgSNGeZZj76rlA";

class DeribitClient {
private:
    asio::io_context io_context;
    asio::ssl::context ssl_context;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;
    std::string access_token;
    std::mutex ws_mutex;
    std::thread io_thread;

public:
    DeribitClient()
        : ssl_context(asio::ssl::context::tlsv12_client),
          ws(io_context, ssl_context) {
        connect();
        authenticate();
        io_thread = std::thread([this]() { io_context.run(); });
    }

    ~DeribitClient() {
        ws.close(websocket::close_code::normal);
        if (io_thread.joinable()) io_thread.join();
    }

    void connect() {
        try {
            tcp::resolver resolver(io_context);
            auto results = resolver.resolve("test.deribit.com", "443");
            beast::get_lowest_layer(ws).connect(results);
            ws.next_layer().handshake(asio::ssl::stream_base::client);
            ws.handshake("test.deribit.com", "/ws/api/v2");

            std::cout << "✅ Connected to Deribit WebSocket API\n";
        } catch (const std::exception& e) {
            std::cerr << "❌ Connection error: " << e.what() << std::endl;
            exit(1);
        }
    }

    void authenticate() {
        json auth_request = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "public/auth"},
            {"params", {
                {"grant_type", "client_credentials"},
                {"client_id", CLIENT_ID},
                {"client_secret", CLIENT_SECRET}
            }}
        };

        sendMessage(auth_request.dump());
        std::string response = receiveMessage();
        json json_response = json::parse(response);

        if (json_response.contains("result") && json_response["result"].contains("access_token")) {
            access_token = json_response["result"]["access_token"];
            std::cout << "✅ Authentication successful! Access Token: " << access_token << "\n";
        } else {
            std::cerr << "❌ Authentication failed!\n";
            exit(1);
        }
    }

    void sendMessage(const std::string& message) {
        std::lock_guard<std::mutex> lock(ws_mutex);
        ws.write(asio::buffer(message));
    }

    std::string receiveMessage() {
        beast::flat_buffer buffer;
        ws.read(buffer);
        return beast::buffers_to_string(buffer.data());
    }

    std::string placeOrder(const std::string& side, const std::string& instrument, double amount) {
        json order_request = {
            {"jsonrpc", "2.0"},
            {"id", 2},
            {"method", side == "buy" ? "private/buy" : "private/sell"},
            {"params", {
                {"instrument_name", instrument},
                {"amount", amount},
                {"type", "market"},
                {"label", "test_order"},
                {"access_token", access_token}
            }}
        };

        sendMessage(order_request.dump());
        std::string response = receiveMessage();
        json json_response = json::parse(response);

        if (json_response.contains("result") && json_response["result"].contains("order")) {
            std::string order_id = json_response["result"]["order"]["order_id"];
            std::cout << "✅ Order Successful! Order ID: " << order_id << "\n";
            return order_id;
        } else {
            std::cerr << "❌ Order Failed!\n";
            return "";
        }
    }

    void cancelOrder(const std::string& order_id) {
        json cancel_request = {
            {"jsonrpc", "2.0"},
            {"id", 3},
            {"method", "private/cancel"},
            {"params", {
                {"order_id", order_id},
                {"access_token", access_token}
            }}
        };

        sendMessage(cancel_request.dump());
        std::string response = receiveMessage();
        std::cout << "Order Cancel Responcse: " << response << std::endl;
        json json_response = json::parse(response);

        if (json_response.contains("result") && json_response["result"]["order_state"] == "cancelled") {
            std::cout << "✅ Order Cancelled Successfully!\n";
        } else {
            std::cerr << "❌ Order Cancellation Failed!\n";
        }
    }
    // Function to modify an order
    void modifyOrder(const std::string& order_id, double new_amount, double new_price) {
        json modify_request = {
            {"jsonrpc", "2.0"},
            {"id", 4},
            {"method", "private/edit"},
            {"params", {
                {"order_id", order_id},
                {"amount", new_amount},
                {"price", new_price},
                // {"advanced", "implv"},
                {"access_token", access_token}
            }}
        };

        std::cout << "Modifying order: " << order_id << "\n";
        sendMessage(modify_request.dump());

        std::string response = receiveMessage();
        std::cout << "Modify Response: " << response << "\n";

        json json_response = json::parse(response);
        if (json_response.contains("result") && json_response["result"].contains("order")) {
            std::cout << "Order Modified Successfully! New Order ID: " << json_response["result"]["order"]["order_id"] << "\n";
        } else {
            std::cerr << "Order Modification Failed!\n";
        }
    }

    // Function to subscribe to market data (ticker and order book)
    void subscribeMarketData(const std::string& instrument) {
        json subscribe_request = {
            {"jsonrpc", "2.0"},
            {"id", 42},
            {"method", "public/subscribe"},
            {"params", {
                {"channels", {
                    "ticker." + instrument + ".raw",
                    "book." + instrument + ".10"
                }}
            }}
        };

        std::cout << "Subscribing to market data for " << instrument << "...\n";
        sendMessage(subscribe_request.dump());

        std::string response = receiveMessage();
        std::cout << "Subscription Response: " << response << "\n";
    }

    // Function to continuously receive and process market data
    void processMarketData() {
        while (true) {
            std::string message = receiveMessage();
            std::cout << "Subscription Response: " << message << "\n";
            // json json_message = json::parse(message);

            // if (json_message.contains("params") && json_message["params"].contains("channel")) {
            //     std::string channel = json_message["params"]["channel"];
                
            //     if (channel.find("ticker") != std::string::npos) {
            //         // Process ticker updates
            //         double last_price = json_message["params"]["data"]["last_price"];
            //         std::cout << "Ticker Update: Last Price = " << last_price << "\n";
            //     } else if (channel.find("book") != std::string::npos) {
            //         // Process order book updates
            //         auto bids = json_message["params"]["data"]["bids"];
            //         auto asks = json_message["params"]["data"]["asks"];
                    
            //         std::cout << "Order Book Update:\n";
            //         std::cout << " Bids: ";
            //         for (const auto& bid : bids) {
            //             std::cout << "[" << bid[1] << "@" << bid[0] << "] ";
            //         }
            //         std::cout << "\n  Asks: ";
            //         for (const auto& ask : asks) {
            //             std::cout << "[" << ask[1] << "@" << ask[0] << "] ";
            //         }
            //         std::cout << "\n";
            //     }
            // }
        }
    }

    void getPositions(const std::string& currency, const std::string& kind) {
        json position_request = {
            {"jsonrpc", "2.0"},
            {"id", 1234},
            {"method", "private/get_positions"},
            {"params", {
                {"currency", currency},
                {"kind", kind},
                {"access_token", access_token}
            }}
        };

        sendMessage(position_request.dump());
        std::string response = receiveMessage();
        json json_response = json::parse(response);

        if (json_response.contains("result") && !json_response["result"].empty()) {
            std::cout << "✅ Open Positions:\n";
            for (const auto& position : json_response["result"]) {
                std::cout << "Instrument: " << position["instrument_name"]
                          << " | Size: " << position["size"]
                          << " | Entry Price: " << position["average_price"]
                          << " | PnL: " << position["floating_profit_loss"] << "\n";
            }
        } else {
            std::cout << "ℹ No open positions.\n";
        }
    }
};

int main() {
    try {
        DeribitClient client;

        // Place an order
        std::string order_id = client.placeOrder("buy", "BTC-21MAR25", 10);

        // Cancel order if it was placed successfully
        if (!order_id.empty()) {
            client.cancelOrder(order_id);
        }

        // Subscribe to market data for ETH-PERPETUAL
        client.subscribeMarketData("ETH-PERPETUAL");

        // Process incoming market data
        client.processMarketData();

        // Get positions
        client.getPositions("ETH", "future");

    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }

    return 0;
}
