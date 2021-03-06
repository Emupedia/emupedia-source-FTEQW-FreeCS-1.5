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

.int iMag_SG552;

// Weapon Info
weaponinfo_t wptSG552 = { 
	WEAPON_SG552, 		// Identifier
	SLOT_PRIMARY,		// Slot
	3500, 				// Price
	CALIBER_556MM, 		// Caliber ID
	0.94, 				// Max Player Speed
	1, 					// Bullets Per Shot
	30, 				// Clip/MagSize
	33, 				// Damage Per Bullet
	2, 					// Penetration Multiplier
	8192, 				// Bullet Range
	0.955, 				// Range Modifier
	TYPE_AUTO,			// Firing Type
	0.0825, 				// Attack-Delay
	3.2, 				// Reload-Delay
	iAmmo_556MM, 		// Caliber Pointer
	iMag_SG552, 		// Clip Pointer
	220,				// Accuracy Divisor
	0.3,				// Accuracy Offset
	1.0,				// Max Inaccuracy
	5,					// Minimum Crosshair Distance
	3,					// Crosshair Movement Delta
	1.4,				// Armor penetration ratio
	ATYPE_RIFLE,		// Animation Type
	SHELL_RIFLE
};

// Anim Table
enum
{
	ANIM_SG552_IDLE,
	ANIM_SG552_RELOAD,
	ANIM_SG552_DRAW,
	ANIM_SG552_SHOOT1,
	ANIM_SG552_SHOOT2,
	ANIM_SG552_SHOOT3
};

void WeaponSG552_Draw(void) {
#ifdef SERVER
	BaseGun_Draw();
#else
	View_PlayAnimation(ANIM_SG552_DRAW);
#endif
}

void WeaponSG552_PrimaryFire(void) {
#ifdef SERVER
	if (BaseGun_PrimaryFire() == TRUE) {
		if (random() <= 0.5) {
			sound(self, CHAN_WEAPON, "weapons/sg552-1.wav", 1, ATTN_NORM);
		} else {
			sound(self, CHAN_WEAPON, "weapons/sg552-2.wav", 1, ATTN_NORM);
		}
	}
#else
	int iRand = (int)floor(random(1, 4));
	if (iRand == 1) {
		View_PlayAnimation(ANIM_SG552_SHOOT1);
	} else if (iRand == 2) {
		View_PlayAnimation(ANIM_SG552_SHOOT2);
	} else {
		View_PlayAnimation(ANIM_SG552_SHOOT3);
	}
	
	BaseGun_ShotMultiplierHandle(1);
#endif
}

void WeaponSG552_SecondaryFire(void) {
#ifdef SERVER
	if (self.viewzoom != 1.0) {
		self.viewzoom = 1.0;
	} else {
		self.viewzoom = 0.6;
	}
	
	self.fAttackFinished = time + 0.5;
#endif
}

void WeaponSG552_Reload(void) {
#ifdef SERVER
	if (BaseGun_Reload() == TRUE) {
		// Play Sound
	}
#else
	View_PlayAnimation(ANIM_SG552_RELOAD);
#endif
}
