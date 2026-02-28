using System;

namespace INF714.Data
{
    public class Auction
    {
        public Guid Id { get; set; }
        public Guid SellerUserId { get; set; }
        public uint SellerItemId { get; set; }
        public uint Price { get; set; }
        public DateTime Timestamp { get; set; }
    }
}
