using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using StockClient_WPF.Interfaces;
using StockClient_WPF.Views.Pages;
using StockServiceProtocol;

namespace StockClient_WPF.ViewModels.Pages
{
    public class MainViewModel : ObservableObject
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        [ObservableProperty]
        private ResItemDto _items;

        //[ObservableProperty]
        //private ResStockDto? _stocks;


        public MainViewModel(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;

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

        private void UpdateItems()
        {
            Packet packet = new Packet
            {
                Type = PacketType.PrintItem,
                GetItemsReq = new GetItemsRequest()
            };

            this._serverConnection.Send(packet);
            Packet res = this._serverConnection.Receive(1024);


            this._items = res.GetItemsRes.ResItemDto;
        }

        private void UpdateStocks()
        {

        }
    }
