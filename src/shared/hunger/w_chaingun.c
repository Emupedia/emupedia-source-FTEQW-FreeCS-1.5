/*
 * Copyright (c) 2016-2020 Marco Hladik <marco@icculus.org>
 * Copyright (c) 2019-2020 Gethyn ThomasQuail <xylemon@posteo.net>
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

/* Animations */
enum
{
	CHAINGUN_IDLE1,
	CHAINGUN_IDLE2,
	CHAINGUN_SPINUP,
	CHAINGUN_SPINDOWN,
	CHAINGUN_FIRE,
	CHAINGUN_DRAW,
	CHAINGUN_HOLSTER
};

void
w_chaingun_precache(void)
{
#ifdef SERVER
	Sound_Precache("weapon_chaingun.fire");
	Sound_Precache("weapon_chaingun.reload");
	Sound_Precache("weapon_chaingun.spindown");
	Sound_Precache("weapon_chaingun.spinup");
#endif
	precache_model("models/v_tfac.mdl");
	precache_model("models/w_tfac.mdl");
	precache_model("models/p_tfac.mdl");
}

int
w_chaingun_pickup(int new)
{
#ifdef SERVER
	player pl = (player)self;

	if (new) {
		pl.chaingun_mag = 100;
	} else {
		if (pl.ammo_9mm < MAX_A_9MM) {
			pl.ammo_9mm = bound(0, pl.ammo_9mm + 100, MAX_A_9MM);
		} else {
			return FALSE;
		}
	}
#endif
	return TRUE;
}

void
w_chaingun_updateammo(player pl)
{
#ifdef SERVER
	Weapons_UpdateAmmo(pl, pl.chaingun_mag, pl.ammo_9mm, -1);
#endif
}

string
w_chaingun_wmodel(void)
{
	return "models/w_tfac.mdl";
}

string
w_chaingun_pmodel(void)
{
	return "models/p_tfac.mdl";
}

string
w_chaingun_deathmsg(void)
{
	return "%s was rolled over by %s' Chaingun.";
}

void
w_chaingun_draw(void)
{
#ifdef CLIENT
	Weapons_SetModel("models/v_tfac.mdl");
	Weapons_ViewAnimation(CHAINGUN_DRAW);
#endif
}

void
w_chaingun_holster(void)
{
	Weapons_ViewAnimation(CHAINGUN_HOLSTER);
}

void
w_chaingun_release(void)
{
	player pl = (player)self;

	if (pl.w_idle_next > 0.0) {
		return;
	}

	/* end firing */
	if (pl.a_ammo3 == 1) {
		pl.a_ammo3 = 0;
		Sound_Play(pl, CHAN_WEAPON, "weapon_chaingun.spindown");
		Weapons_ViewAnimation(CHAINGUN_SPINDOWN);
		pl.w_attack_next = 1.0f;
		pl.w_idle_next = pl.w_attack_next;
		return;
	}

	/* end reload */
	if (pl.a_ammo3 == 2) {
		pl.a_ammo3 = 0;
		Weapons_ViewAnimation(CHAINGUN_DRAW);
		pl.w_attack_next = 1.0f;
		pl.w_idle_next = pl.w_attack_next;
		return;
	}

	int r = (float)input_sequence % 2;
	if (r) {
		Weapons_ViewAnimation(CHAINGUN_IDLE1);
	} else {
		Weapons_ViewAnimation(CHAINGUN_IDLE2);
	}

	pl.w_idle_next = 15.0f;
}

void
w_chaingun_primary(void)
{
	player pl = (player)self;

	/* in case we're spamming primary while reloading */
	if (pl.a_ammo3 == 2) {
		w_chaingun_release();
		return;
	}

	if (pl.w_attack_next > 0.0) {
		return;
	}

	/* ammo check */
#ifdef CLIENT
	if (pl.a_ammo1 <= 0) {
		return;
	}
#else
	if (pl.chaingun_mag <= 0) {
		return;
	}
#endif

	/* spin up first */
	if (pl.a_ammo3 == 0) {
		pl.a_ammo3 = 1;
		Weapons_ViewAnimation(CHAINGUN_SPINUP);
		Sound_Play(pl, CHAN_WEAPON, "weapon_chaingun.spinup");
		pl.w_attack_next = 0.5f;
		pl.w_idle_next = pl.w_attack_next;
		return;
	}

	/* actual firing */
#ifdef CLIENT
	pl.a_ammo1--;
	View_SetMuzzleflash(MUZZLE_RIFLE);
	Weapons_ViewAnimation(CHAINGUN_FIRE);
	Weapons_ViewPunchAngle([random(-2, 2),0,0]);
#else
	pl.chaingun_mag--;
	TraceAttack_FireBullets(1, Weapons_GetCameraPos(), 8, [0.15,0.15], WEAPON_CHAINGUN);
	Sound_Play(pl, CHAN_WEAPON, "weapon_chaingun.fire");
#endif

	pl.w_attack_next = 0.1f;
	pl.w_idle_next = 0.0f;
}

void
w_chaingun_reload(void)
{
	player pl = (player)self;
	if (pl.w_attack_next) {
		w_chaingun_release();
		return;
	}

	/* ammo check */
#ifdef CLIENT
	if (pl.a_ammo1 >= 100) {
		return;
	}
	if (pl.a_ammo2 <= 0) {
		return;
	}
#else
	if (pl.chaingun_mag >= 100) {
		return;
	}
	if (pl.ammo_9mm <= 0) {
		return;
	}
#endif

#ifdef CLIENT
	Weapons_ViewAnimation(CHAINGUN_HOLSTER);
#else
	Sound_Play(pl, CHAN_WEAPON, "weapon_chaingun.reload");
	Weapons_ReloadWeapon(pl, player::chaingun_mag, player::ammo_9mm, 100);
#endif

	pl.a_ammo3 = 2;
	pl.w_attack_next = 10.0f;
	pl.w_idle_next = 1.5f;
}

void
w_chaingun_hud(void)
{
	w_glock_hud();
}

float
w_chaingun_aimanim(void)
{
	return w_mp5_aimanim();
}

void
w_chaingun_hudpic(int selected, vector pos, float a)
{
#ifdef CLIENT
	if (selected) {
		drawsubpic(
			pos,
			[170,45],
			"sprites/tfchud04.spr_0.tga",
			[0,90/256],
			[170/256,45/256],
			g_hud_color,
			a,
			DRAWFLAG_ADDITIVE
		);
	} else {
		drawsubpic(
			pos,
			[170,45],
			"sprites/tfchud03.spr_0.tga",
			[0,45/256],
			[170/256,45/256],
			g_hud_color,
			a,
			DRAWFLAG_ADDITIVE
		);
	}
#endif
}

weapon_t w_chaingun =
{
	.id			= ITEM_CHAINGUN,
	.slot		= 3,
	.slot_pos	= 3,
	.ki_spr		= "sprites/tfc_dmsg.spr_0.tga",
	.ki_size	= [48,16],
	.ki_xy		= [0,16],
	.draw		= w_chaingun_draw,
	.holster	= w_chaingun_holster,
	.primary	= w_chaingun_primary,
	.secondary	= w_chaingun_release,
	.reload		= w_chaingun_reload,
	.release	= w_chaingun_release,
	.crosshair	= w_chaingun_hud,
	.precache	= w_chaingun_precache,
	.pickup		= w_chaingun_pickup,
	.updateammo	= w_chaingun_updateammo,
	.wmodel		= w_chaingun_wmodel,
	.pmodel		= w_chaingun_pmodel,
	.deathmsg	= w_chaingun_deathmsg,
	.aimanim	= w_chaingun_aimanim,
	.hudpic		= w_chaingun_hudpic
};

/* pickups */
#ifdef SERVER
void
weapon_th_chaingun(void)
{
	Weapons_InitItem(WEAPON_CHAINGUN);
}
#endif
