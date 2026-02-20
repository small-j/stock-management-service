#pragma once

// enum으로 하면 선생님이 말씀해주셨던 
// 확장성? 을 가질수가 없는데
// 어떻게 해야 edit, print 이런 동작이 
// 계속 늘어나는 숫자가 되지 않도록 할까
enum RequestCommand 
{
	UNKOWN = -1,
	ADD_ITEM = 0,
	REMOVE_ITEM = 1,
	PRINT_ITEM = 2,
	ADD_STOCK = 3,
	REDUCE_STOCK = 4,
	COUNT = 5
};