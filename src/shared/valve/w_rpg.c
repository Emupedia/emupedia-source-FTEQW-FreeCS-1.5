/*
 * Copyright (c) 2016-2020 Marco Hladik <marco@icculus.org>
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

/*QUAKED weapon_rpg (0 0 1) (-16 -16 0) (16 16 32)
"model" "models/w_rpg.mdl"

HALF-LIFE (1998) ENTITY

RPG Weapon

*/

enum
{
	RPG_IDLE,
	RPG_FIDGET,
	RPG_RELOAD,
	RPG_FIRE2,
	RPG_HOLSTER1,
	RPG_DRAW1,
	RPG_HOLSTER2,
	RPG_DRAW_UL,
	RPG_IDLE_UL,
	RPG_FIDGET_UL,
};

void w_rpg_precache(void)
{
#ifdef SERVER
	Sound_Precache("weapon_rpg.shoot");
#endif
	precache_model("models/v_rpg.mdl");
	precache_model("models/w_rpg.mdl");
	precache_model("models/p_rpg.mdl");
	precache_model("models/rpgrocket.mdl");
	precache_model("sprites/laserdot.spr");
}

void w_rpg_updateammo(player pl)
{
#ifdef SERVER
	Weapons_UpdateAmmo(pl, pl.rpg_mag, pl.ammo_rocket, -1);
#endif
}
string w_rpg_wmodel(void)
{
	return "models/w_rpg.mdl";
}
string w_rpg_pmodel(void)
{
	return "models/p_rpg.mdl";
}
string w_rpg_deathmsg(void)
{
	return "";
}

int w_rpg_pickup(int new)
{
#ifdef SERVER
	player pl = (player)self;

	if (new) {
		pl.rpg_mag = 1;
	} else {
		if (pl.ammo_rocket < MAX_A_ROCKET) {
			pl.ammo_rocket = bound(0, pl.ammo_rocket + 1, MAX_A_ROCKET);
		} else {
			return FALSE;
		}
	}
#endif
	return TRUE;
}

void w_rpg_draw(void)
{
#ifdef CLIENT
	Weapons_SetModel("models/v_rpg.mdl");
	Weapons_ViewAnimation(RPG_DRAW1);
#endif
}

void w_rpg_holster(void)
{
	
}

void w_rpg_primary(void)
{
	player pl = (player)self;
	if (pl.w_attack_next > 0.0) {
		return;
	}

	/* Ammo check */
#ifdef CLIENT
	if (pl.a_ammo1 <= 0) {
		return;
	}
#else
	if (pl.rpg_mag <= 0) {
		return;
	}
#endif

	Weapons_ViewAnimation(RPG_FIRE2);

#ifdef CLIENT
	pl.a_ammo1--;
	Weapons_ViewPunchAngle([-10,0,0]);
#else
	static void Rocket_Touch(void) {
		float dmg = Skill_GetValue("plr_rpg");
		Effect_CreateExplosion(self.origin);
		Damage_Radius(self.origin, self.owner, dmg, dmg * 2.5f, TRUE, WEAPON_RPG);
		sound(self, CHAN_WEAPON, sprintf("weapons/explode%d.wav", floor(random() * 2) + 3), 1, ATTN_NORM);
		remove(self);
	}
	static void Rocket_BuildSpeed(void){
		/* Calculate new direction */
		if (self.weapon) {
			makevectors(self.owner.v_angle);
			traceline(self.owner.origin, self.owner.origin + v_forward * 8096, FALSE, self.owner);
			self.angles = vectoangles(trace_endpos - self.origin);
		}

		/* Increase speed towards it */
		makevectors(self.angles);
		self.velocity += (v_forward * 2000) * frametime;
		self.nextthink = time;
	}

	Weapons_MakeVectors();
	entity rocket = spawn();
	setmodel(rocket, "models/rpgrocket.mdl");
	setorigin(rocket, Weapons_GetCameraPos() + (v_forward * 16));
	rocket.owner = self;
	rocket.movetype = MOVETYPE_FLY;
	rocket.solid = SOLID_BBOX;
	//bolt.flags |= FL_LAGGEDMOVE;
	rocket.gravity = 0.5f;
	rocket.velocity = (v_forward * 250);
	rocket.angles = vectoangles(rocket.velocity);
	rocket.avelocity[2] = 10;
	rocket.touch = Rocket_Touch;
	rocket.think = Rocket_BuildSpeed;
	rocket.nextthink = time + 0.15f;
	rocket.traileffectnum = particleeffectnum("weapon_rpg.trail");

	if (pl.a_ammo3 > 0) {
		rocket.weapon = 1;
	}

	setsize(rocket, [0,0,0], [0,0,0]);
	Sound_Play(pl, CHAN_WEAPON, "weapon_rpg.shoot");
	pl.rpg_mag--;
#endif

	pl.w_attack_next = 
	pl.w_idle_next = 2.5f;
}

void w_rpg_reload(void)
{
	player pl = (player)self;
	

	if (pl.w_attack_next > 0) {
		return;
	}

	/* Ammo check */
#ifdef CLIENT
	if (pl.a_ammo1 >= 1) {
		return;
	}
	if (pl.a_ammo2 <= 0) {
		return;
	}
#else
	if (pl.rpg_mag >= 1) {
		return;
	}
	if (pl.ammo_rocket <= 0) {
		return;
	}
#endif

	/* Audio-Visual Bit */
#ifdef CLIENT
	Weapons_ViewAnimation(RPG_RELOAD);
#else
	Weapons_ReloadWeapon(pl, player::rpg_mag, player::ammo_rocket, 1);
#endif

	pl.w_attack_next = 2.25f;
	pl.w_idle_next = 10.0f;
}

void w_rpg_release(void)
{
	player pl = (player)self;
	if (pl.w_idle_next > 0.0) {
		return;
	}

#ifdef CLIENT
	if (pl.a_ammo1 <= 0 && pl.a_ammo2 > 0) {
		w_rpg_reload();
	}
#else
	if (pl.rpg_mag <= 0 && pl.ammo_rocket > 0) {
		w_rpg_reload();
	}
#endif

	int r = (float)input_sequence % 3;

	if (pl.a_ammo1 > 0) {
		if (r == 1) {
			Weapons_ViewAnimation(RPG_FIDGET);
		} else {
			Weapons_ViewAnimation(RPG_IDLE);
		}
	} else {
		if (r == 1) {
			Weapons_ViewAnimation(RPG_FIDGET_UL);
		} else {
			Weapons_ViewAnimation(RPG_IDLE_UL);
		}
	}

	pl.w_idle_next = 6.0f;
}

void w_rpg_secondary(void)
{
	player pl = (player)self;

	if (pl.w_attack_next > 0.0) {
		return;
	}

	/* toggle laser */
	pl.a_ammo3 = 1 - pl.a_ammo3;

	pl.w_attack_next = 1.0f;
	w_rpg_release();
}

float w_rpg_aimanim(void)
{
	return self.flags & FL_CROUCHING ? ANIM_CR_AIMRPG : ANIM_AIMRPG;
}

void w_rpg_hudpic(int selected, vector pos, float a)
{
#ifdef CLIENT
	if (selected) {
		drawsubpic(pos, [170,45], "sprites/640hud5.spr_0.tga", [0,45/256], [170/256,45/256], g_hud_color, a, DRAWFLAG_ADDITIVE);
	} else {
		drawsubpic(pos, [170,45], "sprites/640hud2.spr_0.tga", [0,45/256], [170/256,45/256], g_hud_color, a, DRAWFLAG_ADDITIVE);
	}
#endif
}

void w_rpg_hud(void)
{
#ifdef CLIENT
	player pl = (player)self;
	vector cross_pos;
	vector aicon_pos;

	/* crosshair/laser */
	if (pl.a_ammo3 == 1) {
		float lerp;
		vector jitter;
		Weapons_MakeVectors();
		vector src = pl.origin + pl.view_ofs;
		traceline(src, src + (v_forward * 256), FALSE, pl);
		lerp = Math_Lerp(18,6, trace_fraction);
		jitter[0] = (random(0,2) - 2) * (1 - trace_fraction);
		jitter[1] = (random(0,2) - 2) * (1 - trace_fraction);
		cross_pos = g_hudmins + (g_hudres / 2) + ([-lerp,-lerp] / 2);
		drawsubpic(
			cross_pos + jitter,
			[lerp,lerp],
			"sprites/laserdot.spr_0.tga",
			[0,0],
			[1.0, 1.0],
			[1,1,1],
			1.0f,
			DRAWFLAG_ADDITIVE
		);
	} else {
		cross_pos = g_hudmins + (g_hudres / 2) + [-12,-12];
		drawsubpic(
			cross_pos,
			[24,24],
			"sprites/crosshairs.spr_0.tga",
			[0,0],
			[0.1875, 0.1875],
			[1,1,1],
			1,
			DRAWFLAG_NORMAL
		);
	}

	/* ammo counters */
	HUD_DrawAmmo1();
	HUD_DrawAmmo2();

	/* ammo icon */
	aicon_pos = g_hudmins + [g_hudres[0] - 48, g_hudres[1] - 42];
	drawsubpic(
		aicon_pos,
		[24,24],
		"sprites/640hud7.spr_0.tga",
		[120/256,72/128],
		[24/256, 24/128],
		g_hud_color,
		pSeat->m_flAmmo2Alpha,
		DRAWFLAG_ADDITIVE
	);
#endif
}

weapon_t w_rpg =
{
	.id		= ITEM_RPG,
	.slot		= 3,
	.slot_pos	= 0,
	.ki_spr		= "sprites/640hud1.spr_0.tga",
	.ki_size	= [48,16],
	.ki_xy		= [192,96],
	.draw		= w_rpg_draw,
	.holster	= w_rpg_holster,
	.primary	= w_rpg_primary,
	.secondary	= w_rpg_secondary,
	.reload		= w_rpg_reload,
	.release	= w_rpg_release,
	.crosshair	= w_rpg_hud,
	.precache	= w_rpg_precache,
	.pickup		= w_rpg_pickup,
	.updateammo	= w_rpg_updateammo,
	.wmodel		= w_rpg_wmodel,
	.pmodel		= w_rpg_pmodel,
	.deathmsg	= w_rpg_deathmsg,
	.aimanim	= w_rpg_aimanim,
	.hudpic		= w_rpg_hudpic
};

#ifdef SERVER
void weapon_rpg(void) {
	Weapons_InitItem(WEAPON_RPG);
}
#endif

