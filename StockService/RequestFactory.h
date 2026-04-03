#pragma once
#include <memory>

class BaseRequest;

class RequestFactory {
public:
	static std::shared_ptr<BaseRequest> createRequestFromCommand(short cmd);
};