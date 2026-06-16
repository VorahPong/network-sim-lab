#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <thread>

#include <fstream>
#include <filesystem>

void run_receiver(int listen_port)
{
    // AF_INET: IPV4 protocol
    // SOCK_DGRAM: UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port); // htons(): converts port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        std::cerr << "Failed to bind socket\n";
        close(sockfd);
        return;
    }

    // create log folder and sim.log file to log receive message
    std::filesystem::create_directories("logs");
    std::ofstream log_file("logs/sim.log", std::ios::app);

    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file\n";
        close(sockfd);
        return;
    }

    std::cout << "Monitor listening on UDP port " << listen_port << "...\n";

    char buffer[1024];

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        ssize_t bytes_received = recvfrom(
            sockfd,
            buffer,
            sizeof(buffer) - 1,
            0,
            reinterpret_cast<sockaddr *>(&client_addr),
            &client_len);

        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            std::string log_entry = std::string("Received: ") + buffer;
            std::cout << log_entry << "\n";
            log_file << log_entry << std::endl;
            std::cout << "Received: " << buffer << "\n";
        }
    }

    close(sockfd);
}

void run_sender(const std::string &node_name, const std::string &dest_ip, int dest_port)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);

    if (inet_pton(AF_INET, dest_ip.c_str(), &dest_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid destination IP address\n";
        close(sockfd);
        return;
    }

    int seq = 1;
    std::string type = "HEARTBEAT";
    if (node_name == "radar") {
        type = "TRACK_UPDATE";
    }
    else if (node_name == "command") {
        type = "STATUS_REQUEST";
    }
    else if (node_name == "sensor") {
        type = "SENSOR_READING";
    }

    while (true)
    {
        std::string message = "NODE=" + node_name + ";TYPE=" + type + ";SEQ=" + std::to_string(seq);

        ssize_t bytes_sent = sendto(sockfd, message.c_str(), message.size(), 0, reinterpret_cast<sockaddr *>(&dest_addr), sizeof(dest_addr));

        if (bytes_sent < 0)
        {
            std::cerr << "Failed to send message\n";
        }
        else
        {
            std::cout << "Sent: " << message << "\n";
        }

        seq++;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(sockfd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: \n";
        std::cout << "Receiver: ./sim_node receiver <listen_port>\n";
        std::cout << "Sender:   ./sim_node sender <node_name> <dest_ip> <dest_port>\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "receiver")
    {
        if (argc != 3) {
            std::cerr << "Usage: ./sim_node receiver <listen_port>\n";
            return 1;
        }

        int listen_port = std::stoi(argv[2]);
        run_receiver(listen_port);
    }
    else if (mode == "sender")
    {
        if (argc != 5) {
            std::cerr << "Usage: ./sim_node sender <node_name> <dest_ip> <dest_port>\n";
            return 1;
        }
        std::string node_name = argv[2];
        std::string dest_ip = argv[3];
        int dest_port = std::stoi(argv[4]);

        run_sender(node_name, dest_ip, dest_port);
    }
    else {
        std::cerr << "Invalid mode.\n";
        return 1;
    }

    return 0;
}