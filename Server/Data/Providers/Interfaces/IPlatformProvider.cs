using System.Threading.Tasks;

namespace INF714.Data.Providers.Interfaces
{
    public interface IPlatformProvider
    {
        Task<ulong> GetIDFromAuthTicket(string authTicket);
    }
}
