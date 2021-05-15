#pragma once
#include <algorithm>
#include "Ryugu/base/Copyable.h"
#include "Ryugu/net/Buffer.h"
#include "Ryugu/base/Debug.h"
#include "HttpRequest.h"


class HttpContext : public ryugu::base::Copyable
{
public:
	enum class ParseState
	{
		kExpectRequestLine,
		kExpectHeaders,
		kExpectBody,
		kGotAll,
	};
	HttpContext()
		:state_(ParseState::kExpectRequestLine)
	{

	}
	HttpRequest getRequest() const { return request_; }
	bool parseRequest(ryugu::net::Buffer* buffer,ryugu::base::Timestamp receiveTime);
	bool gotAll() const { return state_ == ParseState::kGotAll; }
	void reset();
private:
	bool parseRequestLine(const char* begin,const char* end);
	ParseState state_;
	HttpRequest request_;
};

void HttpContext::reset()
{
	state_ = ParseState::kExpectRequestLine;
	HttpRequest dummy;
	request_.~HttpRequest();
	new(&request_)HttpRequest();
}

// GET /XXX/XXX HTTP/1.1
bool HttpContext::parseRequestLine(const char* begin, const char* end)
{
	bool succeed = false;
	const char* start = begin;
	const char* space = std::find(start, end, ' ');
	if (space != end && request_.setMethod(std::string(start, space)))
	{
		start = space + 1;
		space = std::find(start, end, ' ');
		if (space != end)
		{
			const char* question = std::find(start, space, '?');
			if (question != space) //   /path1/do?a=1&b=2
			{
				request_.setPath(std::string(start, question));
				request_.setQuery(std::string(question, space));
			}
			else
			{
				request_.setPath(std::string(start, space));
			}
			start = space + 1;
			succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
			if (succeed)
			{
				switch (*(end - 1))
				{
				case '1':
					request_.setVersion(HttpRequest::Version::kHttp11);
					break;
				case '0':
					request_.setVersion(HttpRequest::Version::kHttp10);
					break;
				default:
					succeed = false;
				}
			}
		}
	}

	return succeed;
}
bool HttpContext::parseRequest(ryugu::net::Buffer* buffer, ryugu::base::Timestamp receiveTime)
{
	bool ok = true;
	bool hasMore = true;
	while (hasMore)
	{
		if (state_ == ParseState::kExpectRequestLine)
		{
			const char* crlf = buffer->find("\r\n");
			if (crlf)
			{
				// ½âÎöÇëÇóÐÐ
				ok = parseRequestLine(buffer->getPeek(), crlf);
				if (ok)
				{
					request_.setReceiveTime(receiveTime);
					buffer->consumeUntil(crlf + 2);
					state_ = ParseState::kExpectHeaders;
				}
				else
				{
					hasMore = false;
				}
			}
			else
			{
				hasMore = false;
			}
		}
		else if (state_ == ParseState::kExpectHeaders)
		{
			const char* crlf = buffer->find("\r\n");
			if (crlf)
			{
				const char* colon = std::find(buffer->getPeek(), crlf, ':');
				if (colon != crlf)
				{
					request_.addHeader(std::string(buffer->getPeek(), colon), std::string(colon + 1, crlf));
				}
				else
				{
					state_ = ParseState::kGotAll;
					hasMore = false;
				}
				buffer->consumeUntil(crlf + 2);
			}
			else
			{
				hasMore = false;
			}
		}
		else if (state_ == ParseState::kExpectBody)
		{
			// TODO
		}
	}
	return ok;
}
