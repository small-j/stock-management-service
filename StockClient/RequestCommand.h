#pragma once
// 아래 되는지 확인해보기

struct Request {
	enum Command : short
	{
		UNKNOWN = -1,
		ADD_ITEM = 0,
		REMOVE_ITEM,
		PRINT_ITEM,
		ADD_STOCK,
		REDUCE_STOCK,
		PRINT_ITEM_TYPE,
		GET_MENU,
		COUNT
	};
};
