#pragma once
#include <memory>

class BaseRequest;
class BaseResponse;

struct RequestKeyPair {
	int socketKey;
	std::shared_ptr<BaseRequest> req;
};

struct ResponseKeyPair {
	int socketKey;
	std::shared_ptr<BaseResponse> res;
};