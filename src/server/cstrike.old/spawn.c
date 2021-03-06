/*
 * Copyright (c) 2016-2019 Marco Hladik <marco@icculus.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

entity eLastTSpawn;
entity eLastCTSpawn;

/*
=================
PlayerFindSpawn

Recursive function that gets the next spawnpoint
=================
*/
entity PlayerFindSpawn(float fTeam)
{
	entity eSpot, eLastSpawn;
	entity eThing;
	int iCount;
	string sClassname;

	if (fTeam == TEAM_T) {
		sClassname = "info_player_deathmatch";
		eSpot = eLastSpawn = eLastTSpawn;
	} else if (fTeam == TEAM_CT) {
		sClassname = "info_player_start";
		eSpot = eLastSpawn = eLastCTSpawn;
	} else if (fTeam == TEAM_VIP) {
		return find(world, classname, "info_vip_start");
	}

	while (1) {
		eSpot = find(eSpot, classname, sClassname);
		
		if (eSpot == eLastSpawn)
		{//fall back on lame cycling/spawnfragging
			eLastSpawn = find(eLastSpawn, classname, sClassname);
			if (!eLastSpawn)
				eLastSpawn = find(eLastSpawn, classname, sClassname);
			return eLastSpawn;
		}
		if (eSpot != world) {
			iCount = 0;
			eThing = findradius(eSpot.origin, 32);
			while(eThing) {
				if (eThing.classname == "player")
					iCount++;
				eThing = eThing.chain;
			}
			if (iCount == 0) {
				eLastSpawn = eSpot;
				return eSpot;
			}
		}
	}

	return eSpot;
}

/*
=================
PlayerRoundRespawn

Called whenever a player just needs his basic properties to be reset
=================
*/
void PlayerRoundRespawn(float fTeam)
{
	entity eSpawn;
	forceinfokey(self, "*spec", "0");
	eSpawn = PlayerFindSpawn(self.team);

	self.classname = "player";
	self.health = self.max_health = 100;
	forceinfokey(self, "*dead", "0");
	Rules_CountPlayers();

	self.takedamage = DAMAGE_YES;
	self.solid = SOLID_SLIDEBOX;
	self.movetype = MOVETYPE_WALK;
	self.flags = FL_CLIENT;
	//self.Pain = Player_Pain;
	//self.Death = Player_Death;
	self.iBleeds = TRUE;
	self.fSlotC4Bomb = 0; // Clear the C4
	self.viewzoom = 1.0; // Clear scopes

	self.origin = eSpawn.origin;
	self.angles = eSpawn.angles;
	self.fixangle = TRUE;

	// Get the player-model from Defs.h's list
	if (self.team != TEAM_VIP) {
		setmodel(self, sCSPlayers[self.fCharModel]);
	} else {
		setmodel(self, "models/player/vip/vip.mdl");
	}
	setsize(self, VEC_HULL_MIN, VEC_HULL_MAX);

	self.view_ofs = VEC_PLAYER_VIEWPOS;
	self.velocity = '0 0 0';

	self.frame = 1; // Idle frame
	self.fBombProgress = 0;

	Ammo_AutoFill(self.fSlotPrimary);
	Ammo_AutoFill(self.fSlotSecondary);
}

/*
=================
PlayerSpawnIngame

Called whenever a player becomes a completely new type of player
=================
*/
void PlayerSpawnIngame(float fCharModel)
{
	// What team are we on - 0= Spectator, < 5 Terrorists, CT rest
	if(fCharModel == 0) {
		PutClientInServer();
		Spawn_ObserverCam();
		return;
	} else if(fCharModel < 5) {
		forceinfokey(self, "*team", "0"); 
		self.team = TEAM_T;
		
		Weapon_AddItem(WEAPON_KNIFE);
		if (autocvar_fcs_knifeonly == FALSE) {
			Weapon_AddItem(WEAPON_GLOCK18);
			Weapon_GiveAmmo(WEAPON_GLOCK18, 40);
			Weapon_Draw(WEAPON_GLOCK18);
		} else {
			Weapon_Draw(WEAPON_KNIFE);
		}
	} else {
		self.team = TEAM_CT;

		Weapon_AddItem(WEAPON_KNIFE);
		if (autocvar_fcs_knifeonly == FALSE) {
			Weapon_AddItem(WEAPON_USP45);
			Weapon_GiveAmmo(WEAPON_USP45, 24);
			Weapon_Draw(WEAPON_USP45);
		} else {
			Weapon_Draw(WEAPON_KNIFE);
		}
	}

	if(self.iInGame == FALSE) {
		self.iInGame = TRUE;
	}

	forceinfokey(self, "*team", ftos(self.team)); 
	PlayerRoundRespawn(self.team);
	self.fAttackFinished = time + 1;
}

/*
=================
PlayerMakeSpectator

Called on connect and whenever a player dies
=================
*/
void PlayerMakeSpectator(void)
{
	self.classname = "spectator";

	self.health = 0;
	self.armor = 0;
	self.takedamage = DAMAGE_NO;
	self.solid = SOLID_NOT;
	self.movetype = MOVETYPE_NOCLIP;
	self.flags = FL_CLIENT;
	self.weapon = 0;
	self.viewzoom = 1.0f;

	self.model = 0;
	setsize (self, '-16 -16 -16', '16 16 16');

	self.view_ofs = self.velocity = '0 0 0';
	forceinfokey(self, "*spec", "2"); // Make sure we are known as a spectator

	Ammo_Clear();

	// Clear the inventory
	self.fSlotMelee = self.fSlotPrimary = self.fSlotSecondary = self.fSlotGrenade = self.iEquipment = 0;
}

/*
=================
CSEv_GamePlayerSpawn_f

Event Handling, called by the Client codebase via 'sendevent'
=================
*/
void CSEv_GamePlayerSpawn_f(float fChar)
{
	if (self.team == TEAM_VIP) {
		centerprint(self, "You are the VIP!\nYou cannot switch roles now.\n");
		self.fAttackFinished = time + 1.0;
		return;
	}

	// Hey, we are alive and are trying to switch teams, so subtract us from the Alive_Team counter.
	if (self.health > 0) {
		self.health = 0;
		Rules_CountPlayers();
		Rules_DeathCheck();
		Player_Death(0);
	}

	Ammo_Clear();

	// Spawn the players immediately when its in the freeze state
	switch (fGameState) {
		case GAME_FREEZE:
			self.fCharModel = fChar;
			PlayerSpawnIngame(fChar);

			if ((self.team == TEAM_T) && (iAlivePlayers_T == 1)) {
				if (iBombZones > 0) {
					Rules_MakeBomber();
				}
			} else if ((self.team == TEAM_CT) && (iAlivePlayers_CT == 1)) {
				if (iVIPZones > 0) {
					Rules_MakeVIP();
				}
			}

			break;
		default:
			if (fChar == 0) {
				PutClientInServer();
				return;
			} else if(fChar < 5) {
				self.team = TEAM_T;
			} else {
				self.team = TEAM_CT;
			}

			PlayerMakeSpectator();
			self.classname = "player";
			self.fCharModel = fChar;
			self.health = 0;
			forceinfokey(self, "*dead", "1");
			forceinfokey(self, "*team", ftos(self.team)); 
			break;
	}

	self.frags = 0;
	self.fDeaths = 0;
	forceinfokey(self, "*deaths", "0");

	// Split up for readability and expandability?
	if ((self.team == TEAM_T) && (iAlivePlayers_T == 0)) {
		Rules_RoundOver(FALSE, 0, FALSE);
	} else if ((self.team == TEAM_CT) && (iAlivePlayers_CT == 0)) {
		Rules_RoundOver(FALSE, 0, FALSE);
	}
}

/*
=================
info_player_start

Counter-Terrorist Spawnpoints
=================
*/
void info_player_start(void)
{
	if (autocvar_fcs_swapteams == TRUE) {
		self.classname = "info_player_deathmatch";
	}
}

/*
=================
info_player_deathmatch

Terrorist Spawnpoints
=================
*/
void info_player_deathmatch(void)
{
	if (autocvar_fcs_swapteams == TRUE) {
		self.classname = "info_player_start";
	}
}

/* Counter-Strike: Source compat */
void info_player_counterterrorist(void)
{
	setorigin(self, self.origin + [0,0,32]);
	self.classname = "info_player_start";
	info_player_start();
}

void info_player_terrorist(void)
{
	setorigin(self, self.origin + [0,0,32]);
	self.classname = "info_player_deathmatch";
	info_player_deathmatch();
}

/*
=================
info_vip_start
=================
*/
void info_vip_start(void)
{
}
