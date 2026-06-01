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
        private IItemApiService _itemApiService;
        private IStockApiService _stockApiService;

        [ObservableProperty]
        private ObservableCollection<Item> _items;

        [ObservableProperty]
        private long _count;

        [ObservableProperty]
        private int _selectedItemId;

        public event Action RequestClose;


        public StockFormViewModel(IItemApiService itemApiService, IStockApiService stockApiService)
        {
            this._itemApiService = itemApiService;
            this._stockApiService = stockApiService;

            this.Items = new ObservableCollection<Item>();

            GetItems();
        }

        private async Task GetItems()
        {
            // call api.
            List<Item> res = await this._itemApiService.GetItems();

            this.Items = new ObservableCollection<Item>();
            foreach (Item item in res)
            {
                this.Items.Add(item);
            }
        }

        [RelayCommand]
        public void EditStock()
        {
            if (Count == 0 || Count < Stock.MIN_STOCK_SIZE || Stock.MAX_STOCK_SIZE < Count)
            {
                Console.WriteLine("입력 값이 유효하지 않습니다.");
                return;
            }

            if (Items.FirstOrDefault(t => t.Id == SelectedItemId) == null)
            {
                Console.WriteLine("선택된 item 값이 유효하지 않습니다.");
                return;
            }

            if (Count > 0)
            {
                AddStock((uint)Count);
            }
            else
            {
                ReduceStock((uint)-Count);
            }
        }

        public async Task AddStock(uint count)
        {
            // call api.
            bool res = await this._stockApiService.AddStock(SelectedItemId, count);

            SelectedItemId = -1;
            Count = 0;

            if (res == false)
            {
                // TODO
            }
            else
            {
                RequestClose?.Invoke();
                // TODO: 부모 window에 items 값을 전달한다. (재호출해서 업데이트 된 결과를 받아야 함.)
                // 즉, 추후에는 업데이트 된 결과가 올때까지 기다린다?
            }
        }

        public async Task ReduceStock(uint count)
        {
            // call api.
            bool res = await this._stockApiService.ReduceStock(SelectedItemId, count);

            SelectedItemId = -1;
            Count = 0;

            if (res == false)
            {
                // TODO
            }
            else
            {
                RequestClose?.Invoke();
                // TODO: 부모 window에 items 값을 전달한다. (재호출해서 업데이트 된 결과를 받아야 함.)
                // 즉, 추후에는 업데이트 된 결과가 올때까지 기다린다?
            }
        }
    }
}
