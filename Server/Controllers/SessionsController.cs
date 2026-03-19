using System;
using System.ComponentModel.DataAnnotations;
using System.IdentityModel.Tokens.Jwt;
using System.Text;
using System.Threading.Tasks;
using System.Security.Claims;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using Microsoft.IdentityModel.Tokens;
using INF714.Data.Providers.Interfaces;

namespace INF714.Controllers
{
    [Route("api/users/{userId}/[controller]")]
    [ApiController]
    public class SessionsController : ControllerBase
    {
        private IUserProvider _userProvider;
        private IPlatformProvider _platformProvider;

        public SessionsController(IUserProvider userProvider, IPlatformProvider platformProvider)
        {
            _userProvider = userProvider;
            _platformProvider = platformProvider;
        }

        private string CreateSessionToken(Guid userId)
        {
            var key = Encoding.ASCII.GetBytes(Globals.ACCESS_TOKEN_KEY);
            var tokenDescriptor = new SecurityTokenDescriptor
            {
                Subject = new ClaimsIdentity(new Claim[]
                {
                    new Claim(ClaimTypes.Name, userId.ToString())
                }),
                Expires = DateTime.UtcNow.AddDays(1),
                SigningCredentials = new SigningCredentials(new SymmetricSecurityKey(key), SecurityAlgorithms.HmacSha256Signature)
            };

            var tokenHandler = new JwtSecurityTokenHandler();
            var token = tokenHandler.CreateToken(tokenDescriptor);
            return tokenHandler.WriteToken(token);
        }

        [Authorize("CanAccessSelfInfo")]
        [HttpPost("refresh")]
        public ActionResult RefreshSession(Guid userId)
        {
            var sessionToken = CreateSessionToken(userId);
            return Ok(sessionToken);
        }

        [HttpPost("create")]
        public async Task<ActionResult> CreateSession(Guid userId, [FromForm] [Required] string guestToken)
        {
            var user = await _userProvider.Get(userId);
            if(user == null)
            {
                return NotFound();
            }
            
            if(string.IsNullOrEmpty(user.GuestToken))
            {
                return Unauthorized();
            }

            if(user.GuestToken != guestToken)
            {
                return Unauthorized();
            }

            var sessionToken = CreateSessionToken(userId);
            return Ok(sessionToken);
        }

        [HttpPost("createFromSteam")]
        public async Task<ActionResult> CreateSessionFromSteam(Guid userId, [FromForm] [Required] string authToken)
        {
            var steamId = await _platformProvider.GetIDFromAuthTicket(authToken);
            var user = await _userProvider.GetFromSteamID(steamId);

            if(user == null)
            {
                return NotFound();
            }
            
            if(string.IsNullOrEmpty(user.GuestToken))
            {
                return Unauthorized();
            }

            var sessionToken = CreateSessionToken(user.Id);
            return Ok(sessionToken);
        }
    }
}
