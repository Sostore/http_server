#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>

#include "ServerExceptions.h"
#include "pystring.h"
#include "HTTPStatusCodes.h"

#define SERVER_NAME "http_server"
#define PROTOCOL_VERSION "HTTP/1.1"

#pragma once
class AnswerMaker
{
	size_t contentSize;
	
	void textHandler(const std::string& target);
	void byteFileHandler(const std::string &target);
public:
	std::string header;
	std::string answer;
	std::string contentType;
	std::string targetPath;
	std::ifstream byteFile;
	std::uintmax_t filesize;
	void createHeader(std::map<std::string, std::string>& headers);
	void createAnswer(const std::string target, std::map<std::string, std::string>& headers);
};

