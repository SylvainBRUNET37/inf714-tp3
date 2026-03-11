using System;
using System.Collections.Generic;

namespace INF714.Data.Providers.Interfaces
{
    public interface IScoreProvider
    {
        void Add(Guid userId, int score);
        List<Score> GetScores(int start, int end);
        long GetRank(Guid userId);
    }
}
