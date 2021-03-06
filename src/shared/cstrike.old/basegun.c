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

weaponinfo_t wptTable[CS_WEAPON_COUNT] = {
	{ 0, 0, 0, 0, 240, 0, 0, 0, 0, 0.0, 0.0, 0, 0.0, 0.0, iAmmo_9MM, iAmmo_9MM, 0.0, 0.0, 0.0, 0, 0 },
	wptKNIFE,
	wptUSP45,
	wptGLOCK18,
	wptDEAGLE,
	wptP228,
	wptELITES,
	wptFIVESEVEN,
	wptM3,
	wptXM1014,
	wptMP5,
	wptP90,
	wptUMP45,
	wptMAC10,
	wptTMP,
	wptAK47,
	wptSG552,
	wptM4A1,
	wptAUG,
	wptSCOUT,
	wptAWP,
	wptG3SG1,
	wptSG550,
	wptPARA,
	wptC4BOMB,
	wptFLASHBANG,
	wptHEGRENADE,
	wptSMOKEGRENADE
};

#ifdef SERVER
.int iShotMultiplier;
.float fDecreaseShotTime;
//.int iOldShotMultiplier;
#endif

/*
====================
BaseGun_ShotMultiplierHandle
====================
*/
void BaseGun_ShotMultiplierHandle(float fShots) {
#ifdef SERVER
	if (self.iShotMultiplier > 12) {
		self.iShotMultiplier = 12;
	} else {
		self.iShotMultiplier += fShots;
	}
	self.fDecreaseShotTime = time + 0.2;
#else
	vector vPunch;
	if (pSeat->iShotMultiplier > 12) {
		pSeat->iShotMultiplier = 12;
	} else {
		pSeat->iShotMultiplier += fShots;
	}
	
	vPunch[0] = -2 * (pSeat->iShotMultiplier / 6);
	vPunch[1] = random(-1, 1);
	/*player pl = (player)self;
	pl.punchangle += vPunch;*/
#endif
}

#ifdef SERVER
/*
====================
BaseGun_ShotMultiplierUpdate

This is being triggered in PlayerPreThink after the input
====================
*/
void BaseGun_ShotMultiplierUpdate(void) {
	if ((self.iShotMultiplier > 0) && (self.fDecreaseShotTime < time)) {
		self.fDecreaseShotTime = time + wptTable[self.weapon].fAttackFinished + 0.01;
		self.iShotMultiplier--;
	}	
}

/*
====================
BaseGun_Draw
====================
*/
void BaseGun_Draw(void) {
	self.iCurrentMag = self.(wptTable[self.weapon].iMagfld);
	self.iCurrentCaliber = self.(wptTable[self.weapon].iCaliberfld);
	Client_SendEvent(self, EV_WEAPON_DRAW);
}

/*
====================
BaseGun_AccuracyCalc
====================
*/
void BaseGun_AccuracyCalc(void) {
	if (wptTable[self.weapon].fAccuracyDivisor == -1) {
		if (self.viewzoom < 1.0f) {
			self.fAccuracy = 0.0f;
		} else {
			self.fAccuracy = 0.05f;
		}
	} else {
		self.fAccuracy = (self.iShotMultiplier / wptTable[self.weapon].fAccuracyDivisor);
	}
}

/*
====================
BaseGun_PrimaryFire

Returns whether or not to play an animation
====================
*/
float BaseGun_PrimaryFire(void) {
	// Nothing in the clip anymore? Don't even attempt
	if ((self.(wptTable[self.weapon].iMagfld) - 1) < 0) {
		return FALSE;
	}
	
	// Responsible for semi-automatic switch
	if (wptTable[self.weapon].fWeaponType == TYPE_SEMI) {
		self.flags = self.flags - (self.flags & FL_SEMI_TOGGLED);
	}
	
	BaseGun_ShotMultiplierHandle(wptTable[self.weapon].iBullets);
	BaseGun_AccuracyCalc();
	TraceAttack_FireBullets(wptTable[self.weapon].iBullets, (self.origin + self.view_ofs), wptTable[self.weapon].iDamage, [self.fAccuracy, self.fAccuracy], self.weapon);
	Animation_ShootWeapon(self);
	
	self.(wptTable[self.weapon].iMagfld) -= 1;
	self.fAttackFinished = time + wptTable[self.weapon].fAttackFinished;
	Client_SendEvent(self, EV_WEAPON_PRIMARYATTACK);
	return TRUE;
}

/*
====================
BaseGun_Reload
====================
*/
float BaseGun_Reload(void) {
	static void BaseGun_FinishReload(void) {
		int iNeed = wptTable[self.weapon].iMagSize - self.(wptTable[self.weapon].iMagfld);
		int iHave = self.(wptTable[self.weapon].iCaliberfld);

		if (iNeed > iHave) {
			self.(wptTable[self.weapon].iMagfld) += iHave;
			self.(wptTable[self.weapon].iCaliberfld) = 0;
		} else {
			self.(wptTable[self.weapon].iMagfld) += iNeed;
			self.(wptTable[self.weapon].iCaliberfld) -= iNeed;
		}

		Weapon_UpdateCurrents();
	}
	
	// Don't bother reloading the gun when full
	if (self.(wptTable[self.weapon].iMagfld) == wptTable[self.weapon].iMagSize) {
		return FALSE;
	}
	
	// Also don't bother reloading the gun if you've got nothing to reload it with
	if (self.(wptTable[self.weapon].iCaliberfld) <= 0) {
		return FALSE;
	}
	
	
	self.think = BaseGun_FinishReload;
	self.nextthink = time + wptTable[self.weapon].fReloadFinished - 0.1f; // Hack - in some cases input might happen first
	self.fAttackFinished = self.nextthink + 0.1f;
	
	Animation_ReloadWeapon(self);
	Client_SendEvent(self, EV_WEAPON_RELOAD);
	return TRUE;
}
#endif
