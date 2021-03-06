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

class CBaseEntity
{
	string m_strTarget;
	string m_strTargetName;
	int m_iBody;

	/* respawn */
	string m_oldModel;
	float m_oldSolid;
	float m_oldHealth;
	vector m_oldOrigin;
	vector m_oldAngle;

	/* keep track of these variables */
	vector net_origin;
	vector net_angles;

#ifdef GS_RENDERFX
	int m_iRenderFX;
	float m_iRenderMode;
	float m_flRenderAmt;
	vector m_vecRenderColor;

	/* respawn */
	int m_oldiRenderFX;
	float m_oldiRenderMode;
	float m_oldflRenderAmt;
	vector m_oldvecRenderColor;
#endif

	string m_parent;
	
	void(void) CBaseEntity;
	virtual void(void) Respawn;
	virtual void(void) Hide;
	virtual void(void) ParentUpdate;
	virtual float(entity, float) SendEntity;
	virtual void(int iHitBody) Pain;
	virtual void(int iHitBody) Death;

	virtual void(float) SetEffects;
	virtual void(float) SetFrame;
	virtual void(string) SetModel;
	virtual void(float) SetModelindex;
	virtual void(float) SetMovetype;
	virtual void(float) SetSkin;
	virtual void(float) SetSolid;
	virtual void(int) SetBody;
	virtual void(vector) SetAngles;
	virtual void(vector) SetOrigin;
	virtual void(vector, vector) SetSize;

#ifdef GS_RENDERFX
	virtual void(int) SetRenderFX;
	virtual void(float) SetRenderMode;
	virtual void(float) SetRenderAmt;
	virtual void(vector) SetRenderColor;
#endif
};

/* we want to really use those set functions because they'll notify of any
 * networking related changes. otherwise we'll have to keep track of copies
 * that get updated every frame */
void
CBaseEntity::SetEffects(float newEffects)
{
	if (newEffects == effects)
		return;

	effects = newEffects;
	SendFlags |= BASEFL_CHANGED_EFFECTS;
}
void
CBaseEntity::SetFrame(float newFrame)
{
	if (newFrame == frame)
		return;

	frame = newFrame;
	SendFlags |= BASEFL_CHANGED_FRAME;
}
void
CBaseEntity::SetModel(string newModel)
{
	model = newModel;
	setmodel(this, newModel);
	SendFlags |= BASEFL_CHANGED_MODELINDEX;
}
void
CBaseEntity::SetModelindex(float newModelIndex)
{
	if (newModelIndex == modelindex)
		return;

	modelindex = newModelIndex;
	SendFlags |= BASEFL_CHANGED_MODELINDEX;
}
void
CBaseEntity::SetMovetype(float newMovetype)
{
	if (newMovetype == movetype)
		return;

	movetype = newMovetype;
	SendFlags |= BASEFL_CHANGED_MOVETYPE;
}
void
CBaseEntity::SetSkin(float newSkin)
{
	if (newSkin == skin)
		return;

	skin = newSkin;
	SendFlags |= BASEFL_CHANGED_SKIN;
}
void
CBaseEntity::SetSolid(float newSolid)
{
	if (newSolid == solid)
		return;

	solid = newSolid;
	SendFlags |= BASEFL_CHANGED_SOLID;
}
void
CBaseEntity::SetBody(int newBody)
{
	if (newBody == m_iBody)
		return;

	m_iBody = newBody;
	SendFlags |= BASEFL_CHANGED_BODY;
}
void
CBaseEntity::SetAngles(vector newAngles)
{
	if (newAngles == angles)
		return;

	angles = newAngles;
	SendFlags |= BASEFL_CHANGED_ANGLES;
}
void
CBaseEntity::SetSize(vector newMins, vector newMaxs)
{
	if (newMins == mins && newMaxs == maxs)
		return;

	setsize(this, newMins, newMaxs);
	SendFlags |= BASEFL_CHANGED_SIZE;
}
void
CBaseEntity::SetOrigin(vector newOrigin)
{
	if (newOrigin == origin)
		return;

	setorigin(this, newOrigin);
	SendFlags |= BASEFL_CHANGED_ORIGIN;
}

#ifdef GS_RENDERFX
void
CBaseEntity::SetRenderFX(int newFX)
{
	if (newFX == m_iRenderFX)
		return;

	m_iRenderFX = newFX;
	SendFlags |= BASEFL_CHANGED_RENDERFX;
}
void
CBaseEntity::SetRenderMode(float newMode)
{
	if (newMode == m_iRenderMode)
		return;

	m_iRenderMode = newMode;
	SendFlags |= BASEFL_CHANGED_RENDERMODE;
}
void
CBaseEntity::SetRenderAmt(float newAmt)
{
	if (newAmt == m_flRenderAmt)
		return;

	m_flRenderAmt = newAmt;
	SendFlags |= BASEFL_CHANGED_RENDERAMT;
}
void
CBaseEntity::SetRenderColor(vector newColor)
{
	if (newColor == m_vecRenderColor)
		return;

	m_vecRenderColor = newColor;
	SendFlags |= BASEFL_CHANGED_RENDERCOLOR;
}
#endif

/* Make sure StartFrame calls this */
float
CBaseEntity::SendEntity(entity ePEnt, float fChanged)
{
	if (!modelindex) {
		return FALSE;
	}

	WriteByte(MSG_ENTITY, ENT_ENTITY);
	WriteFloat(MSG_ENTITY, fChanged);

	/* really trying to get our moneys worth with 23 bits of mantissa */
	if (fChanged & BASEFL_CHANGED_ORIGIN) {
		WriteCoord(MSG_ENTITY, origin[0]);
		WriteCoord(MSG_ENTITY, origin[1]);
		WriteCoord(MSG_ENTITY, origin[2]);
	}
	if (fChanged & BASEFL_CHANGED_ANGLES) {
		WriteFloat(MSG_ENTITY, angles[0]);
		WriteFloat(MSG_ENTITY, angles[1]);
		WriteFloat(MSG_ENTITY, angles[2]);
	}
	if (fChanged & BASEFL_CHANGED_MODELINDEX) {
		WriteShort(MSG_ENTITY, modelindex);
	}
	if (fChanged & BASEFL_CHANGED_SOLID) {
		WriteByte(MSG_ENTITY, solid);
	}
	if (fChanged & BASEFL_CHANGED_MOVETYPE) {
		WriteByte(MSG_ENTITY, movetype);
	}
	if (fChanged & BASEFL_CHANGED_SIZE) {
		WriteCoord(MSG_ENTITY, mins[0]);
		WriteCoord(MSG_ENTITY, mins[1]);
		WriteCoord(MSG_ENTITY, mins[2]);
		WriteCoord(MSG_ENTITY, maxs[0]);
		WriteCoord(MSG_ENTITY, maxs[1]);
		WriteCoord(MSG_ENTITY, maxs[2]);
	}
	if (fChanged & BASEFL_CHANGED_FRAME) {
		WriteByte(MSG_ENTITY, frame);
	}
	if (fChanged & BASEFL_CHANGED_SKIN) {
		WriteByte(MSG_ENTITY, skin + 128);
	}
	if (fChanged & BASEFL_CHANGED_EFFECTS) {
		WriteFloat(MSG_ENTITY, effects);
	}
	if (fChanged & BASEFL_CHANGED_BODY) {
		WriteByte(MSG_ENTITY, m_iBody);
	}

#ifdef GS_RENDERFX
	if (fChanged & BASEFL_CHANGED_RENDERFX) {
		WriteByte(MSG_ENTITY, m_iRenderFX);
	}
	if (fChanged & BASEFL_CHANGED_RENDERMODE) {
		WriteByte(MSG_ENTITY, m_iRenderMode);
	}
	if (fChanged & BASEFL_CHANGED_RENDERCOLOR) {
		WriteFloat(MSG_ENTITY, m_vecRenderColor[0]);
		WriteFloat(MSG_ENTITY, m_vecRenderColor[1]);
		WriteFloat(MSG_ENTITY, m_vecRenderColor[2]);
	}
	if (fChanged & BASEFL_CHANGED_RENDERAMT) {
		WriteFloat(MSG_ENTITY, m_flRenderAmt);
	}
#else
	if (fChanged & BASEFL_CHANGED_ALPHA) {
		WriteFloat(MSG_ENTITY, alpha);
	}
#endif

	return TRUE;
}

void
CBaseEntity::Pain(int body)
{
	
}

void
CBaseEntity::Death(int body)
{
	
}

/* Make sure StartFrame calls this */
void
CBaseEntity::ParentUpdate(void)
{
	/* while the engine is still handling physics for these, we can't
	 * predict when origin/angle might change */
	if (net_origin != origin) {
		net_origin = origin;
		SendFlags |= BASEFL_CHANGED_ORIGIN;
	}
	if (net_angles != angles) {
		net_angles = angles;
		SendFlags |= BASEFL_CHANGED_ANGLES;
	}

	if (m_parent) {
		entity p = find(world, CBaseEntity::m_strTargetName, m_parent);

		if (!p) {
			return;
		}

		SetOrigin(p.origin);
	}
}

void
CBaseEntity::CBaseEntity(void)
{
	/* Not in Deathmatch */
	if (spawnflags & 2048) {
		if (cvar("sv_playerslots") > 1) {
			remove(this);
			return;
		}
	}

	gflags = GF_CANRESPAWN;
	effects |= EF_NOSHADOW;

	int nfields = tokenize(__fullspawndata);
	for (int i = 1; i < (nfields - 1); i += 2) {
		switch (argv(i)) {
		case "origin":
			origin = stov(argv(i+1));
			break;
		case "angles":
			angles = stov(argv(i+1));
			break;
		case "solid":
			solid = stof(argv(i+1));
			break;
		case "health":
			health = stof(argv(i+1));
			break;
		case "shadows":
			if (stof(argv(i+1)) == 1) {
				effects &= ~EF_NOSHADOW;
			}
			break;
		case "targetname":
			m_strTargetName = argv(i+1);
			targetname = __NULL__;
			break;
		case "target":
			m_strTarget = argv(i+1);
			target = __NULL__;
			break;
		case "color":
			m_vecRenderColor = stov(argv(i+1));
			break;
		case "alpha":
			m_flRenderAmt = stof(argv(i+1));
			break;
		case "renderamt":
			m_flRenderAmt = stof(argv(i+1)) / 255;
			break;
		case "rendercolor":
			m_vecRenderColor = stov(argv(i+1)) / 255;
			break;
		case "rendermode":
			m_iRenderMode = stoi(argv(i+1));
			break;
		case "renderfx":
			m_iRenderFX = stoi(argv(i+1));
			break;
		case "parentname":
			m_parent = argv(i+1);
			break;
		case "model":
			model = argv(i+1);
			break;
		default:
			break;
		}
	}

	m_oldAngle = angles;
	m_oldOrigin = origin;
	m_oldSolid = solid;
	m_oldHealth = health;
	m_oldModel = Util_FixModel(model);
	m_oldiRenderFX = m_iRenderFX;
	m_oldiRenderMode = m_iRenderMode;
	m_oldvecRenderColor = m_vecRenderColor;
	m_oldflRenderAmt = m_flRenderAmt;
	m_oldvecRenderColor = m_vecRenderColor;
	m_oldflRenderAmt = m_flRenderAmt;

	if (m_oldModel != "") {
		precache_model(m_oldModel);
	}
}

void
CBaseEntity::Respawn(void)
{
	health = m_oldHealth;
	SetModel(m_oldModel);
	SetSolid(m_oldSolid);
	SetAngles(m_oldAngle);
	SetOrigin(m_oldOrigin);

#ifdef GS_RENDERFX
	SetRenderFX(m_oldiRenderFX);
	SetRenderMode(m_oldiRenderMode);
	SetRenderAmt(m_oldflRenderAmt);
	SetRenderColor(m_oldvecRenderColor);
#endif
}

void
CBaseEntity::Hide(void)
{
	SetModelindex(0);
	SetSolid(SOLID_NOT);
	SetMovetype(MOVETYPE_NONE);
	takedamage = DAMAGE_NO;
}
