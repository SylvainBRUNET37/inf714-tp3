using System;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace INF714.Data.Providers
{
    public class InMemoryUserProvider : Interfaces.IUserProvider
    {
        Dictionary<Guid, User> _users = new Dictionary<Guid, User>();

        public InMemoryUserProvider()
        {

        }

        public async Task Create(User user)
        {
            if(_users.ContainsKey(user.Id))
            {
                throw new System.Exception("User already exists.");
            }
            _users.Add(user.Id, user);
        }

        public async Task<User> Get(Guid id)
        {
            User user = new User();
            _users.TryGetValue(id, out user);
            return user;
        }

        public async Task Save(User user)
        {
            
        }
    }
}
