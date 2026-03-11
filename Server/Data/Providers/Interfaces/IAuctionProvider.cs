using System;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace INF714.Data.Providers.Interfaces
{
    public interface IAuctionProvider
    {
        Task Add(Auction auction);
        Task<List<Auction>> GetLastFive(Guid sellerUserId);
    }
}
