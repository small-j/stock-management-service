using System;
using System.Collections;
using System.Collections.Generic;
using StockClient_WPF.Interfaces;
using StockServiceProtocol;
using System.IO;

namespace StockClient_WPF.Services
{
    internal class TestConnectionService : IServerConnection<Packet, Packet>
    {
        private Queue<Packet> _request = new Queue<Packet>();
        private List<ItemDto> items = new List<ItemDto>();
        private List<StockDto> stocks = new List<StockDto>();

        public void Init()
        {
        }

        public void CreateConnection()
        {
            Console.WriteLine("서버 연결 성공");
        }

        public void DestroyConnection()
        {
        }

        public Packet Receive(int bufferSize = 1024)
        {
            if (_request.Count <= 0)
            {
                throw new InvalidDataException();
            }

            Packet? result = ServerData(_request.Dequeue());
            if (result == null)
            {
                throw new InvalidDataException();
            }

            return result;
        }

        public void Send(Packet obj)
        {
            _request.Enqueue(obj);
        }

        private Packet? ServerData(Packet req)
        {
            Packet? result = null;
            switch (req.Type)
            {
                case PacketType.GetMenu:
                    result = new Packet
                    {
                        Type = PacketType.GetMenu,
                        GetMenusRes = GetMenus()
                    };
                    break;
                case PacketType.AddItem:
                    result = new Packet
                    {
                        Type = PacketType.AddItem,
                        AddItemRes = AddItem(req)
                    };
                    break;
                case PacketType.RemoveItem:
                    result = new Packet
                    {
                        Type = PacketType.RemoveItem,
                        RemoveItemRes = RemoveItem(req)
                    };
                    break;
                case PacketType.PrintItem:
                    result = new Packet
                    {
                        Type = PacketType.PrintItem,
                        GetItemsRes = GetItems()
                    };
                    break;
                case PacketType.GetItemType:
                    result = new Packet
                    {
                        Type = PacketType.GetItemType,
                        GetItemTypesRes = GetItemType()
                    };
                    break;
                case PacketType.AddStock:
                    result = new Packet
                    {
                        Type = PacketType.AddStock,
                        AddStockRes = AddStock(req)
                    };
                    break;
                case PacketType.ReduceStock:
                    result = new Packet
                    {
                        Type = PacketType.ReduceStock,
                        ReduceStockRes = ReduceStock(req)
                    };
                    break;
                default:
                    break;
            }

            return result;
        }

        private GetMenusResponse GetMenus()
        {
            var res = new GetMenusResponse
            {
                Status = true,
                Message = "success",
            };
            res.Menus.Add("AddItem");
            res.Menus.AddRange(new[] { "RemoveItem", "PrintItem", "GetItemType", "AddStock", "ReduceStock" });

            return res;
        }

        private AddItemResponse AddItem(Packet packet)
        {
            items.Add(new ItemDto
            {
                Name = packet.AddItemReq.ItemDto.Name,
                ItemType = packet.AddItemReq.ItemDto.ItemType
            });

            var res = new AddItemResponse
            {
                Status = true,
                Message = "success",
            };

            return res;
        }

        private RemoveItemResponse RemoveItem(Packet packet)
        {
            var res = new RemoveItemResponse
            {
                Status = true,
                Message = "success"
            };
            int? id = packet.RemoveItemReq.ItemId;

            if (id == null)
            {
                res = new RemoveItemResponse
                {
                    Status = false,
                    Message = "fail"
                };
            }
            else
            {
                items.RemoveAt((int)id);
            }

            return res;
        }

        private GetItemsResponse GetItems()
        {
            var res = new GetItemsResponse
            {
                Status = true,
                Message = "success",
            };

            for (int i = 0; i < items.Count; i++)
            {
                res.ResItemDto.Add(new ResItemDto
                {
                    Id = i,
                    Name = items[i].Name,
                    ItemType = items[i].ItemType
                });
            }

            return res;
        }

        private GetItemTypesResponse GetItemType()
        {
            var res = new GetItemTypesResponse
            {
                Status = true,
                Message = "success",
            };
            res.ItemTypes.Add("Food");
            res.ItemTypes.Add("Clothes");

            return res;
        }

        private AddStockResponse AddStock(Packet packet)
        {
            StockDto target = null;
            for (int i = 0; i < stocks.Count(); i++)
            {
                if (stocks[i].ItemId == packet.AddStockReq.StockDto.ItemId)
                {
                    target = stocks[i];
                    break;
                }
            }

            if (target == null)
            {
                stocks.Add(new StockDto
                {
                    ItemId = packet.AddStockReq.StockDto.ItemId,
                    Count = packet.AddStockReq.StockDto.Count
                });
            }
            else
            {
                target.Count += packet.AddStockReq.StockDto.Count;
            }

            var res = new AddStockResponse
            {
                Status = true,
                Message = "success",
            };

            return res;
        }

        private ReduceStockResponse ReduceStock(Packet packet)
        {
            var res = new ReduceStockResponse
            {
                Status = true,
                Message = "success",
            };
            StockDto target = null;

            for (int i = 0; i < stocks.Count(); i++)
            {
                if (stocks[i].ItemId == packet.AddStockReq.StockDto.ItemId)
                {
                    target = stocks[i];
                    break;
                }
            }

            if (target == null)
            {
                res = new ReduceStockResponse
                {
                    Status = false,
                    Message = "fail",
                };
            }
            else
            {
                target.Count -= packet.AddStockReq.StockDto.Count;
            }

            return res;
        }
    }
}
