using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using StockClient_WPF.Interfaces;
using StockClient_WPF.Views.Pages;
using StockClient_WPF.Views.Windows;
using StockServiceProtocol;
using System.Collections.ObjectModel;

namespace StockClient_WPF.ViewModels.Pages
{
    public partial class MainViewModel : ObservableObject
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        [ObservableProperty]
        private ObservableCollection<ResItemDto> _items;

        private Dictionary<int, string> _itemTypes;

        //[ObservableProperty]
        //private ResStockDto? _stocks;


        public MainViewModel(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;
            this._items = new ObservableCollection<ResItemDto>();

            UpdateItemTypes();
            UpdateItems();
        }

        [RelayCommand]
        public void AddItem()
        {
            var itemFormWindow = App.Services.GetRequiredService<ItemForm>();
            itemFormWindow.Show();
        }

        [RelayCommand]
        public void AddStock()
        {
            var stockFormWindow = App.Services.GetRequiredService<StockForm>();
            stockFormWindow.Show();
        }

        [RelayCommand]
        private void UpdateItems()
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
                if (res.Status && this._itemTypes != null)
                {
                    this.Items = new ObservableCollection<ResItemDto>(
                        res.ResItemDto.Select(t => new ResItemDto() {
                           Id = t.Id,
                           Name = t.Name,
                           ItemType = this._itemTypes[t.ItemType],
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
        private void UpdateItemTypes()
        {
            Packet packet = new Packet
            {
                Type = PacketType.GetItemType,
                GetItemTypesReq = new GetItemTypesRequest()
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.GetItemType)
            {
                GetItemTypesResponse res = resP.GetItemTypesRes;
                if (res.Status)
                {
                    this._itemTypes = res.ItemTypes.ToDictionary(t => t.Id, t => t.Name);
                }
                else
                {
                    // null일 경우 화면에 에러 문구 띄워주기 -> 다시 서버에 요청해주세요.
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
        }

        private void UpdateStocks()
        {

        }
    }
}