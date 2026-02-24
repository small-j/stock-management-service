#pragma once

struct Request {
	enum Command : short
	{
		UNKNOWN = -1,
		ADD_ITEM = 0,
		REMOVE_ITEM,
		PRINT_ITEM,
		ADD_STOCK,
		REDUCE_STOCK,
		GET_ITEM_TYPE,
		GET_MENU,
		COUNT
	};
};
