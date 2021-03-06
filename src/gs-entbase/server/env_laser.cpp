/*
 * Copyright (c) 2016-2020 Marco Hladik <marco@icculus.org>
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

/*QUAKED env_laser (1 0 0) (-8 -8 -8) (8 8 8)
"targetname"            Name
"target"                Target when triggered.
"killtarget"            Target to kill when triggered.

Shoots a frickin lazer.
*/

enumflags
{
	ENVLASER_CHANGED_ORIGIN,
	ENVLASER_CHANGED_ANGLES,
	ENVLASER_CHANGED_TEXTURE,
	ENVLASER_CHANGED_ENDTEXTURE,
	ENVLASER_CHANGED_STATE
};

enumflags
{
	ENVLAZ_STARTON,
	ENVLAZ_2,
	ENVLAZ_4,
	ENVLAZ_8,
	ENVLAZ_STARTSPARK,
	ENVLAZ_ENDSPARK,
	ENVLAZ_DECALEND
};

class env_laser:CBaseTrigger
{
	int m_iState;
	int m_iStateOld;
	float m_flDPS;
	string m_strLaserDest;
	string m_strBeamTex;
	string m_strEndTex;

	void(void) env_laser;

	virtual void(void) think;
	virtual void(void) Trigger;
	virtual void(void) Respawn;
	virtual void(void) ParentUpdate;
	virtual float(entity, float) SendEntity;
};

void env_laser::think(void)
{
	entity t;

	if (!m_iState) {
		return;
	} else {
		nextthink = time + 0.1;
	}

	t = (CBaseTrigger)find(world, CBaseEntity::m_strTargetName, m_strLaserDest);
	angles = t.origin;

	if (!t) {
		print(sprintf("^1env_laser::^3think^7: %s has no valid target. Aborting\n", m_strTargetName));
		return;
	}

	traceline(origin, t.origin, FALSE, this);

	if (trace_ent.takedamage) {
		Damage_Apply(trace_ent, this, rint(m_flDPS), 0, DMG_GENERIC);
	}
}

void env_laser::Trigger(void)
{
	m_iState = 1 - m_iState;
	
	if (m_iState) {
		nextthink = time;
	} else {
		nextthink = 0.25;
	}
}

void env_laser::Respawn(void)
{
	if (spawnflags & ENVLAZ_STARTON) {
		m_iState = 1;
		nextthink = time + 0.1;
	}
}

float env_laser::SendEntity(entity ePEnt, float fChanged)
{
	WriteByte(MSG_ENTITY, ENT_ENVLASER);
	WriteFloat(MSG_ENTITY, fChanged);

	if (fChanged & ENVLASER_CHANGED_ORIGIN) {
		WriteCoord(MSG_ENTITY, origin[0]);
		WriteCoord(MSG_ENTITY, origin[1]);
		WriteCoord(MSG_ENTITY, origin[2]);
	}
	if (fChanged & ENVLASER_CHANGED_ANGLES) {
		WriteCoord(MSG_ENTITY, angles[0]);
		WriteCoord(MSG_ENTITY, angles[1]);
		WriteCoord(MSG_ENTITY, angles[2]);
	}
	if (fChanged & ENVLASER_CHANGED_TEXTURE) {
		WriteString(MSG_ENTITY, m_strBeamTex);
	}
	if (fChanged & ENVLASER_CHANGED_ENDTEXTURE) {
		WriteString(MSG_ENTITY, m_strEndTex);
	}
	if (fChanged & ENVLASER_CHANGED_STATE) {
		WriteByte(MSG_ENTITY, m_iState);
	}

	return TRUE;
}

void env_laser::ParentUpdate(void)
{
	/* FIXME: Check our fields for networking */
	/*if (origin != oldnet_origin) {
		SendFlags |= ENVLASER_CHANGED_ORIGIN;
		SendFlags |= ENVLASER_CHANGED_ANGLES;
		SendFlags |= ENVLASER_CHANGED_STATE;
		SendFlags |= ENVLASER_CHANGED_TEXTURE;
		oldnet_origin = origin;
	}
	if (angles != oldnet_angles) {
		SendFlags |= ENVLASER_CHANGED_ORIGIN;
		SendFlags |= ENVLASER_CHANGED_ANGLES;
		SendFlags |= ENVLASER_CHANGED_STATE;
		SendFlags |= ENVLASER_CHANGED_TEXTURE;
		oldnet_angles = angles;
	}
	if (m_iState != m_iStateOld) {
		SendFlags |= ENVLASER_CHANGED_ORIGIN;
		SendFlags |= ENVLASER_CHANGED_ANGLES;
		SendFlags |= ENVLASER_CHANGED_STATE;
		SendFlags |= ENVLASER_CHANGED_TEXTURE;
		m_iStateOld = m_iState;
	}*/

	if (m_parent) {
		entity p = find(world, CBaseEntity::m_strTargetName, m_parent);

		if (!p) {
			return;
		}

		SetOrigin(p.origin);
	}
}

void env_laser::env_laser(void)
{
	for (int i = 1; i < (tokenize(__fullspawndata) - 1); i += 2) {
		switch (argv(i)) {
		case "texture":
			m_strBeamTex = argv(i+1);
			precache_model(m_strBeamTex);
			break;
		case "EndSprite":
			m_strEndTex = argv(i+1);
			precache_model(m_strEndTex);
			break;
		case "LaserTarget":
			m_strLaserDest = argv(i+1);
			break;
		case "damage":
			m_flDPS = stof(argv(i+1));
			break;
		default:
			break;
		}
	}

	CBaseTrigger::CBaseTrigger();
	gflags = GF_CANRESPAWN;
	pvsflags = PVSF_IGNOREPVS;
}
