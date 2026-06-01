using StockClient_WPF.Models;
using System;
using System.Collections.Generic;
using System.Text;

namespace StockClient_WPF.Interfaces
{
    public interface IStockApiService
    {
        Task<List<Stock>> GetStocks();

        Task<bool> AddStock(int itemId, uint count);

        Task<bool> ReduceStock(int itemId, uint count);
    }
}
