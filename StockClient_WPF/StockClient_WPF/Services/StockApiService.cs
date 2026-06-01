using StockClient_WPF.Interfaces;
using StockClient_WPF.Models;
using StockServiceProtocol;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;

namespace StockClient_WPF.Services
{
    public class StockApiService : IStockApiService
    {
        private IServerConnection<Packet, Packet> _serverConnection;

        public StockApiService(IServerConnection<Packet, Packet> serverConnection)
        {
            this._serverConnection = serverConnection;
        }

        public async Task<bool> AddStock(int itemId, uint count)
        {
            Packet packet = new Packet
            {
                Type = PacketType.AddStock,
                AddStockReq = new AddStockRequest()
                {
                    StockDto = new StockDto()
                    {
                        ItemId = itemId,
                        Count = count,
                    }
                }
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.AddStock)
            {
                AddStockResponse res = resP.AddStockRes;
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

        public async Task<List<Stock>> GetStocks()
        {
            List<Stock> result = new List<Stock>();

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
                    result = new List<Stock>(
                        res.StockDto.Select(t => new Stock()
                        {
                            Id = t.Id,
                            ItemName = "",
                            Count = t.Count,
                            ItemId = t.ItemId,
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

        public async Task<bool> ReduceStock(int itemId, uint count)
        {
            Packet packet = new Packet
            {
                Type = PacketType.ReduceStock,
                ReduceStockReq = new ReduceStockRequest()
                {
                    StockDto = new StockDto()
                    {
                        ItemId = itemId,
                        Count = count,
                    }
                }
            };

            this._serverConnection.Send(packet);
            Packet resP = this._serverConnection.Receive(1024);

            if (resP.Type == PacketType.ReduceStock)
            {
                ReduceStockResponse res = resP.ReduceStockRes;
                if (res.Status)
                {
                    return true;
                }
                else
                {
                    // null일 경우 화면에 에러 문구 띄워주기 -> 다시 서버에 요청해주세요.
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
