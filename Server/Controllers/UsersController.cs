using System;
using System.ComponentModel.DataAnnotations;
using System.Security.Cryptography;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using INF714.Data;
using INF714.Data.Providers.Interfaces;
using Amazon.GameLift.Model;

namespace INF714.Controllers
{
    class UserResponse
    {
        public Guid Id { get; set; }
        public string GuestToken { get; set; }
    }

    [Route("api/[controller]")]
    [ApiController]
    public class UsersController : ControllerBase
    {
        private IUserProvider _userProvider;
        private IPlatformProvider _platformProvider;
        private IAnalyticsProvider _analyticProvider;

        public UsersController(IUserProvider userProvider, IPlatformProvider platformProvider, IAnalyticsProvider analyticProvider)
        {
            _userProvider = userProvider;
            _platformProvider = platformProvider;
            _analyticProvider = analyticProvider;
        }

        static string GenerateRandomCryptographicKey(int keyLength)
        {
            var generator = RandomNumberGenerator.Create();
            byte[] randomBytes = new byte[keyLength];
            generator.GetNonZeroBytes(randomBytes);
            return Convert.ToBase64String(randomBytes);
        }

        [Authorize("CanAccessSelfInfo")]
        [HttpGet("{userId}/disconnect")]
        public async Task<ActionResult> Disconnect(Guid userId)
        {
            await SendEvent(userId, "playerDisconnected");

            return Ok();
        }

        [HttpPost]
        public async Task<ActionResult> Create()
        {
            var user = new User
            {
                Id = Guid.NewGuid(),
                GuestToken = GenerateRandomCryptographicKey(32),
                Name = ""
            };

            await _userProvider.Create(user);
            await SendEvent(user.Id, "userCreated");

            return CreatedAtAction(nameof(Get), new { userId = user.Id }, user);
        }

        [HttpPost("{authToken}")]
        public async Task<ActionResult> CreateFromSteam(string authToken)
        {
            var user = await GetFromSteam(authToken);
            if (user is null)
            {
                user = new User
                {
                    Id = Guid.NewGuid(),
                    GuestToken = GenerateRandomCryptographicKey(32),
                    SteamID = await _platformProvider.GetIDFromAuthTicket(authToken),
                    Name = ""
                };

                await _userProvider.Create(user);
                await SendEvent(user.Id, "userCreated");
            }
            else
            {
                await SendEvent(user.Id, "login");
            }

            var response = new UserResponse()
            {
                Id = user.Id,
                GuestToken = user.GuestToken
            };

            return CreatedAtAction(nameof(Get), new { userId = user.Id }, response);
        }

        [Authorize("CanAccessSelfInfo")]
        [HttpGet("{userId}")]
        public async Task<ActionResult> Get(Guid userId)
        {
            var user = await _userProvider.Get(userId);
            if(user == null) return NotFound();
            user.GuestToken = null;
            return Ok(user);
        }

        [HttpGet("{userId}/profile")]
        public async Task<ActionResult> GetProfile(Guid userId)
        {
            var user = await _userProvider.Get(userId);
            if(user == null) return NotFound();
            user.GuestToken = null;
            return Ok(user);
        }

        [Authorize("CanAccessSelfInfo")]
        [HttpPatch("{userId}")]
        public async Task<ActionResult> Update(Guid userId, string name, uint? level)
        {
            var user = await _userProvider.Get(userId);
            if(user == null) return NotFound();
            try
            {
                if(name != null)
                {
                    user.Name = name;
                }
                if(level.HasValue)
                {
                    user.Level = level.Value;
                }
                await _userProvider.Save(user);
            }
            catch(ArgumentException ex)
            {
                return BadRequest(ex);
            }
            return Ok(user);
        }

        [Authorize("CanAccessSelfInfo")]
        [HttpPut("{userId}/name")]
        public async Task<ActionResult> UpdateName(Guid userId, [Required] string value)
        {
            var user = await _userProvider.Get(userId);
            if(user == null) return NotFound();
            user.Name = value;

            await _userProvider.Save(user);
            await SendNameChangedEvent(userId, "nameChanged", value);

            return Ok();
        }

        //

        private async Task<User> GetFromSteam(string authToken)
        {
            var steamId = await _platformProvider.GetIDFromAuthTicket(authToken);

            return await _userProvider.GetFromSteamID(steamId);
        }

        private async Task SendNameChangedEvent(Guid userId, string eventName, string newName)
        {
            var analyticsEvent = new Data.Analytics.NameChangedEvent
            {
                UserId = userId,
                Type = eventName,
                UserName = newName
            };

            await _analyticProvider.SendEvent(analyticsEvent);
        }

        private async Task SendEvent(Guid userId, string name)
        {
            var analyticsEvent = new Data.Analytics.LoginEvent
            {
                UserId = userId,
                Type = name
            };

            await _analyticProvider.SendEvent(analyticsEvent);
        }
    }
}
