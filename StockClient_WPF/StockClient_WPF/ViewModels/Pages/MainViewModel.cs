using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using StockClient_WPF.Interfaces;
using StockClient_WPF.Views.Pages;
using StockClient_WPF.Views.Windows;
using StockServiceProtocol;
using System.Collections.ObjectModel;
using StockClient_WPF.Models;

namespace StockClient_WPF.ViewModels.Pages
{
    public partial class MainViewModel : ObservableObject
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        [ObservableProperty]
        private ObservableCollection<Item> _items;

        private Dictionary<int, string> _itemTypes;

        [ObservableProperty]
        private ObservableCollection<Stock> _stocks;


        public MainViewModel(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;
            this._items = new ObservableCollection<Item>();

            GetItemTypes();
            GetItems();

            GetStocks();
        }

        [RelayCommand]
        public void AddItem()
        {
            var itemFormWindow = App.Services.GetRequiredService<ItemForm>();
            itemFormWindow.Show();
        }

        [RelayCommand]
        public void EditStock()
        {
            var stockFormWindow = App.Services.GetRequiredService<StockForm>();
            stockFormWindow.Show();
        }

        [RelayCommand]
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
                if (res.Status && this._itemTypes != null)
                {
                    this.Items = new ObservableCollection<Item>(
                        res.ItemDto.Select(t => new Item() {
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
        private void DeleteItem(int id)
        {
            Packet packet = new Packet
            {
                Type = PacketType.RemoveItem,
                RemoveItemReq = new RemoveItemRequest()
                {
                    ItemId = id
                }
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.RemoveItem)
            {
                RemoveItemResponse res = resP.RemoveItemRes;
                if (res.Status)
                {
                    var item = this.Items.FirstOrDefault(item => item.Id == id);
                    if (item != null)
                    {
                        this.Items.Remove(item);
                    }
                }
                else
                {
                    // null일 경우 화면에 에러 문구 띄워주기 -> 다시 서버에 요청해주세요.
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
        }

        [RelayCommand]
        private void GetItemTypes()
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

        [RelayCommand]
        private void GetStocks()
        {
            Packet packet = new Packet
            {
                Type = PacketType.GetStocks,
                GetStocksReq = new GetStocksRequest()
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.GetStocks)
            {
                GetStocksResponse res = resP.GetStocksRes;
                if (res.Status)
                {
                    this.Stocks = new ObservableCollection<Stock>(
                        res.StockDto.Select(t => new Stock()
                        {
                            Id = t.Id,
                            ItemName = Items.First(item => item.Id == t.Id).Name,
                            Count = t.Count,
                            ItemId = t.ItemId,
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
    }
}