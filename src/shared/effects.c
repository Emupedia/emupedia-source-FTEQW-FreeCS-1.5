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

#ifdef CLIENT

string g_hgibs[] = {
	"models/gib_b_bone.mdl",
	"models/gib_legbone.mdl",
	"models/gib_lung.mdl",
	"models/gib_skull.mdl",
	"models/gib_b_gib.mdl"
};

void
Effects_Init(void)
{
	precache_model("sprites/fexplo.spr");
	precache_model("sprites/bloodspray.spr");
	precache_model("sprites/blood.spr");
	precache_model("models/glassgibs.mdl");
	precache_model("models/woodgibs.mdl");
	precache_model("models/metalplategibs.mdl");
	precache_model("models/fleshgibs.mdl");
	precache_model("models/ceilinggibs.mdl");
	precache_model("models/computergibs.mdl");
	precache_model("models/rockgibs.mdl");
	precache_model("models/cindergibs.mdl");

	precache_sound("debris/bustglass1.wav");
	precache_sound("debris/bustglass2.wav");
	precache_sound("debris/bustglass3.wav");
	precache_sound("debris/bustcrate1.wav");
	precache_sound("debris/bustcrate2.wav");
	precache_sound("debris/bustcrate3.wav");
	precache_sound("debris/bustmetal1.wav");
	precache_sound("debris/bustmetal2.wav");
	precache_sound("debris/bustflesh1.wav");
	precache_sound("debris/bustflesh2.wav");
	precache_sound("debris/bustconcrete1.wav");
	precache_sound("debris/bustconcrete2.wav");
	precache_sound("debris/bustceiling.wav");
	precache_sound("weapons/explode3.wav");
	precache_sound("weapons/explode4.wav");
	precache_sound("weapons/explode5.wav");
	precache_sound("buttons/spark1.wav");
	precache_sound("buttons/spark2.wav");
	precache_sound("buttons/spark3.wav");
	precache_sound("buttons/spark4.wav");
	precache_sound("buttons/spark5.wav");
	precache_sound("buttons/spark6.wav");
	precache_sound("weapons/ric1.wav");
	precache_sound("weapons/ric2.wav");
	precache_sound("weapons/ric3.wav");
	precache_sound("weapons/ric4.wav");
	precache_sound("weapons/ric5.wav");
	precache_sound("common/bodysplat.wav");

	for (int i = 0; i < g_hgibs.length; i++) {
		precache_model(g_hgibs[i]);
	}

	// Half-Life only has generic ric1-5
#ifdef OLD_CSTRIKE
	precache_sound("weapons/ric_metal-1.wav");
	precache_sound("weapons/ric_metal-2.wav");
	precache_sound("weapons/ric_conc-1.wav");
	precache_sound("weapons/ric_conc-2.wav");
	precache_sound("weapons/knife_hitwall1.wav");
#endif
}
#endif


void
Effect_GibHuman(vector pos)
{
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_GIBHUMAN);
	WriteCoord(MSG_MULTICAST, pos[0]); 
	WriteCoord(MSG_MULTICAST, pos[1]); 
	WriteCoord(MSG_MULTICAST, pos[2]);
	msg_entity = __NULL__;
	multicast(pos, MULTICAST_PVS);
#else
	static void Gib_Remove(void) {
		remove(self);
	}
	static void Gib_Touch(void)
	{
		Decals_Place(self.origin, sprintf("{blood%d", floor(random(1,9))));
	}
	for (int i = 0; i < 5; i++) {
		
		vector vel;
		vel[0] = random(-128,128);
		vel[1] = random(-128,128);
		vel[2] = (300 + random() * 64);
	
		entity gibb = spawn();
		setmodel(gibb, g_hgibs[i]);
		setorigin(gibb, pos);
		gibb.movetype = MOVETYPE_BOUNCE;
		gibb.solid = SOLID_BBOX;
		setsize(gibb, [0,0,0], [0,0,0]);
		gibb.velocity = vel;
		gibb.avelocity = vectoangles(gibb.velocity);
		gibb.think = Gib_Remove;
		gibb.touch = Gib_Touch;
		gibb.nextthink = time + 5.0f;
		gibb.drawmask = MASK_ENGINE;
	}
	pointsound(pos, "common/bodysplat.wav", 1, ATTN_NORM);
#endif
}

void
Effect_CreateExplosion(vector vecPos)
{
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_EXPLOSION);
	WriteCoord(MSG_MULTICAST, vecPos[0]); 
	WriteCoord(MSG_MULTICAST, vecPos[1]); 
	WriteCoord(MSG_MULTICAST, vecPos[2]);
	msg_entity = self;
	multicast(vecPos, MULTICAST_PVS);
#else
	Decals_Place(vecPos, sprintf("{scorch%d", floor(random(1,4))));
	vecPos[2] += 48;
	env_sprite eExplosion = spawn(env_sprite);
	setorigin(eExplosion, vecPos);
	setmodel(eExplosion, "sprites/fexplo.spr");
	sound(eExplosion, CHAN_WEAPON, sprintf("weapons/explode%d.wav", floor(random() * 3) + 3), 1, ATTN_NORM);

	//eExplosion.think = Effect_CreateExplosion_Animate;
	eExplosion.effects = EF_ADDITIVE;
	eExplosion.drawmask = MASK_ENGINE;
	eExplosion.maxframe = modelframecount(eExplosion.modelindex);
	eExplosion.loops = 0;
	eExplosion.framerate = 20;
	eExplosion.nextthink = time + 0.05f;

	te_explosion(vecPos);
#endif
}

void Effect_CreateBlood(vector pos, vector color) {
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_BLOOD);
	WriteCoord(MSG_MULTICAST, pos[0]); 
	WriteCoord(MSG_MULTICAST, pos[1]); 
	WriteCoord(MSG_MULTICAST, pos[2]);
	WriteByte(MSG_MULTICAST, color[0] * 255); 
	WriteByte(MSG_MULTICAST, color[1] * 255); 
	WriteByte(MSG_MULTICAST, color[2] * 255);
	msg_entity = self;
	multicast(pos, MULTICAST_PVS);
#else
	static void Blood_Touch(void)
	{
		Decals_Place(self.origin, sprintf("{blood%d", floor(random(1,9))));
		self.touch = __NULL__;
	}

	env_sprite eBlood = spawn(env_sprite);
	setorigin(eBlood, pos);
	setmodel(eBlood, "sprites/bloodspray.spr");

	//eExplosion.think = Effect_CreateExplosion_Animate;
	//eBlood.effects = EF_ADDITIVE;
	eBlood.drawmask = MASK_ENGINE;
	eBlood.maxframe = modelframecount(eBlood.modelindex);
	eBlood.loops = 0;
	eBlood.scale = 1.0f;
	eBlood.colormod = color;
	eBlood.framerate = 20;
	eBlood.nextthink = time + 0.05f;
	
	for (int i = 0; i < 3; i++) {
		env_sprite ePart = spawn(env_sprite);
		setorigin(ePart, pos);
		setmodel(ePart, "sprites/blood.spr");
		ePart.movetype = MOVETYPE_BOUNCE;
		ePart.gravity = 0.5f;
		ePart.scale = 0.5f;
		ePart.drawmask = MASK_ENGINE;
		ePart.maxframe = modelframecount(ePart.modelindex);
		ePart.loops = 0;
		ePart.colormod = color;
		ePart.framerate = 15;
		ePart.nextthink = time + 0.1f;
		ePart.velocity = randomvec() * 64;
		ePart.touch = Blood_Touch;
		ePart.solid = SOLID_BBOX;
		setsize(ePart, [0,0,0], [0,0,0]);
	}
#endif
}

void Effect_CreateSpark(vector vecPos, vector vAngle) {
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_SPARK);
	WriteCoord(MSG_MULTICAST, vecPos[0]); 
	WriteCoord(MSG_MULTICAST, vecPos[1]); 
	WriteCoord(MSG_MULTICAST, vecPos[2]);
	WriteCoord(MSG_MULTICAST, vAngle[0]); 
	WriteCoord(MSG_MULTICAST, vAngle[1]); 
	WriteCoord(MSG_MULTICAST, vAngle[2]);
	msg_entity = self;
	multicast(vecPos, MULTICAST_PVS);
#else
	pointparticles(PARTICLE_SPARK, vecPos, vAngle, 1);
	pointsound(vecPos, sprintf("buttons/spark%d.wav", floor(random() * 6) + 1), 1, ATTN_STATIC);
#endif
}

#ifdef OLD_CSTRIKE
#ifdef SERVER
void Effect_CreateFlash(entity eTarget) {
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_FLASH);
	msg_entity = eTarget;
	multicast([0,0,0], MULTICAST_ONE);
}
#endif
#endif

#ifdef OLD_CSTRIKE
void Effect_CreateSmoke(vector vecPos) {
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_SMOKE);
	WriteCoord(MSG_MULTICAST, vecPos[0]); 
	WriteCoord(MSG_MULTICAST, vecPos[1]); 
	WriteCoord(MSG_MULTICAST, vecPos[2]);
	msg_entity = self;
	multicast([0,0,0], MULTICAST_ALL);
#else
	static void Effect_CreateSmoke_Think(void) {
		// HACK: This should only ever happen when rounds restart!
		// Any way this can go wrong?
		if (self.skin < getstatf(STAT_GAMETIME)) {
			remove(self);
		}
		if (self.frame <= 0) {
			remove(self);
			return;
		} 
	
		pointparticles(PARTICLE_SMOKEGRENADE, self.origin, [0,0,0], 1);
		self.frame--;
		self.nextthink = time + 0.2f;
		self.skin = getstatf(STAT_GAMETIME);
	}
	
	entity eSmoke = spawn();
	setorigin(eSmoke, vecPos);
	eSmoke.think = Effect_CreateSmoke_Think;
	eSmoke.nextthink = time;
	eSmoke.frame = 200;
	eSmoke.skin = getstatf(STAT_GAMETIME);
#endif
}
#endif

void Effect_Impact(int iType, vector vecPos, vector vNormal) {
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_IMPACT);
	WriteByte(MSG_MULTICAST, (float)iType);
	WriteCoord(MSG_MULTICAST, vecPos[0]); 
	WriteCoord(MSG_MULTICAST, vecPos[1]); 
	WriteCoord(MSG_MULTICAST, vecPos[2]);
	WriteCoord(MSG_MULTICAST, vNormal[0]); 
	WriteCoord(MSG_MULTICAST, vNormal[1]); 
	WriteCoord(MSG_MULTICAST, vNormal[2]);
	msg_entity = self;
	multicast(vecPos, MULTICAST_PVS);
#else
	/* decals */
	switch (iType) {
	case IMPACT_GLASS:
		Decals_Place(vecPos, sprintf("{break%d", floor(random(1,4))));
		break;
	case IMPACT_MELEE:
		Decals_Place(vecPos, sprintf("{shot%d", floor(random(1,6))));
		break;
	default:
		Decals_Place(vecPos, sprintf("{bigshot%d", floor(random(1,6))));
		break;
	}

	switch (iType) {
		case IMPACT_MELEE:
			pointsound(vecPos, "weapons/knife_hitwall1.wav", 1, ATTN_STATIC);
			break;
		case IMPACT_EXPLOSION:
			break;
		case IMPACT_GLASS:
			pointparticles(PARTICLE_PIECES_BLACK, vecPos, vNormal, 1);
			break;
		case IMPACT_WOOD:
			pointparticles(PARTICLE_SPARK, vecPos, vNormal, 1);
			pointparticles(PARTICLE_PIECES_BLACK, vecPos, vNormal, 1);
			pointparticles(PARTICLE_SMOKE_BROWN, vecPos, vNormal, 1);
			break;
		case IMPACT_METAL:
			pointparticles(PARTICLE_SPARK, vecPos, vNormal, 1);
			pointparticles(PARTICLE_SPARK, vecPos, vNormal, 1);
			pointparticles(PARTICLE_PIECES_BLACK, vecPos, vNormal, 1);
			break;
		case IMPACT_FLESH:
			pointparticles(PARTICLE_BLOOD, vecPos, vNormal, 1);
			break;
		case IMPACT_DEFAULT:
			pointparticles(PARTICLE_SPARK, vecPos, vNormal, 1);
			pointparticles(PARTICLE_PIECES_BLACK, vecPos, vNormal, 1);
			pointparticles(PARTICLE_SMOKE_GREY, vecPos, vNormal, 1);
			break;
		default:
	}
	
	switch (iType) {
#ifdef OLD_CSTRIKE
		case IMPACT_METAL:
			pointsound(vecPos, sprintf("weapons/ric_metal-%d.wav", floor((random() * 2) + 1)), 1, ATTN_STATIC);
			break;
		case IMPACT_ROCK:
			pointsound(vecPos, sprintf("weapons/ric_conc-%d.wav", floor((random() * 2) + 1)), 1, ATTN_STATIC);
			break;
#endif
		case IMPACT_FLESH:
			break;
		default:
			pointsound(vecPos, sprintf("weapons/ric%d.wav", floor((random() * 5) + 1)), 1, ATTN_STATIC);
			break;
	}
	
#endif
}

void Effect_BreakModel(int count, vector vMins, vector vMaxs, vector vVel, float fStyle) {
#ifdef SERVER
	WriteByte(MSG_MULTICAST, SVC_CGAMEPACKET);
	WriteByte(MSG_MULTICAST, EV_MODELGIB);
	WriteCoord(MSG_MULTICAST, vMins[0]); 
	WriteCoord(MSG_MULTICAST, vMins[1]); 
	WriteCoord(MSG_MULTICAST, vMins[2]);
	WriteCoord(MSG_MULTICAST, vMaxs[0]); 
	WriteCoord(MSG_MULTICAST, vMaxs[1]); 
	WriteCoord(MSG_MULTICAST, vMaxs[2]);
	WriteByte(MSG_MULTICAST, fStyle);
	WriteByte(MSG_MULTICAST, count);

	msg_entity = self;
	
	vector vWorldPos;
	vWorldPos[0] = vMins[0] + (0.5 * (vMaxs[0] - vMins[0]));
	vWorldPos[1] = vMins[1] + (0.5 * (vMaxs[1] - vMins[1]));
	vWorldPos[2] = vMins[2] + (0.5 * (vMaxs[2] - vMins[2]));
	multicast(vWorldPos, MULTICAST_PVS);
#else
	static void Effect_BreakModel_Remove(void) { remove(self) ; }

	float fModelCount;
	vector vecPos;
	string sModel = "";

	switch (fStyle) {
		case GSMATERIAL_GLASS:
		case GSMATERIAL_GLASS_UNBREAKABLE:
			sModel = "models/glassgibs.mdl";
			fModelCount = 8;
			break;
		case GSMATERIAL_WOOD:
			sModel = "models/woodgibs.mdl";
			fModelCount = 3;
			break;
		case GSMATERIAL_METAL:
			sModel = "models/metalplategibs.mdl";
			fModelCount = 13;
			break;
		case GSMATERIAL_FLESH:
			sModel = "models/fleshgibs.mdl";
			fModelCount = 4;
			break;
		case GSMATERIAL_TILE:
			sModel = "models/ceilinggibs.mdl";
			fModelCount = 4;
			break;
		case GSMATERIAL_COMPUTER:
			sModel = "models/computergibs.mdl";
			fModelCount = 15;
			break;
		case GSMATERIAL_ROCK:
			sModel = "models/rockgibs.mdl";
			fModelCount = 3;
			break;
		default:
		case GSMATERIAL_CINDER:
			sModel = "models/cindergibs.mdl";
			fModelCount = 9;
			break;
	}
	
	vector vWorldPos;
	vWorldPos[0] = vMins[0] + (0.5 * (vMaxs[0] - vMins[0]));
	vWorldPos[1] = vMins[1] + (0.5 * (vMaxs[1] - vMins[1]));
	vWorldPos[2] = vMins[2] + (0.5 * (vMaxs[2] - vMins[2]));
	
	switch (fStyle) {
		case GSMATERIAL_GLASS:
			pointsound(vWorldPos, sprintf("debris/bustglass%d.wav", random(1, 4)), 1.0f, ATTN_NORM);
			break;
		case GSMATERIAL_WOOD:
			pointsound(vWorldPos, sprintf("debris/bustcrate%d.wav", random(1, 4)), 1.0f, ATTN_NORM);
			break;
		case GSMATERIAL_METAL:
		case GSMATERIAL_COMPUTER:
			pointsound(vWorldPos, sprintf("debris/bustmetal%d.wav", random(1, 3)), 1.0f, ATTN_NORM);
			break;
		case GSMATERIAL_FLESH:
			pointsound(vWorldPos, sprintf("debris/bustflesh%d.wav", random(1, 3)), 1.0f, ATTN_NORM);
			break;
		case GSMATERIAL_CINDER:
		case GSMATERIAL_ROCK:
			pointsound(vWorldPos, sprintf("debris/bustconcrete%d.wav", random(1, 4)), 1.0f, ATTN_NORM);
			break;
		case GSMATERIAL_TILE:
			pointsound(vWorldPos, "debris/bustceiling.wav", 1.0f, ATTN_NORM);
			break;
	}
	
	for (int i = 0; i < count; i++) {
		entity eGib = spawn();
		eGib.classname = "gib";
		
		vecPos[0] = vMins[0] + (random() * (vMaxs[0] - vMins[0]));	
		vecPos[1] = vMins[1] + (random() * (vMaxs[1] - vMins[1]));	
		vecPos[2] = vMins[2] + (random() * (vMaxs[2] - vMins[2]));	
		
		setorigin(eGib, vecPos);
		setmodel(eGib, sModel);
		setcustomskin(eGib, "", sprintf("geomset 0 %f\n", random(1, fModelCount + 1)));
		eGib.movetype = MOVETYPE_BOUNCE;
		eGib.solid = SOLID_NOT;
		
		eGib.avelocity[0] = random()*600;
		eGib.avelocity[1] = random()*600;
		eGib.avelocity[2] = random()*600;
		eGib.think = Effect_BreakModel_Remove;
		eGib.nextthink = time + 10;
		
		if ((fStyle == GSMATERIAL_GLASS) || (fStyle == GSMATERIAL_GLASS_UNBREAKABLE)) {
			eGib.effects = EF_ADDITIVE;
		}

		eGib.drawmask = MASK_ENGINE;
	}
#endif
}
