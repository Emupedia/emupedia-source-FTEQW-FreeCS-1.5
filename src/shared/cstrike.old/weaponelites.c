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

.int iMag_ELITES;

#ifdef SERVER
//.int iMode_ELITES;
#else
int iWeaponMode_ELITES;
#endif

// Weapon Info
weaponinfo_t wptELITES = { 
	WEAPON_ELITES, 		// Identifier
	SLOT_SECONDARY,		// Slot
	800, 				// Price
	CALIBER_9MM, 		// Caliber ID
	1.0, 				// Max Player Speed
	1, 					// Bullets Per Shot
	30, 				// Clip/MagSize
	45, 				// Damage Per Bullet
	1, 					// Penetration Multiplier
	4096, 				// Bullet Range
	0.75, 				// Range Modifier
	TYPE_SEMI,			// Firing Type
	0.15, 				// Attack-Delay
	4.6, 				// Reload-Delay
	iAmmo_9MM, 			// Caliber Pointer
	iMag_ELITES, 		// Clip Pointer
	200,				// Accuracy Divisor
	0.55,				// Accuracy Offset
	1.4,				// Max Inaccuracy
	4,					// Minimum Crosshair Distance
	3,					// Crosshair Movement Delta
	1.05,				// Armor penetration ratio
	ATYPE_DUALPISTOLS,	// Animation Type
	SHELL_PISTOL
};

// Anim Table
enum
{
	ANIM_ELITES_IDLE,
	ANIM_ELITES_IDLE_LEFTEMPTY,
	ANIM_ELITES_SHOOT_LEFT1,
	ANIM_ELITES_SHOOT_LEFT2,
	ANIM_ELITES_SHOOT_LEFT3,
	ANIM_ELITES_SHOOT_LEFT4,
	ANIM_ELITES_SHOOT_LEFT5,
	ANIM_ELITES_SHOOT_LEFTLAST,
	ANIM_ELITES_SHOOT_RIGHT1,
	ANIM_ELITES_SHOOT_RIGHT2,
	ANIM_ELITES_SHOOT_RIGHT3,
	ANIM_ELITES_SHOOT_RIGHT4,
	ANIM_ELITES_SHOOT_RIGHT5,
	ANIM_ELITES_SHOOT_RIGHTLAST,
	ANIM_ELITES_RELOAD,
	ANIM_ELITES_DRAW
};

void WeaponELITES_Draw(void) {
#ifdef SERVER
	BaseGun_Draw();
	sound(self, CHAN_WEAPON, "weapons/elite_deploy.wav", 1, ATTN_IDLE); // TODO: Move to the client...?
#else
	View_PlayAnimation(ANIM_ELITES_DRAW);
#endif
}

void WeaponELITES_PrimaryFire(void) {
#ifdef SERVER
	if (BaseGun_PrimaryFire() == TRUE) {
		// Play Sound
		sound(self, CHAN_WEAPON, "weapons/elite_fire.wav", 1, ATTN_NORM);
	}
#else
	int iRand = (int)floor(random(1, 4));
	if (iWeaponMode_ELITES == 0) {
		if (getstatf(STAT_CURRENT_MAG) == 1) {
			View_PlayAnimation(ANIM_ELITES_SHOOT_LEFTLAST);
		} else {
			if (iRand == 1) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_LEFT1);
			} else if (iRand == 2) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_LEFT2);
			} else if (iRand == 3) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_LEFT3);
			} else if (iRand == 4) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_LEFT4);
			} else {
				View_PlayAnimation(ANIM_ELITES_SHOOT_LEFT5);
			}
		}
	} else {
		if (getstatf(STAT_CURRENT_MAG) == 0) {
			View_PlayAnimation(ANIM_ELITES_SHOOT_RIGHTLAST);
		} else {
			if (iRand == 1) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_RIGHT1);
			} else if (iRand == 2) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_RIGHT2);
			} else if (iRand == 3) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_RIGHT3);
			} else if (iRand == 4) {
				View_PlayAnimation(ANIM_ELITES_SHOOT_RIGHT4);
			} else {
				View_PlayAnimation(ANIM_ELITES_SHOOT_RIGHT5);
			}
		}
	}
	
	BaseGun_ShotMultiplierHandle(1);
	iWeaponMode_ELITES = 1 - iWeaponMode_ELITES;
#endif
}

void WeaponELITES_Reload(void) {
#ifdef SERVER
	if (BaseGun_Reload() == TRUE) {
		// Play Sound
	}
#else
	View_PlayAnimation(ANIM_ELITES_RELOAD);
	iWeaponMode_ELITES = 0;
#endif
}
