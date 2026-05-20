#pragma once

#include <memory>

class BaseRequest;
class AddItemDto;
class RemoveItemDto;
class AddStockDto;
class ReduceStockDto;

class DtoFactory {
public:
	static std::shared_ptr<AddItemDto> getAddItemDtoFromReq(std::shared_ptr<BaseRequest> req);
	static std::shared_ptr<RemoveItemDto> getRemoveItemDtoFromReq(std::shared_ptr<BaseRequest> req);
	static std::shared_ptr<AddStockDto> getAddStockDtoFromReq(std::shared_ptr<BaseRequest> req);
	static std::shared_ptr<ReduceStockDto> getReduceStockDtoFromReq(std::shared_ptr<BaseRequest> req);
};