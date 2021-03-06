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

.int iMag_SG550;

// Weapon Info
weaponinfo_t wptSG550 = { 
	WEAPON_SG550, 		// Identifier
	SLOT_PRIMARY,		// Slot
	4200, 				// Price
	CALIBER_556MM, 		// Caliber ID
	0.84, 				// Max Player Speed
	1, 					// Bullets Per Shot
	30, 				// Clip/MagSize
	70, 				// Damage Per Bullet
	2, 					// Penetration Multiplier
	8192, 				// Bullet Range
	0.98, 				// Range Modifier
	TYPE_AUTO,			// Firing Type
	0.25, 				// Attack-Delay
	3.8, 				// Reload-Delay
	iAmmo_556MM, 		// Caliber Pointer
	iMag_SG550, 		// Clip Pointer
	200,				// Accuracy Divisor
	0.55,				// Accuracy Offset
	1.4,				// Max Inaccuracy
	5,					// Minimum Crosshair Distance
	3,					// Crosshair Movement Delta
	1.45,				// Armor penetration ratio
	ATYPE_RIFLE,		// Animation Type
	SHELL_RIFLE
};

// Anim Table
enum
{
	ANIM_SG550_IDLE,
	ANIM_SG550_SHOOT1,
	ANIM_SG550_SHOOT2,
	ANIM_SG550_RELOAD,
	ANIM_SG550_DRAW,
};

void WeaponSG550_Draw(void) {
	#ifdef SERVER
	BaseGun_Draw();
	#else
	View_PlayAnimation(ANIM_SG550_DRAW);
	#endif
}

void WeaponSG550_PrimaryFire(void) {
	#ifdef SERVER
	if (BaseGun_PrimaryFire() == TRUE) {
		sound(self, CHAN_WEAPON, "weapons/sg550-1.wav", 1, ATTN_NORM);
	}
	#else

	if (random() <= 0.5) {
		View_PlayAnimation(ANIM_SG550_SHOOT1);
	} else {
		View_PlayAnimation(ANIM_SG550_SHOOT2);
	} 
	BaseGun_ShotMultiplierHandle(1);
	#endif
}

void WeaponSG550_SecondaryFire(void) {
#ifdef SERVER
	if (self.viewzoom == 1.0) {
		self.viewzoom = 0.45;
	} else if (self.viewzoom == 0.45) {
		self.viewzoom = 0.15;
	} else {
		self.viewzoom = 1.0;
	}
	
	self.fAttackFinished = time + 0.5;
#endif
}

void WeaponSG550_Reload(void) {
	#ifdef SERVER
	if (BaseGun_Reload() == TRUE) {
		// Play Sound
	}
	#else
	View_PlayAnimation(ANIM_SG550_RELOAD);
	#endif
}
