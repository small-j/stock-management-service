using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using StockClient_WPF.Interfaces;
using StockClient_WPF.Models;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace StockClient_WPF.ViewModels.Windows
{
    public partial class ItemFormViewModel : ObservableObject
    {
        private IItemApiService _itemApiService;
        private IStockApiService _stockApiService;

        [ObservableProperty]
        private ObservableCollection<ItemType> _itemTypes;

        [ObservableProperty]
        private int _selectedItemType;

        [ObservableProperty]
        private String _name;

        public event Action RequestClose;

        public ItemFormViewModel(IItemApiService itemApiService, IStockApiService stockApiService)
        {
            this._itemApiService = itemApiService;
            this._stockApiService = stockApiService;

            this.ItemTypes = new ObservableCollection<ItemType>();

            GetItemTypes();
        }

        private async Task GetItemTypes()
        {
            // call api.
            List<ItemType> res = await this._itemApiService.GetItemTypes();

            this.ItemTypes = new ObservableCollection<ItemType>();
            foreach (ItemType itemType in res)
            {
                this.ItemTypes.Add(itemType);
            }
        }

        [RelayCommand]
        public async Task AddItem()
        {
            bool isValidItemType = ItemTypes.Any(t => t.Id == SelectedItemType);

            if (isValidItemType == false || Name.Length == 0)
            {
                Console.WriteLine("입력 값이 유효하지 않습니다.");
                return;
            }

            // call api.
            bool res = await this._itemApiService.AddItem(SelectedItemType, Name);

            SelectedItemType = -1;
            Name = "";

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
