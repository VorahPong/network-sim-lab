#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void run_receiver(int listen_port) {
    // AF_INET: IPV4 protocol
    // SOCK_DGRAM: UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); 

    if (sockfd < 0) {
        std::cerr << "Failed to create socket\n";
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(listen_port);     // htons(): converts port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket\n";
        close(sockfd);
        return;
    }

    std::cout << "Monitor listening on UDP port " << listen_port << "...\n";

    char buffer[1024];

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        ssize_t bytes_received = recvfrom(
            sockfd,
            buffer,
            sizeof(buffer) - 1,
            0,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_len
        );

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Received: " << buffer << "\n";
        }
    }

    close(sockfd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./sim_node receiver <listen_port>\n";
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "receiver") {
        int listen_port = std::stoi(argv[2]);
        run_receiver(listen_port);
    } else {
        std::cerr << "Invalid mode. Use: receiver\n";
        return 1;
    }

    return 0;

}