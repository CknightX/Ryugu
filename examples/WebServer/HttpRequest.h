#pragma once

#include "Ryugu/Copyable.h"
#include "Ryugu/Timestamp.h"

#include <map>
#include <cassert>
#include <string>


class HttpRequest : public ryugu::base::Copyable
{
public:
	enum class Method
	{
		kInvalid,kGet,kPost,kHead,kPut,kDelete
	};
	enum class Version
	{
		kUnknown,kHttp10,kHttp11
	};
	HttpRequest()
		:method_(Method::kInvalid),
		version_(Version::kUnknown)
	{
	}
	void setVersion(Version v) { version_ = v; }
	Version getVersion() const { return version_; }
	bool setMethod(const std::string& method)
	{
		assert(method_ == Method::kInvalid);
		if (method == "GET")
			method_ = Method::kGet;
		else if (method == "POST")
			method_ = Method::kPost;
		else if (method == "HEAD")
			method_ = Method::kHead;
		else if (method == "PUT")
			method_ = Method::kPut;
		else if (method == "DELETE")
			method_ = Method::kDelete;
		else
			method_ = Method::kInvalid;
		return method_ != Method::kInvalid;
	}
	Method getMethod() const { return method_; }
	void setPath(const std::string& path) { path_ = path; }
	const std::string& getPath() const { return path_; }
	void setQuery(const std::string& query) { query_ = query; }
	const std::string& getQuery() const { return query_; }
	void setReceiveTime(ryugu::Timestamp rt) { receiveTime_ = rt; }
	void addHeader(const std::string& field, const std::string& value)
	{
		headers_[field] = value;
	}
	const std::string getHeader(const std::string& field) const
	{
		std::string res{};
		auto iter = headers_.find(field);
		if (iter != headers_.end())
		{
			res = iter->second;
		}
		return res;
	}
	const std::unordered_map<std::string, std::string>& getHeaders() const { return headers_; }
	void swap(HttpRequest& other)
	{

	}
private:
	Method method_;
	Version version_;
	std::string path_;
	std::string query_;
	ryugu::Timestamp receiveTime_;
	std::unordered_map<std::string, std::string> headers_;
};