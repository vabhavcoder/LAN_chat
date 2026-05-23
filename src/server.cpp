#include <iostream>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>     
#include <string>       
#include <thread>       
#include <vector>       // <-- NEW: To store a list of clients
#include <mutex>        // <-- NEW: To prevent thread crashes
#include <algorithm>    // <-- NEW: For finding/removing clients

// --- GLOBAL VARIABLES ---
std::vector<int> client_list;
std::mutex clients_mutex;

// --- NEW: Send a message to everyone EXCEPT the sender ---
void broadcast_message(const std::string& message, int sender_socket) {
    // Lock the list so no one can join/leave while we are sending
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    for (int client : client_list) {
        if (client != sender_socket) { // Don't echo the message back to the sender
            send(client, message.c_str(), message.size(), 0);
        }
    }
}

// --- UPDATED CLIENT HANDLER ---
void handle_client(int client_socket) {
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_list.push_back(client_socket);
    }
    
    // --- NEW: Wait for the client's first message (their username) ---
    char name_buffer[1024] = {0};
    recv(client_socket, name_buffer, sizeof(name_buffer), 0);
    std::string username = std::string(name_buffer);
    
    std::cout << "\n[" << username << "] joined the chat!" << std::endl;
    
    // Announce to everyone that a new user joined
    std::string join_msg = "\n*** " + username + " has joined the chat! ***\nYou: ";
    broadcast_message(join_msg, client_socket);
    
    char buffer[1024]; 
    while (true) {
        for(int i = 0; i < 1024; i++) buffer[i] = '\0';
        
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        if (bytes_received > 0) {
            std::cout << "[" << username << "]: " << buffer << std::endl;
            
            // --- NEW: Attach their username to their messages ---
            std::string msg_to_send = "\r" + username + ": " + std::string(buffer) + "\n";
            broadcast_message(msg_to_send, client_socket);
        } 
        else {
            std::cout << "[" << username << "] disconnected." << std::endl;
            std::string leave_msg = "\n*** " + username + " has left the chat. ***\nYou: ";
            broadcast_message(leave_msg, client_socket);
            break; 
        }
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_list.erase(std::remove(client_list.begin(), client_list.end(), client_socket), client_list.end());
    }
    close(client_socket); 
}

int main() {
    std::cout << "Starting CppChat Server..." << std::endl;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    std::cout << "Server is listening on port 8080... Waiting for connections." << std::endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == -1) continue; 
        
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach(); 
    }

    close(server_socket);
    return 0;
}
