using System;

namespace INF714.Data.Analytics
{
    public class NameChangedEvent : LoginEvent
    {
        public NameChangedEvent()
        {
            Type = "nameChanged";
        }

        public string UserName { get; set; }
    };
}
