using System;
using System.Threading.Tasks;

namespace INF714.Data.Providers.Interfaces
{
    public interface IUserProvider
    {
        Task Create(User user);
        Task<User> Get(Guid id);
        Task Save(User user);
    }
}
