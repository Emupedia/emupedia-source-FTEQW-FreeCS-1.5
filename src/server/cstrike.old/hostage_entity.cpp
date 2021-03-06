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

enum
{
	HOSTAGE_IDLE,
	HOSTAGE_WALK,
	HOSTAGE_RUN
};

enum
{
	HOSA_WALK,
	HOSA_WALKSCARED,
	HOSA_RUN,
	HOSA_RUNSCARED,
	HOSA_RUNLOOK,
	HOSA_180LEFT,
	HOSA_180RIGHT,
	HOSA_FLINCH,
	HOSA_PAIN,
	HOSA_PAINLEFT,
	HOSA_PAINRIGHT,
	HOSA_PAINLEGLEFT,
	HOSA_PAINLEGRIGHT,
	HOSA_IDLE1,
	HOSA_IDLE2,
	HOSA_IDLE3,
	HOSA_IDLE4,
	HOSA_IDLE5,
	HOSA_IDLE6,
	HOSA_SCARED_END,
	HOSA_SCARED1,
	HOSA_SCARED2,
	HOSA_SCARED3,
	HOSA_SCARED4,
	HOSA_PANIC,
	HOSA_FEAR1,
	HOSA_FEAR2,
	HOSA_CRY,
	HOSA_SCI1,
	HOSA_SCI2,
	HOSA_SCI3,
	HOSA_DIE_SIMPLE,
	HOSA_DIE_FORWARD1,
	HOSA_DIE_FORWARD2,
	HOSA_DIE_BACKWARD,
	HOSA_DIE_HEADSHOT,
	HOSA_DIE_GUTSHOT,
	HOSA_LYING1,
	HOSA_LYING2,
	HOSA_DEADSIT,
	HOSA_DEADTABLE1,
	HOSA_DEADTABLE2,
	HOSA_DEADTABLE3
};

string g_hostsnd[] = {
	"hostage/hos1.wav",
	"hostage/hos2.wav",
	"hostage/hos3.wav",
	"hostage/hos4.wav",
	"hostage/hos5.wav"
};

class hostage_entity:CBaseEntity
{
	vector m_vecLastUserPos;
	entity m_eUser;
	entity m_eRescuer;
	int m_iUsed;
	int m_iScared;
	void(void) hostage_entity;

	virtual void(void) touch;
	virtual void(void) Hide;
	virtual void(void) Respawn;
	virtual void(void) PlayerUse;
	virtual void(int) Pain;
	virtual void(int) Death;
	virtual void(void) Physics;
};

void hostage_entity::Physics(void)
{
	float spvel;
	input_movevalues = [0,0,0];
	input_impulse = 0;
	input_buttons = 0;
	
	/* Deal with a hostage being rescued when it's following someone else */
	if (m_eRescuer.classname == "hostage_entity") {
		if (m_eRescuer.solid == SOLID_NOT) {
			m_eRescuer = m_eUser;
		}
	}
	/* Deal with the hostage losing its rescuer (death) */
	if (m_eUser.health <= 0) {
		m_eUser = world;
	}

	if (m_eUser!= world) {
		v_angle = vectoangles(m_eRescuer.origin - origin);
		v_angle[0] = 0;
		v_angle[1] = Math_FixDelta(v_angle[1]);
		v_angle[2] = 0;

		/* Give up after 1024 units */
		if (vlen(m_eRescuer.origin - origin) > 1024) {
			m_eUser = world;
		} else if (vlen(m_eRescuer.origin - origin) > 64) {
			input_movevalues[0] = 240;

			other = world;
			traceline(origin, /*mins, maxs, */m_eRescuer.origin, MOVE_OTHERONLY, this);

			/* Tracing failed, there's world geometry in the way */
			if (trace_fraction < 1.0f) {
				v_angle = vectoangles(m_vecLastUserPos - origin);
				v_angle[0] = 0;
				v_angle[1] = Math_FixDelta(v_angle[1]);
				v_angle[2] = 0;
			} else {
				m_vecLastUserPos = m_eRescuer.origin;
			}

			/* Trace again to see if another hostage is in our path and if so
			 * follow them instead, this makes pathing easier */
			traceline(origin, /*mins, maxs,*/ m_vecLastUserPos, FALSE, this);
			if (trace_ent.classname == "hostage_entity") {
				hostage_entity que = (hostage_entity)trace_ent;
				if (que.m_eRescuer == m_eUser) {
					if (trace_ent != this) {
						m_eRescuer = trace_ent;
					}
				}
			}
		}
	}

	spvel = vlen(velocity);

	if (spvel < 5) {
		frame = m_iScared ? HOSA_SCARED1:HOSA_IDLE1;
	} else if (spvel <= 140) {
		frame = m_iScared ? HOSA_WALKSCARED:HOSA_WALK;
	} else if (spvel <= 240) {
		frame = m_iScared ? HOSA_RUNSCARED:HOSA_RUN;
	}

	input_angles = angles = v_angle;
	input_timelength = frametime;
	movetype = MOVETYPE_WALK;

	runstandardplayerphysics(this);
	Footsteps_Update();

	movetype = MOVETYPE_NONE;
}

void hostage_entity::touch(void)
{
	if (other.team != TEAM_T) {
		velocity = normalize(other.origin - origin) * -128;
	}
}

void hostage_entity::PlayerUse(void)
{
	if (eActivator.team == TEAM_CT) {
		if (m_eUser != world) {
			m_eUser = world;
			return;
		}

		/* only give cash to the CT for using it for the first time */
		if (m_iUsed == FALSE) {
			int r = floor(random(0,5));
			sound(this, CHAN_VOICE, g_hostsnd[r], 1.0, ATTN_IDLE);
			Money_AddMoney(eActivator, 150);
			m_iUsed = TRUE;
		}

		m_eUser = eActivator;
		m_eRescuer = m_eUser;
		m_vecLastUserPos = m_eUser.origin;
	}
}

void hostage_entity::Pain(int iHitBody)
{
	frame = HOSA_FLINCH + floor(random(0, 5));
	//m_iScared = TRUE;
}

void hostage_entity::Death(int iHitBody)
{
	solid = SOLID_NOT;
	takedamage = DAMAGE_NO;
	customphysics = Empty;

	Radio_BroadcastMessage(RADIO_HOSDOWN);
	frame = HOSA_DIE_SIMPLE + floor(random(0, 6));
}

void hostage_entity::Hide(void)
{
	SetModel("");
	m_eUser = world;
	solid = SOLID_NOT;
	movetype = MOVETYPE_NONE;
	customphysics = __NULL__;
}

void hostage_entity::Respawn(void)
{
	v_angle[0] = Math_FixDelta(m_oldAngle[0]);
	v_angle[1] = Math_FixDelta(m_oldAngle[1]);
	v_angle[2] = Math_FixDelta(m_oldAngle[2]);

	SetOrigin(m_oldOrigin);
	angles = v_angle;
	solid = SOLID_SLIDEBOX;
	movetype = MOVETYPE_NONE;
	SetModel(m_oldModel);
	setsize(this, VEC_HULL_MIN + [0,0,36], VEC_HULL_MAX + [0,0,36]);

	m_eUser = world;
	iBleeds = TRUE;
	takedamage = DAMAGE_YES;
	style = HOSTAGE_IDLE;
	customphysics = Physics;

	frame = HOSA_IDLE1;
	health = 100;
	velocity = [0,0,0];
	m_iUsed = m_iScared = FALSE;
}

void hostage_entity::hostage_entity(void)
{
	for (int i = 0; i < g_hostsnd.length; i++) {
		precache_sound(g_hostsnd[i]);
	}

	/* Path hack, FIXME do it a better way */
	if (model == "/models/hostage.mdl") {
		model = "";
	}

	if (!model) {
		model = "models/hostage.mdl";
	}

	CBaseEntity::CBaseEntity();

	precache_model(m_oldModel);
	SetModel(m_oldModel);
	iHostagesMax = iHostagesMax + 1;
	Respawn();
}
