#pragma once
#include "common.h"

class BaseRequest;

class RequestFactory {
public:
	static std::shared_ptr<BaseRequest> createRequestFromCommand(short cmd);
};