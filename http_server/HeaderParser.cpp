#include <iostream>
#include <algorithm>
#include "HeaderParser.h"
#include "pystring.h"

HeaderParser::HeaderParser(std::string recvb)
{
	this->recvbuf = recvb;
	parse();
	
}

void HeaderParser::parse()
{
	std::vector< std::string > result;

	// Strip \r\n in recvbuf
	recvbuf = pystring::strip(recvbuf);

	// Split headers by the \r\n
	std::string escape_chars = "\r\n";
	result = pystring::split(recvbuf, escape_chars, 1);

	// Parse GET / HTTP/1.1
	parseFirstLine(result[0]);
	parseHeader(result);
	introduceConnection();
}

void HeaderParser::parseFirstLine(std::string& firstLine)
{
	std::vector< std::string > result;

	// Split first line by the empty space
	result = pystring::split(firstLine, " ", 2);

	this->method = result[0];
	this->target = result[1];
	this->protocolVersion = result[2];
}

void HeaderParser::parseHeader(std::vector< std::string >& header)
{
	// Erase GET / HTTP/1.1
	header.erase(header.begin() + 0);

	// Parse headers and separate key and value by the ":"
	std::vector< std::string > result;
	for (const std::string& line : header)
	{
		result = pystring::split(line, ":", 1);
		headers[result[0]] = result[1];
	}
}

std::string HeaderParser::getMethod()
{
	return method;
}

std::string HeaderParser::getTarget()
{
	return target;
}

std::map<std::string, std::string> HeaderParser::getHeaders()
{
	return headers;
}

std::string HeaderParser::getprotocolVersion()
{
	return protocolVersion;
}

void HeaderParser::printHeaders()
{
	std::cout << "Method: " << method << std::endl;
	std::cout << "Target: " << target << std::endl;
	std::cout << "Protocol version: " << protocolVersion << std::endl;
	for (auto const& pair : headers)
	{
		std::cout << pair.first << ": " << pair.second << std::endl;
	}
}

void HeaderParser::introduceConnection()
{
	std::cout << method << " " << target << " " << protocolVersion << std::endl;
}

std::string HeaderParser::getHost()
{
	return "Error";
}

std::string HeaderParser::getConnection()
{
	return "Error";
}