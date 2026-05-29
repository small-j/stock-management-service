using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using StockClient_WPF.Interfaces;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace StockClient_WPF.ViewModels.Windows
{
    public partial class ItemFormViewModel : ObservableObject
    {
        private IServerConnection<Packet, Packet> _serverConnection;


        [ObservableProperty]
        private ObservableCollection<ItemTypeDto> _itemTypes;

        [ObservableProperty]
        private int _selectedItemType;

        [ObservableProperty]
        private String _name;

        public event Action RequestClose;

        public ItemFormViewModel(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;

            this.ItemTypes = new ObservableCollection<ItemTypeDto>();

            UpdateItemTypes();
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
                    this.ItemTypes = new ObservableCollection<ItemTypeDto>(res.ItemTypes);
                }
                else
                {
                    // null일 경우 화면에 에러 문구 띄워주기 -> 다시 서버에 요청해주세요.
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
        }

        [RelayCommand]
        public void AddItem()
        {
            bool isValidItemType = ItemTypes.Any(t => t.Id == SelectedItemType);

            if (isValidItemType == false || Name.Length == 0)
            {
                Console.WriteLine("입력 값이 유효하지 않습니다.");
                return;
            }

            Packet packet = new Packet
            {
                Type = PacketType.AddItem,
                AddItemReq = new AddItemRequest()
                {
                    ItemDto = new ItemDto()
                    {
                        ItemType = SelectedItemType,
                        Name = Name,
                    }
                }
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.AddItem)
            {
                AddItemResponse res = resP.AddItemRes;
                if (res.Status)
                {
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
