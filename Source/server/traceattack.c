/***
*
*	Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
*
*	See the file LICENSE attached with the sources for usage details.
*
****/

int iTotalPenetrations;

/*
=================
TraceAttack_FireSingle

Fires a single shot that can penetrate some materials
=================
*/
void TraceAttack_FireSingle(vector vPos, vector vAngle, int iDamage)
{
	/*static void TraceAttack_Penetrate(vector vPos, vector vAngle ) {
		if (iTotalPenetrations > 0) {
			return;
		}

		TraceAttack_FireSingle(vPos, vAngle, iDamage);
		iTotalPenetrations = 1;
	}*/

#ifdef CSTRIKE
	traceline(vPos, vPos + (vAngle * wptTable[self.weapon].fRange), MOVE_HITMODEL, self);
#else
	traceline(vPos, vPos + (vAngle * 8196), MOVE_HITMODEL, self);
#endif

	if (trace_fraction != 1.0) {
		if (trace_ent.takedamage == DAMAGE_YES) {
#ifdef CSTRIKE
			Damage_Apply(trace_ent, self, iDamage, trace_endpos, FALSE);
#endif
		}

		if (trace_ent.iBleeds == TRUE) {
			Effect_CreateBlood(trace_endpos, [0,0,0]);
		} else {
			string sTexture = getsurfacetexture(trace_ent, getsurfacenearpoint(trace_ent, trace_endpos));

			switch((float)hash_get(hashMaterials, sTexture)) { 
				case 'G':
				case 'V':
					Effect_Impact(IMPACT_METAL, trace_endpos, trace_plane_normal);
					break;
				case 'M':
				case 'P':
					Effect_Impact(IMPACT_METAL, trace_endpos, trace_plane_normal);
					break;
				case 'D':
				case 'W':
					Effect_Impact(IMPACT_WOOD, trace_endpos, trace_plane_normal);
					break;
				case 'Y':
					Effect_Impact(IMPACT_GLASS, trace_endpos, trace_plane_normal);
					break;
				case 'N':
					Effect_Impact(IMPACT_DEFAULT, trace_endpos, trace_plane_normal);
					break;
				case 'T':
				default:
					Effect_Impact(IMPACT_DEFAULT, trace_endpos, trace_plane_normal);
					break;
			 }

			//TraceAttack_Penetrate(trace_endpos + (v_forward * 2), vAngle);
		}
	}
}

void TraceAttack_FireSingleLagged(vector vPos, vector vAngle, int iDamage)
{
	/*static void TraceAttack_Penetrate(vector vPos, vector vAngle ) {
		if (iTotalPenetrations > 0) {
			return;
		}
		
		TraceAttack_FireSingle(vPos, vAngle, iDamage);
		iTotalPenetrations = 1;
	}*/
	
#ifdef CSTRIKE
	traceline(vPos, vPos + (vAngle * wptTable[self.weapon].fRange), MOVE_LAGGED | MOVE_HITMODEL, self);
#else
	traceline(vPos, vPos + (vAngle * 8196), MOVE_LAGGED | MOVE_HITMODEL, self);
#endif

	if (trace_fraction != 1.0) {
		if (trace_ent.takedamage == DAMAGE_YES) {

			Damage_Apply(trace_ent, self, iDamage, trace_endpos, FALSE);

		}

		if (trace_ent.iBleeds == TRUE) {
			//Effect_CreateBlood(trace_endpos, [0,0,0]);
		} else {
			string sTexture = getsurfacetexture(trace_ent, getsurfacenearpoint(trace_ent, trace_endpos));

			switch ((float)hash_get(hashMaterials, sTexture)) { 
				case 'G':
				case 'V':
					Effect_Impact(IMPACT_METAL, trace_endpos, trace_plane_normal);
					//TraceAttack_Penetrate(trace_endpos + (v_forward * 2), vAngle);
					break;
				case 'M':
				case 'P':
					Effect_Impact(IMPACT_METAL, trace_endpos, trace_plane_normal);
					break;
				case 'D':
				case 'W':
					Effect_Impact(IMPACT_WOOD, trace_endpos, trace_plane_normal);
					//TraceAttack_Penetrate(trace_endpos + (v_forward * 2), vAngle);
					break;
				case 'Y':
					Effect_Impact(IMPACT_GLASS, trace_endpos, trace_plane_normal);
					break;
				case 'N':
					Effect_Impact(IMPACT_DEFAULT, trace_endpos, trace_plane_normal);
					//TraceAttack_Penetrate(trace_endpos + (v_forward * 2), vAngle);
					break;
				case 'T':
				default:
					Effect_Impact(IMPACT_DEFAULT, trace_endpos, trace_plane_normal);
					break;
			 }
		}
	}
}

/*
=================
TraceAttack_FireBullets

Fire a given amount of shots
=================
*/
void TraceAttack_FireBullets(int iShots, vector vPos, int iDamage, vector vecAccuracy)
{
	vector vDir;
	makevectors(self.v_angle);

	while (iShots > 0) {
		iTotalPenetrations = 0;

		vDir = aim(self, 100000) + Math_CRandom()*vecAccuracy[0]*v_right + Math_CRandom()*vecAccuracy[1]*v_up;

		//TraceAttack_FireSingle(vPos, vDir, iDamage);
		TraceAttack_FireSingleLagged(vPos, vDir, iDamage);
		iShots--;
	}
}
