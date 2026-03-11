using System;
using System.Threading.Tasks;
using Nest;

namespace INF714.Data.Providers
{
    public class ElasticSearchAnalyticsProvider : Interfaces.IAnalyticsProvider
    {
        ElasticClient _client;

        public ElasticSearchAnalyticsProvider()
        {
            var node = new Uri("http://localhost:9200");
            var settings = new ConnectionSettings(node);
            _client = new ElasticClient(settings);
        }

        public async Task SendEvent(Analytics.Event evt)
        {
            var indexName = String.Format("events-{0:D4}.{1:D2}.{2:D2}",
                evt.Timestamp.Year, evt.Timestamp.Month, evt.Timestamp.Day);
            var result = await _client.IndexAsync(evt, idx => idx.Index(indexName));
            if(!result.IsValid) throw new System.Exception();
        }
    }
}
