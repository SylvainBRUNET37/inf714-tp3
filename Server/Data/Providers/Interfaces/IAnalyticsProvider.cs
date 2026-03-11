using System.Threading.Tasks;

namespace INF714.Data.Providers.Interfaces
{
    public interface IAnalyticsProvider
    {
        Task SendEvent(Analytics.Event evt);
    }
}
