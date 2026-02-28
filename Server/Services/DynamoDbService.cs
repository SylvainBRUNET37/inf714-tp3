using Amazon.DynamoDBv2;

namespace INF714.Services
{
    public class DynamoDbService
    {
        readonly AmazonDynamoDBClient _client;
        
        public DynamoDbService(bool isProduction)
        {
            if(isProduction)
            {
                _client = new AmazonDynamoDBClient();
            }
            else
            {
                _client = new AmazonDynamoDBClient("1234", "1234", new AmazonDynamoDBConfig() { ServiceURL = "http://localhost:8000" });
            }
        }

        public AmazonDynamoDBClient Client
        {
            get { return _client; }
        }
    }
}
