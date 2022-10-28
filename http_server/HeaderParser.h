
#include <string>
#include <map>
#include <vector>

#pragma once
class HeaderParser
{
	std::string recvbuf;
	std::string method;
	std::string target;
	std::string protocolVersion;
	std::map<std::string, std::string> headers;
	
public:
	HeaderParser(std::string recvb);
	void parse();
	void parseFirstLine(std::string& firstLine);
	void parseHeader(std::vector<std::string>& line);
	void printHeaders();
	void introduceConnection();
	std::string getMethod();
	std::string getTarget();
	std::map<std::string, std::string> getHeaders();
	std::string getprotocolVersion();
	std::string getHost();
	std::string getConnection();
};

