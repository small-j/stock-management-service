using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using StockClient_WPF.Interfaces;
using StockClient_WPF.Models;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Xml.Linq;

namespace StockClient_WPF.ViewModels.Windows
{
    public partial class StockFormViewModel : ObservableObject
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        [ObservableProperty]
        private ObservableCollection<Item> _items;

        [ObservableProperty]
        private uint _count;

        [ObservableProperty]
        private int _selectedItemId;

        public event Action RequestClose;


        public StockFormViewModel(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;

            this.Items = new ObservableCollection<Item>();

            GetItems();
        }

        private void GetItems()
        {
            Packet packet = new Packet
            {
                Type = PacketType.GetItems,
                GetItemsReq = new GetItemsRequest()
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.GetItems)
            {
                GetItemsResponse res = resP.GetItemsRes;
                if (res.Status)
                {
                    this.Items = new ObservableCollection<Item>(
                        res.ItemDto.Select(t => new Item()
                        {
                            Id = t.Id,
                            Name = t.Name,
                            ItemType = "",
                        })
                    );
                }
                else
                {
                    // null일 경우 화면에 에러 문구 띄워주기 -> 다시 서버에 요청해주세요.
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
        }

        [RelayCommand]
        public void AddStock()
        {
            Packet packet = new Packet
            {
                Type = PacketType.AddStock,
                AddStockReq = new AddStockRequest()
                {
                    StockDto = new StockDto()
                    {
                        ItemId = SelectedItemId,
                        Count = Count,
                    }
                }
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.AddStock)
            {
                AddStockResponse res = resP.AddStockRes;
                if (res.Status)
                {
                    SelectedItemId = -1;
                    Count = 0;
                    RequestClose?.Invoke();
                    // TODO: 부모 window에 items 값을 전달한다. (재호출해서 업데이트 된 결과를 받아야 함.)
                    // 즉, 추후에는 업데이트 된 결과가 올때까지 기다린다?
                }
                else
                {
                    // null일 경우 화면에 에러 문구 띄워주기 -> 다시 서버에 요청해주세요.
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
        }
    }
}
