using CommunityToolkit.Mvvm.ComponentModel;
using Google.Protobuf;
using StockClient_WPF.Interfaces;
using StockClient_WPF.ViewModels.Pages;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Text;

namespace StockClient_WPF.ViewModels.Windows
{
    public class MainWindowViewModel : ObservableObject
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        public MainWindowViewModel(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;

            initialize();
        }

        public void initialize()
        {
            GetMenusRequest req = new GetMenusRequest();
            var packet = new Packet
            {
                Type = PacketType.GetMenu,
                GetMenusReq = req
            };

            this._serverConnection.Send(packet);
            Packet result = this._serverConnection.Receive();
            // TODO: client에서 갖고 있는 기능 목록과 비교.
        }
    }
}
