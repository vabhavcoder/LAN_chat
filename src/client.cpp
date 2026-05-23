#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <string>       
#include <thread>       // <-- NEW: For background listening

// --- NEW: Background thread to constantly listen for messages ---
void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        for(int i = 0; i < 1024; i++) buffer[i] = '\0';
        
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        if (bytes_received > 0) {
            // Print the incoming message
            std::cout << "\r" << buffer;
            // Re-print the "You: " prompt so the user knows they can still type
            std::cout << "You: " << std::flush;
        } else {
            std::cout << "\nServer disconnected!" << std::endl;
            exit(0); // Kill the client if the server crashes
        }
    }
}
int main() {
    std::cout << "Starting CppChat Client..." << std::endl;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    inet_pton(AF_INET, "172.31.222.8", &server_address.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "Error: Connection failed!" << std::endl;
        return -1;
    }

    // --- NEW: Ask for username and send it to the server ---
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    send(client_socket, username.c_str(), username.size(), 0);

    std::cout << "\nWelcome, " << username << "! (Type 'quit' to exit)\n" << std::endl;

    std::thread receiver_thread(receive_messages, client_socket);
    receiver_thread.detach();

    while (true) {
        std::string message;
        std::cout << "You: ";
        std::getline(std::cin, message); 

        if (message == "quit") {
            break;
        }

        send(client_socket, message.c_str(), message.size(), 0);
    }

    close(client_socket);
    std::cout << "Disconnected." << std::endl;
    return 0;
}
