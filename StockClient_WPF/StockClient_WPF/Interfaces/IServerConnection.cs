using System;
using System.Collections.Generic;
using System.Text;

namespace StockClient_WPF.Interfaces
{
    public interface IServerConnection<C, T>
    {
        void Init();
        void CreateConnection();
        void DestroyConnection();

        void Send(C obj);
        T Receive(int bufferSize = 1024);

    }
}
