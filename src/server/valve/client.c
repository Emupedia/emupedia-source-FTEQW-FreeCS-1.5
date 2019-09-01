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

void Empty(void) {}

void Game_ClientConnect(void)
{
	bprint(PRINT_HIGH, sprintf("%s connected\n", self.netname));

	int playercount = 0;
	for (entity eFind = world; (eFind = find(eFind, classname, "player"));) {
		playercount++;
	}

	/* We're the first. */	
	if (playercount == 0) {
		for (entity a = world; (a = findfloat(a, gflags, GF_CANRESPAWN));) {
         	CBaseEntity caw = (CBaseEntity)a;
            caw.Respawn();
         }		
	}
}

void Game_ClientDisconnect(void)
{
	bprint(PRINT_HIGH, sprintf("%s disconnected\n", self.netname));
	
	/* Make this unusable */
	self.solid = SOLID_NOT;
	self.movetype = MOVETYPE_NONE;
	self.modelindex = 0;
	self.health = 0;
	self.takedamage = 0;
	self.SendFlags = 1;
}

void Game_ClientKill(void)
{
	Damage_Apply(self, self, self.health, self.origin, TRUE);	
}

void Game_PlayerPreThink(void)
{
	
}

void Game_PlayerPostThink(void)
{
	Animation_PlayerUpdate();
	self.SendFlags = 1;
}
void Game_RunClientCommand(void)
{
	Footsteps_Update();
	QPhysics_Run(self);
}

void Game_DecodeChangeParms(void)
{
	player pl = (player)self;
	g_landmarkpos[0] = parm1;
	g_landmarkpos[1] = parm2;
	g_landmarkpos[2] = parm3;
	pl.angles[0] = parm4;
	pl.angles[1] = parm5;
	pl.angles[2] = parm6;
	pl.velocity[0] = parm7;
	pl.velocity[1] = parm8;
	pl.velocity[2] = parm9;
	pl.g_items = parm10;
	pl.activeweapon = parm11;
}
void Game_SetChangeParms(void)
{
	player pl = (player)self;
	parm1 = g_landmarkpos[0];
	parm2 = g_landmarkpos[1];
	parm3 = g_landmarkpos[2];
	parm4 = pl.angles[0];
	parm5 = pl.angles[1];
	parm6 = pl.angles[2];
	parm7 = pl.velocity[0];
	parm8 = pl.velocity[1];
	parm9 = pl.velocity[2];
	parm10 = pl.g_items;
	parm11 = pl.activeweapon;
}

void Game_PutClientInServer(void)
{
	if (self.classname != "player") {
		spawnfunc_player();
	}
	player pl = (player)self;

	entity spot;
	pl.classname = "player";
	pl.health = self.max_health = 100;
	//forceinfokey(self, "*dead", "0");
	pl.takedamage = DAMAGE_YES;
	pl.solid = SOLID_SLIDEBOX;
	pl.movetype = MOVETYPE_WALK;
	pl.flags = FL_CLIENT;
	pl.viewzoom = 1.0;
	pl.model = "models/player.mdl";
	
	string mymodel = infokey(pl, "model");
	
	if (mymodel) {
		mymodel = sprintf("models/player/%s/%s.mdl", mymodel, mymodel);
		if (whichpack(mymodel)) {
			pl.model = mymodel;
		}
	} 
	setmodel(pl, pl.model);
	
	setsize(pl, VEC_HULL_MIN, VEC_HULL_MAX);
	pl.view_ofs = VEC_PLAYER_VIEWPOS;
	pl.velocity = [0,0,0];
	pl.frame = 1;
	pl.SendEntity = Player_SendEntity;
	pl.customphysics = Empty;
	pl.vPain = Player_Pain;
	pl.vDeath = Player_Death;
	pl.iBleeds = TRUE;
	forceinfokey(pl, "*spec", "0");
	forceinfokey(self, "*deaths", ftos(self.deaths));

	if (cvar("sv_playerslots") == 1) {
		Game_DecodeChangeParms();

		if (startspot != "") {
			print(sprintf("[LEVEL] Startspot is \"%s\"\n", startspot));
			setorigin(pl, Landmark_GetSpot());
		} else {
			spot = find(world, classname, "info_player_start");
			setorigin(pl, spot.origin);
			pl.angles = spot.angles;
			pl.fixangle = TRUE;
		}
	} else {
		spot = Spawn_SelectRandom("info_player_deathmatch");
		setorigin(pl, spot.origin);
		pl.angles = spot.angles;
		pl.fixangle = TRUE;

		pl.ammo_9mm = 68;
		Weapons_AddItem(pl, WEAPON_CROWBAR);
		Weapons_AddItem(pl, WEAPON_GLOCK);
	}
}

void SV_SendChat(entity eSender, string sMessage, entity eEnt, float fType)
{
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, fType == 0 ? EV_CHAT:EV_CHAT_TEAM);
	WriteByte(MSG_MULTICAST, num_for_edict(eSender) - 1); 
	WriteByte(MSG_MULTICAST, eSender.team); 
	WriteString(MSG_MULTICAST, sMessage);
	if (eEnt) {
		msg_entity = eEnt;
		multicast([0,0,0], MULTICAST_ONE);
	} else {
		multicast([0,0,0], MULTICAST_ALL);
	}
}

void Game_ParseClientCommand(string cmd)
{
	tokenize(cmd);

	if (argv(1) == "timeleft") {
		float fTimeLeft = cvar("mp_timelimit") - (time / 60);
		Vox_Singlecast(self, sprintf("we have %s minutes remaining", Vox_TimeToString(fTimeLeft)));
		return;
	}

	// Players talk to players, spectators to spectators.
	if (argv(0) == "say") {
		localcmd(sprintf("echo [SERVER] %s: %s\n", self.netname, argv(1)));
		SV_SendChat(self, argv(1), world, 0);
		return;
	} else if (argv(0) == "say_team") {
		localcmd(sprintf("echo [TEAM %d] %s: %s\n", self.team, self.netname, argv(1)));
		for (entity eFind = world; (eFind = find(eFind, classname, "player"));) { 
			if (eFind.team == self.team) {
				SV_SendChat(self, argv(1), eFind, 1);
			}
		}
		return;
	}

	clientcommand(self, cmd);
}

void Game_SetNewParms(void)
{

}