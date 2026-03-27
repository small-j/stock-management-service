#pragma once

struct StockServer {
	enum StatusCode {
		OK = 0,
		CANCELLED = 1,
	};

	enum ThreadStatus {
		INIT = 0,
		WORKING,
		STOPED,
		QUIT
	};
};