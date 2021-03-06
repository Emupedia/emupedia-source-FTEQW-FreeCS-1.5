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

/*QUAKED weapon_hegrenade (0 0 1) (-16 -16 0) (16 16 32)
"model" "models/w_hegrenade.mdl"

COUNTER-STRIKE (1999) ENTITY

HE (High Explosive) Grenade Weapon

When thrown, explodes with a fairly deadly blast radius to players.

- Buy Menu -
Price: $300

*/

enum
{
	HEGRENADE_IDLE,
	HEGRENADE_PULLPIN,
	HEGRENADE_THROW,
	HEGRENADE_DRAW,
};

void
w_hegrenade_precache(void)
{
#ifdef SERVER
	Sound_Precache("weapon_hegrenade.bounce");
	Sound_Precache("weapon_hegrenade.explode");
#endif
	precache_model("models/v_hegrenade.mdl");
	precache_model("models/w_hegrenade.mdl");
	precache_model("models/p_hegrenade.mdl");
}

void
w_hegrenade_updateammo(player pl)
{
#ifdef SERVER
	Weapons_UpdateAmmo(pl, -1, pl.ammo_hegrenade, pl.a_ammo3);
#endif
}

int
w_hegrenade_pickup(int new)
{
#ifdef SERVER
	player pl = (player)self;

	if (pl.ammo_hegrenade < 3) {
		pl.ammo_hegrenade = bound(0, pl.ammo_hegrenade + 1, 3);
	} else {
		return FALSE;
	}
#endif
	return TRUE;
}

string
w_hegrenade_wmodel(void)
{
	return "models/w_hegrenade.mdl";
}

string
w_hegrenade_pmodel(void)
{
	return "models/p_hegrenade.mdl";
}

string
w_hegrenade_deathmsg(void)
{
	return "";
}

void
w_hegrenade_draw(void)
{
	Weapons_SetModel("models/v_hegrenade.mdl");
	Weapons_ViewAnimation(HEGRENADE_DRAW);
}

#ifdef SERVER
void w_hegrenade_throw(void)
{
	static void hegrenade_explode(void)
	{
		float dmg = 100;
		Effect_CreateExplosion(self.origin);
		Damage_Radius(self.origin, self.owner, dmg, dmg * 2.5f, TRUE, WEAPON_HEGRENADE);
		Sound_Play(self, CHAN_BODY, "weapon_hegrenade.explode");
		remove(self);
	}
	
	static void hegrenade_touch(void)
	{
		if (other.takedamage == DAMAGE_YES) {
			Damage_Apply(other, self.owner, 15, WEAPON_HEGRENADE, DMG_BLUNT);
		} else {
			Sound_Play(self, CHAN_BODY, "weapon_hegrenade.bounce");
		}
		self.frame = 0;
	}

	player pl = (player)self;
	vector vPLAngle = pl.v_angle;
	if (vPLAngle[0] < 0) {
		vPLAngle[0] = -10 + vPLAngle[0] * ((90 - 10) / 90.0);
	} else {
		vPLAngle[0] = -10 + vPLAngle[0] * ((90 + 10) / 90.0);
	}

	float flVel = (90 - vPLAngle[0]) * 5;
	if (flVel > 1000) {
		flVel = 1000;
	}

	makevectors(vPLAngle);
	vector vecSrc = pl.origin + pl.view_ofs + v_forward * 16;
	vector vecThrow = v_forward * flVel + pl.velocity;

	entity eGrenade = spawn();
	eGrenade.owner = pl;
	eGrenade.classname = "remove_me";
	eGrenade.solid = SOLID_BBOX;
	eGrenade.frame = 1;
	eGrenade.velocity = vecThrow;
	eGrenade.movetype = MOVETYPE_BOUNCE;
	eGrenade.think = hegrenade_explode;
	eGrenade.touch = hegrenade_touch;
	eGrenade.nextthink = time + 4.0f;
	setmodel(eGrenade, "models/w_hegrenade.mdl");
	setsize(eGrenade, [0,0,0], [0,0,0]);
	setorigin(eGrenade, vecSrc);
}
#endif

void
w_hegrenade_primary(void)
{
	player pl = (player)self;
	if (pl.w_attack_next > 0.0) {
		return;
	}
	
	/* We're abusing this network variable for the holding check */
	if (pl.a_ammo3 > 0) {
		return;
	}

	/* Ammo check */
#ifdef CLIENT
	if (pl.a_ammo2 <= 0) {
		return;
	}
#else
	if (pl.ammo_hegrenade <= 0) {
		return;
	}
#endif

	Weapons_ViewAnimation(HEGRENADE_PULLPIN);

	pl.a_ammo3 = 1;
	pl.w_attack_next = 0.975f;
	pl.w_idle_next = pl.w_attack_next;
}

void
w_hegrenade_release(void)
{
	player pl = (player)self;
	
	if (pl.w_idle_next > 0.0) {
		return;
	}

	if (pl.a_ammo3 == 1) {
#ifdef CLIENT
		pl.a_ammo2--;
		Weapons_ViewAnimation(HEGRENADE_THROW);
#else
		pl.ammo_hegrenade--;
		w_hegrenade_throw();
#endif
		pl.a_ammo3 = 2;
		pl.w_attack_next = 1.0f;
		pl.w_idle_next = 0.5f;
	} else if (pl.a_ammo3 == 2) {
#ifdef CLIENT
		Weapons_ViewAnimation(HEGRENADE_DRAW);
#else
		if (!pl.ammo_hegrenade) {
			Weapons_RemoveItem(pl, WEAPON_HEGRENADE);
		}
#endif
		pl.w_attack_next = 0.5f;
		pl.w_idle_next = 0.5f;
		pl.a_ammo3 = 0;
	}
}

float
w_hegrenade_aimanim(void)
{
	return w_flashbang_aimanim();
}

void
w_hegrenade_hud(void)
{
#ifdef CLIENT

	HUD_DrawAmmo2();
	vector aicon_pos = g_hudmins + [g_hudres[0] - 48, g_hudres[1] - 42];
	drawsubpic(aicon_pos, [24,24], "sprites/640hud7.spr_0.tga", [72/256,96/256], [24/256, 24/256], g_hud_color, pSeat->m_flAmmo2Alpha, DRAWFLAG_ADDITIVE);
#endif
}

void
w_hegrenade_hudpic(int selected, vector pos, float a)
{
#ifdef CLIENT
	if (selected) {
		drawsubpic(
			pos,
			[170,45],
			"sprites/640hud6.spr_0.tga",
			[0,45/256],
			[170/256,45/256],
			g_hud_color,
			1.0f,
			DRAWFLAG_ADDITIVE
		);
	} else {
		drawsubpic(
			pos,
			[170,45],
			"sprites/640hud3.spr_0.tga",
			[0,45/256],
			[170/256,45/256],
			g_hud_color,
			1.0f,
			DRAWFLAG_ADDITIVE
		);
	}
#endif
}

weapon_t w_hegrenade =
{
	ITEM_HEGRENADE,
	3,
	0,
	"sprites/640hud1.spr_0.tga",
	[32,16],
	[192,16],
	w_hegrenade_draw,
	__NULL__,
	w_hegrenade_primary,
	w_hegrenade_release,
	w_hegrenade_release,
	w_hegrenade_release,
	w_hegrenade_hud,
	w_hegrenade_precache,
	w_hegrenade_pickup,
	w_hegrenade_updateammo,
	w_hegrenade_wmodel,
	w_hegrenade_pmodel,
	w_hegrenade_deathmsg,
	w_hegrenade_aimanim,
	w_hegrenade_hudpic
};

#ifdef SERVER
void
weapon_hegrenade(void)
{
	Weapons_InitItem(WEAPON_HEGRENADE);
}
#endif
