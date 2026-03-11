using System;

namespace INF714.Data.Analytics
{
    public class LoginEvent : Event
    {
        public LoginEvent()
        {
            base.Type = "login";
        }

        public Guid UserId { get; set; }
    };
}
