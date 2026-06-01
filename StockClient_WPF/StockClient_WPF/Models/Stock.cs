using System;
using System.Collections.Generic;
using System.Text;

namespace StockClient_WPF.Models
{
    public class Stock
    {
        // TODO: 추후 범위 늘리기.
        public static readonly long MAX_STOCK_SIZE = 100000000;
        public static readonly long MIN_STOCK_SIZE = -100000000;
        public int Id { get; set; }
        public string ItemName { get; set; }
        public int ItemId { get; set; }
        public uint Count { get; set; }
    }
}
