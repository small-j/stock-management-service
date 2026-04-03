#include "pch.h"
#include "RequestFactory.h"

#include <cassert>

#include "BaseRequest.h"
#include "AddItemRequest.h"
#include "RemoveItemRequest.h"
#include "PrintItemRequest.h"
#include "GetItemTypesRequest.h"
#include "AddStockRequest.h"
#include "ReduceStockRequest.h"
#include "GetMenusRequest.h"

std::shared_ptr<BaseRequest> RequestFactory::createRequestFromCommand(short cmd) {
	switch (cmd) {
	case Request::Command::ADD_ITEM:
		return std::make_shared<AddItemRequest>();
	case Request::Command::REMOVE_ITEM:
		return std::make_shared<RemoveItemRequest>();
	case Request::Command::GET_ITEM_TYPE:
		return std::make_shared<GetItemTypesRequest>();
	case Request::Command::PRINT_ITEM:
		return std::make_shared<PrintItemRequest>();
	case Request::Command::ADD_STOCK:
		return std::make_shared<AddStockRequest>();
	case Request::Command::REDUCE_STOCK:
		return std::make_shared<ReduceStockRequest>();
	case Request::Command::GET_MENU:
		return std::make_shared<GetMenusRequest>();
	default:
		assert(false);
		return nullptr;
	}
}