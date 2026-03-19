using System;
using System.Collections.Generic;

namespace INF714.Data
{
    public class User
    {
        public Guid Id { get; set; }
        public string GuestToken { get; set; }
        public ulong SteamID { get; set; }

        string _name = string.Empty;
        public string Name 
        { 
            get { return _name; }
            set
            {
                if(value.Length > 16)
                {
                    throw new ArgumentOutOfRangeException("Name", "Name length must be less than 16 characters long.");
                }
                _name = value;
            }
        }

        uint _level = 0;
        public uint Level 
        {
            get
            {
                return _level;
            }

            set
            {
                if(value > 100)
                {
                    throw new ArgumentOutOfRangeException("Level", "Level must be between 0 and 100.");
                }
                _level = value;
            }
        }

        public Dictionary<uint, Item> Items { get; } = new Dictionary<uint, Item>();
    }
}
