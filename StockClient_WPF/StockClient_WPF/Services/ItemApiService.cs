using StockClient_WPF.Interfaces;
using StockClient_WPF.Models;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Xml.Linq;

namespace StockClient_WPF.Services
{
    public class ItemApiService : IItemApiService
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        public ItemApiService(IServerConnection<Packet, Packet> serverConnection) 
        {
            this._serverConnection = serverConnection;
        }

        public async Task<bool> AddItem(int itemType, string name)
        {
            Packet packet = new Packet
            {
                Type = PacketType.AddItem,
                AddItemReq = new AddItemRequest()
                {
                    ItemDto = new ItemDto()
                    {
                        ItemType = itemType,
                        Name = name,
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
                    return true;
                }
                else
                {
                    Console.WriteLine($"실패: {res.Message}");
                    return false;
                }
            }
            else
            {
                Console.WriteLine($"실패: return 형식이 잘못되었습니다.");
            }

            return false;
        }

        public async Task<List<Item>> GetItems()
        {
            List<Item> result = new List<Item>();

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
                    result = new List<Item>(
                        res.ItemDto.Select(t => new Item()
                        {
                            Id = t.Id,
                            Name = t.Name,
                            ItemType = t.ItemType,
                        })
                    );
                }
                else
                {
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
            else
            {
                Console.WriteLine($"실패: return 형식이 잘못되었습니다.");
            }

            return result;
        }

        public async Task<List<ItemType>> GetItemTypes()
        {
            List<ItemType> result = new List<ItemType>();
            
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
                    result = new List<ItemType>(
                        res.ItemTypes.Select(t => new ItemType() 
                        { 
                            Id = t.Id,
                            Name = t.Name
                        })
                    );
                }
                else
                {
                    Console.WriteLine($"실패: {res.Message}");
                }
            }
            else
            {
                Console.WriteLine($"실패: return 형식이 잘못되었습니다.");
            }

            return result;
        }

        public async Task<bool> RemoveItem(int id)
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
                    return true;
                }
                else
                {
                    Console.WriteLine($"실패: {res.Message}");
                    return false;
                }
            }
            else
            {
                Console.WriteLine($"실패: return 형식이 잘못되었습니다.");
            }

            return false;
        }
    }
}
