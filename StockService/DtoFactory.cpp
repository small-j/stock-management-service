#include "pch.h"
#include "DtoFactory.h"

#include "LoggerService.h"

#include "AddItemRequest.h"
#include "RemoveItemRequest.h"
#include "AddStockRequest.h"
#include "ReduceStockRequest.h"

#include "AddItemDto.h"
#include "RemoveItemDto.h"
#include "AddStockDto.h"
#include "ReduceStockDto.h"

std::shared_ptr<AddItemDto> DtoFactory::getAddItemDtoFromReq(std::shared_ptr<BaseRequest> req)
{
    if (req == nullptr) {
        LoggerService::error("AddItemDto 객체 생성에 실패했습니다. request 값이 비어있습니다.");

        return nullptr;
    }

    auto parsedReq = std::dynamic_pointer_cast<AddItemRequest>(req);

    if (req == nullptr) {
        LoggerService::error("AddItemDto 객체 생성에 실패했습니다.");

        return nullptr;
    }

    return std::make_shared<AddItemDto>(parsedReq->getName(), parsedReq->getItemType());
}

std::shared_ptr<RemoveItemDto> DtoFactory::getRemoveItemDtoFromReq(std::shared_ptr<BaseRequest> req)
{
    if (req == nullptr) {
        LoggerService::error("RemoveItemDto 객체 생성에 실패했습니다. request 값이 비어있습니다.");

        return nullptr;
    }

    auto parsedReq = std::dynamic_pointer_cast<RemoveItemRequest>(req);

    if (req == nullptr) {
        LoggerService::error("RemoveItemDto 객체 생성에 실패했습니다.");

        return nullptr;
    }

    return std::make_shared<RemoveItemDto>(parsedReq->getItemId());
}

std::shared_ptr<AddStockDto> DtoFactory::getAddStockDtoFromReq(std::shared_ptr<BaseRequest> req)
{
    if (req == nullptr) {
        LoggerService::error("AddStockDto 객체 생성에 실패했습니다. request 값이 비어있습니다.");

        return nullptr;
    }

    auto parsedReq = std::dynamic_pointer_cast<AddStockRequest>(req);

    if (req == nullptr) {
        LoggerService::error("AddStockDto 객체 생성에 실패했습니다.");

        return nullptr;
    }

    return std::make_shared<AddStockDto>(parsedReq->getItemId(), parsedReq->getCount());
}

std::shared_ptr<ReduceStockDto> DtoFactory::getReduceStockDtoFromReq(std::shared_ptr<BaseRequest> req)
{
    if (req == nullptr) {
        LoggerService::error("ReduceStockDto 객체 생성에 실패했습니다. request 값이 비어있습니다.");

        return nullptr;
    }

    auto parsedReq = std::dynamic_pointer_cast<ReduceStockRequest>(req);

    if (req == nullptr) {
        LoggerService::error("ReduceStockDto 객체 생성에 실패했습니다.");

        return nullptr;
    }

    return std::make_shared<ReduceStockDto>(parsedReq->getItemId(), parsedReq->getCount());
}

