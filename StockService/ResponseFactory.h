#pragma once
#include "common.h"

class BaseResponse;
class BaseReuqest;

class ResponseFactory {
public:
	virtual ~ResponseFactory() = default;

	static std::shared_ptr<BaseResponse> createResponseFromReq(short cmd, bool status = false, std::string msg = "", std::string datas = "");
};
