#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cerrno>
#include <sstream>

// Function to convert IP address to string (alternative to inet_ntoa)
std::string ipToString(const struct sockaddr_in& addr) {
	unsigned char* ip = (unsigned char*)&addr.sin_addr.s_addr;
	std::stringstream ss;
	ss << (int)ip[0] << "." << (int)ip[1] << "." << (int)ip[2] << "." << (int)ip[3];
	return ss.str();
}

int main() {
	int server_fd;
	struct sockaddr_in server_addr;
	const int PORT = 12344;
	std::vector<struct pollfd> poll_fds;
	char buffer[1024];

	// Create socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
		return EXIT_FAILURE;
	}

	// Set socket options to reuse address
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		std::cerr << "Failed to set socket options: " << strerror(errno) << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}

	// Bind to port
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		std::cerr << "Bind failed: " << strerror(errno) << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}

	// Listen for connections
	if (listen(server_fd, 10) == -1) {
		std::cerr << "Listen failed: " << strerror(errno) << std::endl;
		close(server_fd);
		return EXIT_FAILURE;
	}
	// Add server socket to poll fds
	struct pollfd server_poll_fd;
	server_poll_fd.fd = server_fd;
	server_poll_fd.events = POLLIN;
	poll_fds.push_back(server_poll_fd);

	while (true) {
		// Wait for events
		int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);
		if (poll_count == -1) {
			std::cerr << "Poll failed: " << strerror(errno) << std::endl;
			break;
		}

		// Check all file descriptors
		for (size_t i = 0; i < poll_fds.size(); ++i) {
			if (poll_fds[i].revents & POLLIN) {
				if (poll_fds[i].fd == server_fd) {
					// New connection
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);
					
					int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
					if (client_fd == -1) {
						std::cerr << "Accept failed: " << strerror(errno) << std::endl;
						continue;
					}

					std::cout << "Client connected: " << ipToString(client_addr) << std::endl;

					// Add client to poll fds
					struct pollfd client_poll_fd;
					client_poll_fd.fd = client_fd;
					client_poll_fd.events = POLLIN;
					poll_fds.push_back(client_poll_fd);
				} else {
					// Data from client
					int client_fd = poll_fds[i].fd;
					ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
					
					if (bytes_read <= 0) {
						// Client disconnected or error
						if (bytes_read == 0) {
							std::cout << "Client disconnected" << std::endl;
						} else {
							std::cerr << "Recv failed: " << strerror(errno) << std::endl;
						}
						
						close(client_fd);
						poll_fds.erase(poll_fds.begin() + i);
						--i; // Adjust index after removal
					} else {
						// Echo back to client
						if (send(client_fd, buffer, bytes_read, 0) == -1) {
							std::cerr << "Send failed: " << strerror(errno) << std::endl;
						}
					}
				}
			}
		}
	}

	// Clean up
	for (size_t i = 0; i < poll_fds.size(); ++i) {
		close(poll_fds[i].fd);
	}

	return EXIT_SUCCESS;
}