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

.int iMag_MAC10;

// Weapon Info
weaponinfo_t wptMAC10 = { 
	WEAPON_MAC10, 		// Identifier
	SLOT_PRIMARY,		// Slot
	1400, 				// Price
	CALIBER_45ACP, 		// Caliber ID
	1.0, 				// Max Player Speed
	1, 					// Bullets Per Shot
	30, 				// Clip/MagSize
	29, 				// Damage Per Bullet
	1, 					// Penetration Multiplier
	4096, 				// Bullet Range
	0.82, 				// Range Modifier
	TYPE_AUTO,			// Firing Type
	0.07, 				// Attack-Delay
	3.2, 				// Reload-Delay
	iAmmo_45ACP, 		// Caliber Pointer
	iMag_MAC10, 		// Clip Pointer
	200,				// Accuracy Divisor
	0.6,				// Accuracy Offset
	1.65,				// Max Inaccuracy
	9,					// Minimum Crosshair Distance
	3,					// Crosshair Movement Delta
	0.95,				// Armor penetration ratio
	ATYPE_MP5,			// Animation Type
	SHELL_PISTOL
};

// Anim Table
enum
{
	ANIM_MAC10_IDLE,
	ANIM_MAC10_RELOAD,
	ANIM_MAC10_DRAW,
	ANIM_MAC10_SHOOT1,
	ANIM_MAC10_SHOOT2,
	ANIM_MAC10_SHOOT3
};

void WeaponMAC10_Draw(void) {
#ifdef SERVER
	BaseGun_Draw();
#else
	View_PlayAnimation(ANIM_MAC10_DRAW);
#endif
}

void WeaponMAC10_PrimaryFire(void) {
#ifdef SERVER
	if (BaseGun_PrimaryFire() == TRUE) {
		sound(self, CHAN_WEAPON, "weapons/mac10-1.wav", 1, ATTN_NORM);
	}
#else
	int iRand = (int)floor(random(1, 4));
	
	if (iRand == 1) {
		View_PlayAnimation(ANIM_MAC10_SHOOT1);
	} else if (iRand == 2) {
		View_PlayAnimation(ANIM_MAC10_SHOOT2);
	} else {
		View_PlayAnimation(ANIM_MAC10_SHOOT3);
	}
	BaseGun_ShotMultiplierHandle(1);
#endif
}

void WeaponMAC10_Reload(void) {
#ifdef SERVER
	if (BaseGun_Reload() == TRUE) {
		// Play Sound
	}
#else
	View_PlayAnimation(ANIM_MAC10_RELOAD);
#endif
}
