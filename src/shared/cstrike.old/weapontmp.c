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

.int iMag_TMP;

// Weapon Info
weaponinfo_t wptTMP = { 
	WEAPON_TMP, 		// Identifier
	SLOT_PRIMARY,		// Slot
	1250, 				// Price
	CALIBER_9MM, 		// Caliber ID
	1.0, 				// Max Player Speed
	1, 					// Bullets Per Shot
	30, 				// Clip/MagSize
	26, 				// Damage Per Bullet
	1, 					// Penetration Multiplier
	4096, 				// Bullet Range
	0.84, 				// Range Modifier
	TYPE_AUTO,			// Firing Type
	0.07, 				// Attack-Delay
	2.1, 				// Reload-Delay
	iAmmo_9MM, 			// Caliber Pointer
	iMag_TMP, 			// Clip Pointer
	200,				// Accuracy Divisor
	0.55,				// Accuracy Offset
	1.4,				// Max Inaccuracy
	7,					// Minimum Crosshair Distance
	3,					// Crosshair Movement Delta
	1.0,				// Armor penetration ratio
	ATYPE_MP5,			// Animation Type
	SHELL_PISTOL
};

// Anim Table
enum
{
	ANIM_TMP_IDLE,
	ANIM_TMP_RELOAD,
	ANIM_TMP_DRAW,
	ANIM_TMP_SHOOT1,
	ANIM_TMP_SHOOT2,
	ANIM_TMP_SHOOT3
};

void WeaponTMP_Draw(void) {
#ifdef SERVER
	BaseGun_Draw();
#else
	View_PlayAnimation(ANIM_TMP_DRAW);
#endif
}

void WeaponTMP_PrimaryFire(void) {
#ifdef SERVER
	if (BaseGun_PrimaryFire() == TRUE) {
		if (random() <= 0.5) {
			sound(self, CHAN_WEAPON, "weapons/tmp-1.wav", 1, ATTN_NORM);
		} else {
			sound(self, CHAN_WEAPON, "weapons/tmp-2.wav", 1, ATTN_NORM);
		}
	}
#else
	int iRand = (int)floor(random(1, 4));
	if (iRand == 1) {
		View_PlayAnimation(ANIM_TMP_SHOOT1);
	} else if (iRand == 2) {
		View_PlayAnimation(ANIM_TMP_SHOOT2);
	} else {
		View_PlayAnimation(ANIM_TMP_SHOOT3);
	}
	
	BaseGun_ShotMultiplierHandle(1);
#endif
}

void WeaponTMP_Reload(void) {
#ifdef SERVER
	if (BaseGun_Reload() == TRUE) {
		// Play Sound
	}
#else
	View_PlayAnimation(ANIM_TMP_RELOAD);
#endif
}
