#include "AnswerMaker.h"


void AnswerMaker::createHeader(std::map<std::string, std::string>& headers)
{
	std::string header = "";
	std::string protocol = PROTOCOL_VERSION;
	std::string statusCode = std::to_string(OK);

	header += protocol + " " + statusCode + " OK\r\n";
	if (headers.count("Connection"))
	{
		header += "Connection: " + headers["Connection"] + "\r\n";
		if (headers["Connection"] == "keep-alive")
		{
			header += "Keep-alive: timeout=5, max=1000\r\n";
		}
	}
	if (headers.count("Content-type") && !headers.count("Connection"))
	{
		header += "Content-type: " + headers["Content-type"] + "\r\n";
		header += "Connection: keep-alive\r\n";
		header += "Accept-Ranges: bytes\r\n";
	}
	if (headers.count("Content-type") && headers.count("Connection"))
	{
		header += "Content-type: " + headers["Content-type"] + "\r\n";
		header += "Accept-Ranges: bytes\r\n";
	}
	if (contentType == "")
	{
		header += "Content-type: text/html\r\n";
	}
	else if (contentType != "")
	{
		header += "Content-type: " + contentType + "\r\n";
	}

	this->header = header;
}

void AnswerMaker::createAnswer(const std::string target, std::map<std::string, std::string>& headers)
{
	if (headers.count("Content-type"))
	{
		if (headers["Content-type"] != "text/html")
		{
			contentType = headers["Content-type"];
			byteFileHandler(target);
		}

		if (headers["Content-type"] == "text/html")
		{
			contentType = "text/html";
			createHeader(headers);
			textHandler(target);
		}
	}
	if (!headers.count("Content-type"))
	{
		if (std::filesystem::absolute("./index.html").extension() == ".html" && target == "/")
		{
			contentType = "text/html";
			createHeader(headers);
			textHandler(target);
		}
		else
		{
			if (std::filesystem::absolute("." + target).extension() == ".ico")
			{
				contentType = "image/x-icon";
			}
			else if (std::filesystem::absolute("." + target).extension() == ".jpg")
			{
				contentType = "image/jpg";
			}
			
			createHeader(headers);
			byteFileHandler(target);
		}
	}
	
}

void AnswerMaker::textHandler(const std::string& target)
{
	if (target == "/")
	{
		this->targetPath = "./index.html";
	}
	else
	{
		this->targetPath = "." + target;
	}

	this->filesize = std::filesystem::file_size(std::filesystem::absolute(targetPath));

	std::ifstream ifs(this->targetPath);
	if (ifs.fail())
	{
		switch (ifs.rdstate())
		{
		case std::ios_base::badbit:
			throw FileStreamFailed("Read/writing error on i/o operation");
		case std::ios_base::failbit:
			throw FileStreamFailed("Logical error on i/o operation");
		default:
			break;
		}
	}
	std::string line;
	std::string textFile;
	this->answer = this->header;
	if (ifs.is_open())
	{
		while (std::getline(ifs, line))
		{
			textFile += pystring::strip(line);
		}
		ifs.close();
	}
	else std::cout << "Unable to open file";

	answer += "Content-Length: " + std::to_string(textFile.size()) + "\r\n\r\n";
	answer += textFile;
}

void AnswerMaker::byteFileHandler(const std::string& target)
{
	this->targetPath = "." + target;
	this->filesize = std::filesystem::file_size(std::filesystem::absolute(targetPath));
	this->header += "Content-Length: " + std::to_string(filesize) + "\r\n\r\n";

	this->byteFile = std::ifstream(this->targetPath, std::ifstream::binary);
}