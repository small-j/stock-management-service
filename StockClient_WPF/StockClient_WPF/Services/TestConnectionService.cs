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
        private List<ItemDto> _items = new List<ItemDto>();
        private List<StockDto> _stocks = new List<StockDto>();

        private static int _gItemCnt = 0;
        private static int _gStockCnt = 0;

        public void Init()
        {
            _gItemCnt++;
            ItemDto itemDto = new ItemDto();
            itemDto.Id = 1;
            itemDto.Name = "test";
            itemDto.ItemType = 1;
            _items.Add(itemDto);

            _gStockCnt++;
            StockDto stockDto = new StockDto();
            stockDto.Id = 1;
            stockDto.Count = 5;
            stockDto.ItemId = 1;
            _stocks.Add(stockDto);
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
                case PacketType.GetItems:
                    result = new Packet
                    {
                        Type = PacketType.GetItems,
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
                case PacketType.GetStocks:
                    result = new Packet
                    {
                        Type = PacketType.GetStocks,
                        GetStocksRes = GetStocks()
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
            res.Menus.AddRange(new[] { "RemoveItem", "GetItem", "GetItemType", "AddStock", "ReduceStock", "GetStocks" });

            return res;
        }

        private AddItemResponse AddItem(Packet packet)
        {
            _gItemCnt++;
            _items.Add(new ItemDto
            {
                Id = _gItemCnt,
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
                _items.RemoveAt((int)id);
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

            for (int i = 0; i < _items.Count; i++)
            {
                res.ItemDto.Add(new ItemDto
                {
                    Id = _items[i].Id,
                    Name = _items[i].Name,
                    ItemType = _items[i].ItemType
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
            res.ItemTypes.Add(new ItemTypeDto() { Id=1, Name="Food" });
            res.ItemTypes.Add(new ItemTypeDto() { Id = 2, Name = "Clothes" });

            return res;
        }

        private AddStockResponse AddStock(Packet packet)
        {
            _gStockCnt++;
            StockDto target = null;
            for (int i = 0; i < _stocks.Count(); i++)
            {
                if (_stocks[i].ItemId == packet.AddStockReq.StockDto.ItemId)
                {
                    target = _stocks[i];
                    break;
                }
            }

            if (target == null)
            {
                _stocks.Add(new StockDto
                {
                    Id = _gStockCnt,
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

            for (int i = 0; i < _stocks.Count(); i++)
            {
                if (_stocks[i].ItemId == packet.AddStockReq.StockDto.ItemId)
                {
                    target = _stocks[i];
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

        private GetStocksResponse GetStocks()
        {
            var res = new GetStocksResponse
            {
                Status = true,
                Message = "success",
            };

            for (int i = 0; i < _items.Count; i++)
            {
                res.StockDto.Add(new StockDto
                {
                    Id = _stocks[i].Id,
                    Count = _stocks[i].Count,
                    ItemId = _stocks[i].ItemId
                });
            }

            return res;
        }
    }
}
