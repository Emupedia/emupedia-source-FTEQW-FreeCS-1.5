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

#define SF_GUNTARGET_ON 1

class func_guntarget:CBaseTrigger
{
	float m_flSpeed;

	void() func_guntarget;
	
	virtual void() Respawn;
	virtual void() NextPath;
	virtual void() Move;
	virtual void() Stop;
	virtual void() Trigger;
	virtual void(int) vDeath;
};

void func_guntarget::Move(void)
{
	float flTravelTime;
	vector vel_to_pos;
	entity f;

	f = find(world, CBaseTrigger::m_strTargetName, m_strTarget);

	if (!f) {
		print("^1func_guntarget^7: Path node not found!\n");
		return;
	}

	vector vecWorldPos;
	vecWorldPos[0] = absmin[0] + (0.5 * (absmax[0] - absmin[0]));
	vecWorldPos[1] = absmin[1] + (0.5 * (absmax[1] - absmin[1]));
	vecWorldPos[2] = absmin[2] + (0.5 * (absmax[2] - absmin[2]));

	vel_to_pos = (f.origin - vecWorldPos);
	flTravelTime = (vlen(vel_to_pos) / m_flSpeed);

	if (!flTravelTime) {
		NextPath();
		return;
	}

	velocity = (vel_to_pos * (1 / flTravelTime));
	think = NextPath;
	nextthink = (ltime + flTravelTime);
}

void func_guntarget::NextPath(void)
{
	CBaseTrigger current_target;

	print(sprintf("^2func_guntarget^7: Talking to current target %s... ", m_strTarget));
	current_target = (CBaseTrigger)find(world, CBaseTrigger::m_strTargetName, m_strTarget);

	if (!current_target) {
		print("^1FAILED.\n");
	} else {
		print("^2SUCCESS.\n");
	}

	m_strTarget = current_target.m_strTarget;
	velocity = [0,0,0];

	if (m_strTarget) {
		Move();
	}
}

void func_guntarget::vDeath(int iHitBody)
{
	Stop();
}

void func_guntarget::Stop(void)
{
	takedamage = DAMAGE_NO;
	velocity = [0,0,0];
	nextthink = 0;
	think = __NULL__;
}

void func_guntarget::Trigger(void)
{
	flags = (1<<FL_FROZEN) | flags;

	if (flags & FL_FROZEN) {
		takedamage = DAMAGE_NO;
		Stop();
	} else {
		takedamage = DAMAGE_YES;
		NextPath();
	}
}

void func_guntarget::Respawn(void)
{
	solid = SOLID_BSP;
	movetype = MOVETYPE_PUSH;

	setmodel(this, m_oldModel);
	setorigin(this, m_oldOrigin);

	if (spawnflags & SF_GUNTARGET_ON) {
		think = Trigger;
		nextthink = time + 0.1f;
	}
}

void func_guntarget::func_guntarget(void)
{
	for (int i = 1; i < (tokenize(__fullspawndata) - 1); i += 2) {
		switch (argv(i)) {
		case "health":
			health = stof(argv(i+1));
			break;
		case "speed":
			m_flSpeed = stof(argv(i+1));
			break;
		default:
			break;
		}
	}

	if (!m_flSpeed) {
		m_flSpeed = 100;
	}

	CBaseTrigger::CBaseTrigger();
	Respawn();
}