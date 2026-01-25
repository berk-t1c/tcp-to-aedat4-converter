#pragma once

#include "config.hpp"
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
    #define INVALID_SOCK INVALID_SOCKET
    #define SOCKET_ERROR_CODE WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netinet/tcp.h>
    typedef int socket_t;
    #define INVALID_SOCK (-1)
    #define SOCKET_ERROR_CODE errno
#endif

namespace converter {

/**
 * TCP Receiver class
 * 
 * Connects to the camera TCP server and receives binary frames.
 * Handles partial reads and optional frame headers.
 */
class TcpReceiver {
public:
    /**
     * Constructor
     * @param cfg Configuration reference
     */
    explicit TcpReceiver(const Config& cfg);
    
    /**
     * Destructor - closes socket
     */
    ~TcpReceiver();
    
    // Disable copy
    TcpReceiver(const TcpReceiver&) = delete;
    TcpReceiver& operator=(const TcpReceiver&) = delete;
    
    // Enable move
    TcpReceiver(TcpReceiver&& other) noexcept;
    TcpReceiver& operator=(TcpReceiver&& other) noexcept;
    
    /**
     * Connect to the camera TCP server
     * @return true if connection successful
     */
    bool connect();
    
    /**
     * Disconnect from the camera
     */
    void disconnect();
    
    /**
     * Check if connected
     * @return true if connected
     */
    bool isConnected() const;
    
    /**
     * Receive one complete frame
     * @param buffer Output buffer (will be resized to frame size)
     * @return true if frame received successfully, false on error/disconnect
     */
    bool receiveFrame(std::vector<uint8_t>& buffer);
    
    /**
     * Get the expected frame size (without header)
     * @return Frame size in bytes
     */
    int getFrameSize() const;
    
    /**
     * Get total bytes received
     * @return Total bytes received since connection
     */
    uint64_t getTotalBytesReceived() const { return total_bytes_received_; }
    
    /**
     * Get total frames received
     * @return Total frames received since connection
     */
    uint64_t getTotalFramesReceived() const { return total_frames_received_; }

private:
    /**
     * Receive exact number of bytes (handles partial reads)
     * @param buffer Output buffer
     * @param size Number of bytes to receive
     * @return true if all bytes received, false on error
     */
    bool receiveExact(uint8_t* buffer, size_t size);
    
    /**
     * Initialize socket library (Windows only)
     */
    static bool initSocketLib();
    
    /**
     * Cleanup socket library (Windows only)
     */
    static void cleanupSocketLib();
    
    const Config& config_;
    socket_t socket_;
    bool connected_;
    
    uint64_t total_bytes_received_;
    uint64_t total_frames_received_;
    
    static bool socket_lib_initialized_;
};

} // namespace converter
