#include <string>
#include <stdexcept>
#include <fstream>

class ServerException : public std::runtime_error {
public:
	ServerException(std::string info) : std::runtime_error(info) {}
	virtual ~ServerException() {}
};

class WSAStartupException : public ServerException {
public:
	WSAStartupException(int err_no) : ServerException("WSA library init failed with error" + err_no) {}
};

class SocketErrorException : public ServerException {
public:
	SocketErrorException(std::string info, int err_no) : ServerException(info += err_no) {}
};

class FileStreamFailed : public ServerException {
public:
	FileStreamFailed(std::string state) : ServerException("file stream failed with state: " + state) {};
};