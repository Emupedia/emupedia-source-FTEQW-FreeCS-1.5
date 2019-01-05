/***
*
*   Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
* 
* 	See the file LICENSE attached with the sources for usage details.
*
****/

enum {
	HOSTAGE_IDLE,
	HOSTAGE_WALK,
	HOSTAGE_RUN
};

enum {
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

class hostage_entity:CBaseEntity
{
	entity m_eUser;
	int m_iUsed;
	void() hostage_entity;

	virtual void() Hide;
	virtual void() Respawn;
	virtual void() PlayerUse;
	virtual void(int) vPain;
	virtual void(int) vDeath;
	virtual void() Physics;
};

void hostage_entity::Physics(void)
{
	input_movevalues = [0,0,0];
	input_impulse = 0;
	input_buttons = 0;
	if (m_eUser!= world) {
		vector enda = vectoangles(m_eUser.origin - origin);
		enda[0] = 0;
		enda[1] = Math_FixDelta(enda[1]);
		enda[2] = 0;
		v_angle = enda;
		
		if (vlen(m_eUser.origin - origin) > 128) {
			input_movevalues[0] = 240;
		}
	}
	
	input_timelength = frametime;
	input_angles = v_angle;
	movetype = MOVETYPE_WALK;
	
	runstandardplayerphysics(this);
	Footsteps_Update();
	
	angles = v_angle;
	movetype = MOVETYPE_NONE;
}

void hostage_entity::PlayerUse(void)
{
	if (eActivator.team == TEAM_CT) {
		if ((m_eUser == world)) {
			// Only give cash to the CT for using it for the first time
			if (m_iUsed == FALSE) {
				Money_AddMoney(eActivator, 150);
				sound(this, CHAN_VOICE, sprintf("hostage/hos%d.wav", random(1, 6)), 1.0, ATTN_IDLE);
				m_iUsed = TRUE;
			}

			m_eUser = eActivator;
		} else {
			m_eUser = world;
		}
	}
}

void hostage_entity::vPain(int iHitBody)
{
	frame = HOSA_FLINCH + floor(random(0, 5));
}

void hostage_entity::vDeath(int iHitBody)
{
	Radio_BroadcastMessage(RADIO_HOSDOWN);
	frame = HOSA_DIE_SIMPLE + floor(random(0, 6));
	
	solid = SOLID_NOT;
	takedamage = DAMAGE_NO;
	customphysics = Empty;
}

void hostage_entity::Hide(void)
{
	setmodel(this, "");
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

	setorigin(this, m_oldOrigin);
	angles = v_angle;
	solid = SOLID_SLIDEBOX;
	movetype = MOVETYPE_NONE;
	setmodel(this, m_oldModel);
	setsize(this, VEC_HULL_MIN + [0,0,36], VEC_HULL_MAX + [0,0,36]);

	m_eUser = world;
	iBleeds = TRUE;
	takedamage = DAMAGE_YES;
	style = HOSTAGE_IDLE;
	customphysics = Physics;

	frame = HOSA_IDLE1;
	health = 100;
	velocity = [0,0,0];
	m_iUsed = FALSE;
}

void hostage_entity::hostage_entity(void)
{
	// Path hack, FIXME do it a better way
	if (model == "/models/hostage.mdl") {
		model = "";
	}

	if (!model) {
		model = "models/hostage.mdl";
	}

	CBaseEntity::CBaseEntity();

	precache_model(m_oldModel);
	setmodel(this, m_oldModel);
	iHostagesMax = iHostagesMax + 1;
	Respawn();
}