using Amazon.DynamoDBv2;
using Amazon.DynamoDBv2.Model;
using INF714.Services;
using System;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace INF714.Data.Providers
{
    public class DynamoDbUserProvider : Interfaces.IUserProvider
    {
        readonly AmazonDynamoDBClient _client;
        const string _tableName = "inf714_users";

        public DynamoDbUserProvider(DynamoDbService dynamoDbService)
        {
            _client = dynamoDbService.Client;
        }

        #region Serialization Boilerplate

        private Dictionary<string, AttributeValue> ConvertItemToDynamoDbItem(Item userItem)
        {
            var item = new Dictionary<string, AttributeValue>();
            if(!string.IsNullOrEmpty(userItem.Name)) item.Add("name", new AttributeValue() { S = userItem.Name });
            item.Add("amount", new AttributeValue() { N = userItem.Amount.ToString() });
            return item;
        }

        private Item ConvertItemFromDynamoDbItem(Dictionary<string, AttributeValue> item)
        {
            var userItem = new Item();
            AttributeValue value;
            if(item.TryGetValue("name", out value)) userItem.Name = value.S;
            if(item.TryGetValue("amount", out value)) userItem.Amount = uint.Parse(value.N);
            return userItem;
        }

        private Dictionary<string, AttributeValue> ConvertUserToDynamoDbItem(User user)
        {
            var item = new Dictionary<string, AttributeValue>();
            item.Add("id", new AttributeValue() { S = user.Id.ToString() });
            item.Add("guestToken", new AttributeValue() { S = user.GuestToken });
            item.Add("steamId", new AttributeValue() { N = user.SteamID.ToString() });
            if(user.Name is not null) 
                item.Add("name", new AttributeValue() { S = user.Name });
            item.Add("level", new AttributeValue() { N = user.Level.ToString() } );

            var userItemsItem = new Dictionary<string, AttributeValue>();
            foreach(var userItemPair in user.Items)
            {
                userItemsItem.Add(userItemPair.Key.ToString(), new AttributeValue() { M = ConvertItemToDynamoDbItem(userItemPair.Value) });
            }
            if(userItemsItem.Count != 0) item.Add("items", new AttributeValue() { M = userItemsItem });

            return item;
        }

        private User ConvertUserFromDynamoDbItem(Dictionary<string, AttributeValue> item)
        {
            var user = new User();
            AttributeValue value;
            if(item.TryGetValue("id", out value)) user.Id = Guid.Parse(value.S);
            if(item.TryGetValue("guestToken", out value)) user.GuestToken = value.S;
            if(item.TryGetValue("name", out value)) user.Name = value.S;
            if(item.TryGetValue("level", out value)) user.Level = uint.Parse(value.N);
            if(item.TryGetValue("steamId", out value)) user.SteamID = ulong.Parse(value.N);

            if(item.TryGetValue("items", out value))
            {
                foreach(var itemPair in value.M)
                {
                    user.Items.Add(uint.Parse(itemPair.Key), ConvertItemFromDynamoDbItem(itemPair.Value.M));
                }
            }

            return user;
        }
        
        #endregion

        public async Task Create(User user)
        {
            var request = new PutItemRequest();
            request.TableName = _tableName;
            request.ConditionExpression = "attribute_not_exists(id)";
            request.Item = ConvertUserToDynamoDbItem(user);
            await _client.PutItemAsync(request);
        }

        public async Task<User> Get(Guid id)
        {
            var key = new Dictionary<string, AttributeValue>();
            key.Add("id", new AttributeValue() { S = id.ToString() });

            var request = new GetItemRequest();
            request.TableName = _tableName;
            request.Key = key;
            var response = await _client.GetItemAsync(request);

            if(!response.IsItemSet)
            {
                return null;
            }

            return ConvertUserFromDynamoDbItem(response.Item);
        }

        public async Task Save(User user)
        {
            var request = new PutItemRequest();
            request.TableName = _tableName;
            request.Item = ConvertUserToDynamoDbItem(user);
            request.ConditionExpression = "attribute_exists(id)";
            await _client.PutItemAsync(request);
        }

        public async Task<User> GetFromSteamID(ulong steamId)
        {
            var request = new ScanRequest
            {
                TableName = _tableName,
                FilterExpression = "steamId = :steamId",
                ExpressionAttributeValues = new Dictionary<string, AttributeValue>()
                {
                    { ":steamId", new AttributeValue() { N = steamId.ToString() } }
                }
            };

            var response = await _client.ScanAsync(request);
            if(response.Items.Count == 0)
            {
                return null;
            }

            return ConvertUserFromDynamoDbItem(response.Items[0]);
        }
    }
}
