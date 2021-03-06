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

string sPainSounds[5] = {
	"player/pl_pain2.wav",
	"player/pl_pain4.wav",
	"player/pl_pain5.wav",
	"player/pl_pain6.wav",
	"player/pl_pain7.wav"
};

/*
=================
Player_SendEntity
=================
*/
float Player_SendEntity(entity ePEnt, float fChanged)
{
	/* If dead */
	if (self.health <= 0 && ePEnt != self) {
		return FALSE;
	}

	/* Fix CT/VIP team */
	float t1, t2;
	t1 = ePEnt.team;
	t2 = self.team;
	if (t1 == TEAM_VIP) {
		t1 = TEAM_CT;
	} else if (t2 == TEAM_VIP) {
		t2 = TEAM_CT;
	}

	/* Always make team-mates visible */
	if (t1 != t2 && ePEnt.health > 0) {
		/* Can we even see them? */
		if (!checkpvs(ePEnt.origin, self)) {
			return FALSE;
		} else {
			/* We're in the same PVS, but we might still not be able to see them */
			traceline(self.origin, ePEnt.origin, FALSE, self);
			if (trace_ent != ePEnt) {
				return FALSE;
			}
		}
	}

	WriteByte(MSG_ENTITY, ENT_PLAYER);
	WriteShort(MSG_ENTITY, self.modelindex);
	WriteCoord(MSG_ENTITY, self.origin[0]);
	WriteCoord(MSG_ENTITY, self.origin[1]);
	WriteCoord(MSG_ENTITY, self.origin[2]);
	WriteCoord(MSG_ENTITY, self.v_angle[0]);
	WriteCoord(MSG_ENTITY, self.angles[1]);
	WriteCoord(MSG_ENTITY, self.angles[2]);
	WriteCoord(MSG_ENTITY, self.velocity[0]);
	WriteCoord(MSG_ENTITY, self.velocity[1]);
	WriteCoord(MSG_ENTITY, self.velocity[2]);
	WriteFloat(MSG_ENTITY, self.flags);
	WriteFloat(MSG_ENTITY, self.pmove_flags);
	WriteByte(MSG_ENTITY, self.weapon);
	WriteByte(MSG_ENTITY, self.health);
	WriteFloat(MSG_ENTITY, self.movetype);
	WriteFloat(MSG_ENTITY, self.view_ofs[2]);
	WriteFloat(MSG_ENTITY, self.viewzoom);
	WriteFloat(MSG_ENTITY, self.jumptime);
	WriteFloat(MSG_ENTITY, self.teleport_time);
	return TRUE;
}

/*
=================
Player_Pain
=================
*/
void Player_Pain(int iHitBody)
{
	/*
	if (iHitBody == BODY_HEAD) {
		Animation_PlayerTopTemp(ANIM_HEAD_FLINCH, 0.25f);
	} else {
		Animation_PlayerTopTemp(ANIM_GUT_FLINCH, 0.25f);
	}*/
	
	sound(self, CHAN_VOICE, sPainSounds[floor(random() * 5)], 1, ATTN_IDLE);
	self.velocity = '0 0 0';
}

/*
=================
Player_Death
=================
*/
void Player_Death(int iHitBody)
{
	if (iHitBody == BODY_HEAD) {
		sound(self, CHAN_VOICE, sprintf("player/headshot%d.wav", floor((random() * 3) + 1)), 1, ATTN_NORM);
	} else {
		sound(self, CHAN_VOICE, sprintf("player/die%d.wav", floor((random() * 3) + 1)), 1, ATTN_NORM);
	}
			
	// Drop a corpse
	entity eCorpse = spawn();
	eCorpse.classname = "remove_me";
	setorigin(eCorpse, self.origin);
	setmodel(eCorpse, self.model);
	setsize(eCorpse, self.mins, self.maxs);
	eCorpse.angles = [0, self.angles[1], 0];
	eCorpse.movetype = MOVETYPE_BOUNCE;

	// Drop primary weapon as well as the bomb if present
	if (self.fSlotPrimary) {
		Weapon_DropWeapon(SLOT_PRIMARY);
	} else {
		if (self.fSlotSecondary) {
			Weapon_DropWeapon(SLOT_SECONDARY);
		}
	}
	if (self.fSlotGrenade) {
		Weapon_DropWeapon(SLOT_GRENADE);
	}
	
	// Make ourselves disappear
	self.modelindex = 0;
	
	if (self.flags & FL_CROUCHING) {
		eCorpse.frame = ANIM_CROUCH_DIE;
	} else {
		switch (iHitBody) {
			case BODY_HEAD:
				eCorpse.frame = ANIM_DIE_HEAD;
				break;
			case BODY_STOMACH:
				eCorpse.frame = ANIM_DIE_GUT;
				break;
			case BODY_LEGLEFT:
			case BODY_ARMLEFT:
				eCorpse.frame = ANIM_DIE_LEFT;
				break;
			case BODY_LEGRIGHT:
			case BODY_ARMRIGHT:
				eCorpse.frame = ANIM_DIE_RIGHT;
				break;
			default:
				eCorpse.frame = ANIM_DEATH1 + floor(random() * 3);
				break;
		}
	}

	PlayerMakeSpectator();
	self.classname = "player";
	self.health = 0;
	forceinfokey(self, "*dead", "1"); 
	forceinfokey(self, "*team", ftos(self.team));

	Rules_CountPlayers();

	/* In Assassination, all Terrorists receive a $2500
	 *  reward if they won by killing the VIP. */
	if (self.team == TEAM_VIP) {
		Rules_RoundOver(TEAM_T, 2500, FALSE);
		return;
	}

	Rules_DeathCheck();
}

/*
====================
UseWorkaround
====================
*/
void UseWorkaround(entity eTarget)
{
	eActivator = self;
	entity eOldSelf = self;
	self = eTarget;
	self.PlayerUse();
	self = eOldSelf;
}

/*
====================
Player_UseDown
====================
*/
void Player_UseDown(void)
{
    if (self.health <= 0) {
		return;
	} else if (!(self.gflags & GF_USE_RELEASED)) {
		return;
	}

	vector vSource;

	makevectors(self.v_angle);
	vSource = self.origin + self.view_ofs;
	traceline (vSource, vSource + (v_forward * 64), FALSE, self);

	if (trace_ent.PlayerUse) {
		self.gflags &= ~GF_USE_RELEASED;

		UseWorkaround(trace_ent);

		/* Some entities want to support Use spamming */
		if (!(self.gflags & GF_USE_RELEASED)) {
			sound(self, CHAN_ITEM, "common/wpn_select.wav", 0.25, ATTN_IDLE);
		}
	} else {
		sound(self, CHAN_ITEM, "common/wpn_denyselect.wav", 0.25, ATTN_IDLE);
		self.gflags &= ~GF_USE_RELEASED;
	}
}

/*
====================
Player_UseUp
====================
*/
void Player_UseUp(void)
{
	if (!(self.gflags & GF_USE_RELEASED)) {
		self.gflags |= GF_USE_RELEASED;
		self.fProgressBar = 0;
	}
}

/*
=================
PlayerPreThink

Run before physics
=================
*/
void Game_PlayerPreThink(void)
{ 
	BaseGun_ShotMultiplierUpdate();
}

/*
=================
PlayerPreThink

Run after physics
=================
*/
void Game_PlayerPostThink(void)
{
	Animation_PlayerUpdate();
	Footsteps_Update();
	
	self.SendFlags = 1;
}
