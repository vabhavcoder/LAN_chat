#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <string>       
#include <thread>       

// --- Background thread to constantly listen for messages ---
void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        for(int i = 0; i < 1024; i++) buffer[i] = '\0';
        
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        if (bytes_received > 0) {
            // \r teleports cursor to the start of the line, wiping out the old prompt
            std::cout << "\r" << buffer;
            // Re-print the green prompt on the fresh line so the user can keep typing
            std::cout << "\033[32mYou: \033[0m" << std::flush;
        } else {
            std::cout << "\nServer disconnected!" << std::endl;
            exit(0); // Kill the client if the server crashes
        }
    }
}

int main() {
    std::cout << "Starting CppChat Client..." << std::endl;

    // --- Ask the user for the Server IP ---
    std::string server_ip;
    std::cout << "Enter Server IP (Press Enter for localhost/127.0.0.1): ";
    std::getline(std::cin, server_ip);

    // If the user just presses Enter, default to localhost
    if (server_ip.empty()) {
        server_ip = "127.0.0.1";
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error: Failed to create socket!" << std::endl;
        return -1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    
    if (inet_pton(AF_INET, server_ip.c_str(), &server_address.sin_addr) <= 0) {
        std::cerr << "Error: Invalid IP address format!" << std::endl;
        close(client_socket);
        return -1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error: Connection to " << server_ip << " failed! Is the server running?" << std::endl;
        close(client_socket);
        return -1;
    }

    // --- Ask for username and send it to the server ---
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    send(client_socket, username.c_str(), username.size(), 0);

    std::cout << "\nWelcome, " << username << "! (Type 'quit' to exit)\n" << std::endl;

    // --- Start the background listening thread ---
    std::thread receiver_thread(receive_messages, client_socket);
    receiver_thread.detach();

    // --- MAIN LOOP: Only handles typing and sending ---
    while (true) {
        std::string message;
        // Print the green prompt for the user
        std::cout << "\033[32mYou: \033[0m";
        std::getline(std::cin, message); 

        if (message == "quit") {
            break;
        }

        // Only send if the message is not empty
        if (!message.empty()) {
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }

    close(client_socket);
    std::cout << "Disconnected." << std::endl;
    return 0;
}
