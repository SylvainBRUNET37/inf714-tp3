using System;

namespace INF714.Data.Analytics
{
    public class Event
    {
        public Guid Id { get; set; } = Guid.NewGuid();
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
        public string Type { get; set; }
    };
}
