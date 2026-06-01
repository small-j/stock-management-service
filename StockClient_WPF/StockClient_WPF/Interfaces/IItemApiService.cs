using StockClient_WPF.Models;
using System;
using System.Collections.Generic;
using System.Text;

namespace StockClient_WPF.Interfaces
{
    public interface IItemApiService
    {
        Task<List<Item>> GetItems();

        Task<bool> AddItem(int itemType, string name);

        Task<bool> RemoveItem(int id);

        Task<List<ItemType>> GetItemTypes();
    }
}
