// StockServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include "App.h"

int main()
{

    // App의 status를 체크해서 app이 quit 될 때까지 실행.
    App app;
    app.run(); // TODO: thread를 나눠서 실행해야함.

    while (StockServer::ThreadStatus::QUIT != app.getStatus()) {}
    return 0;
}
