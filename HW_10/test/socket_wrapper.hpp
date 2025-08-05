#ifndef SOCKET_WRAPPER_HPP
#define SOCKET_WRAPPER_HPP

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

namespace test_util
{
    class ClientSocket
    {
    public:
        ClientSocket(
            const std::string&  host,
            const std::uint16_t port)
        {
#ifdef _WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                throw std::runtime_error("WSAStartup failed");
            }
#endif
            const std::string port_str = std::to_string(port);
            struct addrinfo hints = {};
            struct addrinfo *res = nullptr;

            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res) != 0)
            {
#ifdef _WIN32
                WSACleanup();
#endif
                throw std::runtime_error("getaddrinfo failed");
            }

            sock_ = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (sock_ < 0)
            {
                freeaddrinfo(res);
#ifdef _WIN32
                WSACleanup();
#endif
                throw std::runtime_error("socket creation failed");
            }

#ifdef _WIN32
            const int addrlen = static_cast<int>(res->ai_addrlen);
#else
            const socklen_t addrlen = res->ai_addrlen;
#endif
            if (connect(sock_, res->ai_addr, addrlen) < 0)
            {
                freeaddrinfo(res);
                close_socket();

                throw std::runtime_error("connect failed");
            }

            freeaddrinfo(res);
        }

        ~ClientSocket()
        {
            close_socket();
        }

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator=(const ClientSocket&) = delete;
        ClientSocket(ClientSocket&&) = delete;
        ClientSocket& operator=(ClientSocket&&) = delete;

        void send_data(const std::string& data) const
        {
#ifdef _WIN32
            const int len = static_cast<int>(data.length());
#else
            const size_t len = data.length();
#endif
            if (send(sock_, data.c_str(), len, 0) < 0)
            {
                throw std::runtime_error("send failed");
            }

#ifdef _WIN32
            if (shutdown(sock_, SD_SEND) != 0)
#else
            if (shutdown(sock_, SHUT_WR) != 0)
#endif
            {
                throw std::runtime_error("shutdown failed");
            }
        }

    private:
        void close_socket()
        {
#ifdef _WIN32
            if (sock_ != INVALID_SOCKET)
            {
                closesocket(sock_);
                WSACleanup();
                sock_ = INVALID_SOCKET;
            }
#else
            if (sock_ != -1)
            {
                if (close(sock_) != 0)
                {
                    std::cerr << "close failed\n";
                }

                sock_ = -1;
            }
#endif
        }

#ifdef _WIN32
        SOCKET sock_ = INVALID_SOCKET;
#else
        int sock_ = -1;
#endif
    };

} // namespace test_util

#endif // SOCKET_WRAPPER_HPP
