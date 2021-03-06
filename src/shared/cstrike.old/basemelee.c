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

#ifdef SERVER
void BaseMelee_Draw(void) {
	self.iCurrentMag = 0;
	self.iCurrentCaliber = 0;
	
	Client_SendEvent(self, EV_WEAPON_DRAW);
}

int BaseMelee_Attack(void) {
	vector vSource;
	vector vOrigin;

	makevectors(self.v_angle);
	vSource = (self.origin + self.view_ofs);
	traceline(vSource, vSource + (v_forward * 64), MOVE_HITMODEL, self);

	if (trace_fraction == 1.0) {
		return FALSE;
	}

	vOrigin = trace_endpos - v_forward * 2;

	if (trace_ent.takedamage) {
		if (trace_ent.iBleeds == TRUE) {
			Effect_Impact(IMPACT_FLESH, trace_endpos, trace_plane_normal);
			sound(self, CHAN_WEAPON, sprintf("weapons/knife_hit%d.wav", floor((random() * 4) + 1)), 1, ATTN_NORM);
		}
		Damage_Apply(trace_ent, self, wptTable[self.weapon].iDamage, FALSE, self.weapon);
	} else {
		Effect_Impact(IMPACT_MELEE, trace_endpos, trace_plane_normal);
	}
	
	return TRUE;
}

void BaseMelee_Delayed(float fDelay) {
	static void BaseMelee_Delayed_Trigger(void) {
		BaseMelee_Attack();
	}
	
	self.think = BaseMelee_Delayed_Trigger;
	self.nextthink = time + fDelay;
}
#endif
