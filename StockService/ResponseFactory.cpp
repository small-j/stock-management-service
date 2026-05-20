#include "pch.h"
#include "ResponseFactory.h"

#include <cassert>

#include "BaseResponse.h"
#include "BaseRequest.h"

#include "GetMenusResponse.h"
#include "GetItemTypesResponse.h"
#include "AddItemResponse.h"
#include "RemoveItemResponse.h"
#include "PrintItemResponse.h"
#include "AddStockResponse.h"
#include "ReduceStockResponse.h"

std::shared_ptr<BaseResponse> ResponseFactory::createResponseFromReq(short cmd, bool status , std::string msg, std::string datas) {
	if (msg.length() == 0) {
		switch (cmd) {
		case Request::Command::ADD_ITEM:
			return std::make_shared<AddItemResponse>();
		case Request::Command::REMOVE_ITEM:
			return std::make_shared<RemoveItemResponse>();
		case Request::Command::GET_ITEM_TYPE:
			return std::make_shared<GetItemTypesResponse>();
		case Request::Command::PRINT_ITEM:
			return std::make_shared<PrintItemResponse>();
		case Request::Command::ADD_STOCK:
			return std::make_shared<AddStockResponse>();
		case Request::Command::REDUCE_STOCK:
			return std::make_shared<ReduceStockResponse>();
		case Request::Command::GET_MENU:
			return std::make_shared<GetMenusResponse>();
		default:
			assert(false);
			return nullptr;
		}
	}

	switch (cmd) {
	case Request::Command::ADD_ITEM:
		return std::make_shared<AddItemResponse>(status, msg);
	case Request::Command::REMOVE_ITEM:
		return std::make_shared<RemoveItemResponse>(status, msg);
	case Request::Command::GET_ITEM_TYPE:
		return std::make_shared<GetItemTypesResponse>(status, msg, datas);
	case Request::Command::PRINT_ITEM:
		return std::make_shared<PrintItemResponse>(status, msg, datas);
	case Request::Command::ADD_STOCK:
		return std::make_shared<AddStockResponse>(status, msg);
	case Request::Command::REDUCE_STOCK:
		return std::make_shared<ReduceStockResponse>(status, msg);
	case Request::Command::GET_MENU:
		return std::make_shared<GetMenusResponse>(status, msg, datas);
	default:
		assert(false);
		return nullptr;
	}
}

