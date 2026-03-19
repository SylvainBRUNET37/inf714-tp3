using System;
using System.Net.Http;
using System.Text.Json;
using System.Threading.Tasks;
using Nest;

namespace INF714.Data.Providers
{
    public class SteamProvider : Interfaces.IPlatformProvider
    {
        private const string STEAM_API_KEY = "EAC5F0D9EB6CA722E902B3D9F31BE64D";
        private const string STEAM_APP_ID = "480";


        public SteamProvider()
        {
            
        }

        public Task<ulong> GetIDFromAuthTicket(string authTicket)
        {
            var requestUrl = CreateSteamRequestUrl(authTicket);

            using var httpClient = new HttpClient();
            var response = httpClient.GetAsync(requestUrl).Result;
            if (!response.IsSuccessStatusCode) 
                throw new Exception("Failed to validate Steam auth ticket");
                
            var content = response.Content.ReadAsStringAsync().Result;
            var jsonResponse = JsonDocument.Parse(content);

            var steamIDString = jsonResponse
                                .RootElement
                                .GetProperty("response")
                                .GetProperty("params")
                                .GetProperty("steamid")
                                .GetString();
            
            var steamID = ulong.Parse(steamIDString);

            return Task.FromResult(steamID);
        }

        private string CreateSteamRequestUrl(string authTicket)
        {
            return $"https://api.steampowered.com/ISteamUserAuth/AuthenticateUserTicket/v1/?key={STEAM_API_KEY}&appid={STEAM_APP_ID}&ticket={authTicket}";
        }
    }
}
