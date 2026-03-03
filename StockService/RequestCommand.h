#pragma once

struct Request {
	enum Command : short
	{
		UNKNOWN = -1,
		ADD_ITEM = 1001,
		REMOVE_ITEM,
		PRINT_ITEM,
		GET_ITEM_TYPE,
		ADD_STOCK = 2001,
		REDUCE_STOCK,
		GET_MENU = 3001,
		//COUNT
	};
};
