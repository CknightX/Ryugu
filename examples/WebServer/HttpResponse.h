#pragma once

#include <unordered_map>
#include <string>
#include "Ryugu/base/Copyable.h"
#include "Ryugu/net/Buffer.h"


class HttpResponse : public ryugu::base::Copyable
{
public:
	enum class StatusCode
	{
		kUnknown,
		k200Ok=200,
		k301MovedPermanently=301,
		k400BadRequest=400,
		k404NotFound=404
	};
	explicit HttpResponse(bool close)
		:statusCode_(StatusCode::kUnknown),
		closeConnection_(close)
	{
	}
	void setStatusCode(StatusCode code) { statusCode_ = code; }
	void setStatusMessage(const std::string& message) { statusMessage_ = message; }
	void setCloseConnection(bool on) { closeConnection_ = on; }
	bool isClose() const { return closeConnection_; }
	void setContentType(const std::string& contentType)
	{
	}
	void addHeader(const std::string& field, const std::string& value)
	{
		headers_[field] = value;
	}
	void setBody(const std::string& body) { body_ = body; }
	void appendToBuffer(ryugu::net::Buffer* outputBuffer) const;

private:
	std::unordered_map<std::string, std::string> headers_;
	StatusCode statusCode_;
	std::string statusMessage_;
	bool closeConnection_;
	std::string body_;
};
void HttpResponse::appendToBuffer(ryugu::net::Buffer* output) const
{
	char buf[32];
	snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
	output->append(buf);
	output->append(statusMessage_);
	output->append("\r\n");

	if (closeConnection_)
	{
		output->append("Connection: close\r\n");
	}
	else
	{
		snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
		output->append(buf);
		output->append("Connection: Keep-Alive\r\n");
	}

	for (const auto& header : headers_)
	{
		output->append(header.first);
		output->append(": ");
		output->append(header.second);
		output->append("\r\n");
	}
	output->append("\r\n");
	output->append(body_);
}
