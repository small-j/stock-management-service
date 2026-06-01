using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Microsoft.Extensions.DependencyInjection;
using StockClient_WPF.Interfaces;
using StockClient_WPF.Models;
using StockClient_WPF.Views.Pages;
using StockClient_WPF.Views.Windows;
using StockServiceProtocol;
using System.Collections.ObjectModel;
using System.Xml.Linq;

namespace StockClient_WPF.ViewModels.Pages
{
    public partial class MainViewModel : ObservableObject
    {
        private IItemApiService _itemApiService;
        private IStockApiService _stockApiService;

        [ObservableProperty]
        private ObservableCollection<Item> _items;

        private Dictionary<int, string>? _itemTypes;

        [ObservableProperty]
        private ObservableCollection<Stock> _stocks;


        public MainViewModel(IItemApiService itemApiService, IStockApiService stockApiService)
        {
            _itemApiService = itemApiService;
            _stockApiService = stockApiService;

            this.Items = new ObservableCollection<Item>();
            this.Stocks = new ObservableCollection<Stock>();

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
        private async Task GetItems()
        {
            if (this._itemTypes == null)
            {
                // TODO: 에러문 출력.
                return;
            }

            // call api.
            List<Item> res = await this._itemApiService.GetItems();

            this.Items = new ObservableCollection<Item>();

            foreach (var item in res)
            {
                item.ItemTypeName = this._itemTypes[item.ItemType];
                this.Items.Add(item);
            }
        }

        [RelayCommand]
        private async Task DeleteItem(int id)
        {
            // call api.
            bool res = await this._itemApiService.RemoveItem(id);
           
            if (res == true)
            {
                var item = this.Items.FirstOrDefault(item => item.Id == id);
                if (item != null)
                {
                    this.Items.Remove(item);
                }
            }
            else
            {
                // TODO
            }
        }

        [RelayCommand]
        private async Task GetItemTypes()
        {
            // call api.
            List<ItemType> res = await this._itemApiService.GetItemTypes();

            this._itemTypes = res.ToDictionary(t => t.Id, t => t.Name);
        }

        [RelayCommand]
        private async Task GetStocks()
        {
            // call api.
            List<Stock> res = await this._stockApiService.GetStocks();

            this.Stocks = new ObservableCollection<Stock>();
            foreach (var stock in res)
            {
                Item? stockItem = Items.FirstOrDefault(item => item.Id == stock.ItemId);

                if (stockItem == null) continue;

                stock.ItemName = stockItem.Name;
                Stocks.Add(stock);
            }
        }
    }
}