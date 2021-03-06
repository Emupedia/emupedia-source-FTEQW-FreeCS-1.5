/***
*
*	Copyright (c) 2016-2019 Marco 'eukara' Hladik. All rights reserved.
*
*	See the file LICENSE attached with the sources for usage details.
*
****/

string sScoreTeams[4] = {
	_("SCORE_TITLE_SPECTATOR"),
	_("SCORE_TITLE_T"),
	_("SCORE_TITLE_CT"),
	_("SCORE_TITLE_VIP"),
};


void Scores_Init(void)
{
	
}

/*
====================
VGUI_Scores_DrawTeam
====================
*/
vector VGUI_Scores_DrawTeam(vector vecPos, float fTeam) {
	
	vector vColor;
	// Preserve the old vector so we can draw the amount of players per team later on
	vector vNewPos = vecPos;

	int iPlayerCount = 0;

	vNewPos += '0 32';

	drawfont = FONT_CON;
	// Player loop, this one goes through ALL players but only prints the one from fTeam
	for (int i = -1; i > -32; i--) {
		if (stof(getplayerkeyvalue(i, "*team")) == fTeam) {
			if (getplayerkeyvalue(i, "name")) {
				vColor = HUD_GetChatColor(fTeam);

				if (getplayerkeyvalue(i, "name") == getplayerkeyvalue(player_localnum, "name")) {
					drawfill(vNewPos+'19 -2', '493 16', vColor, 0.5f, DRAWFLAG_ADDITIVE);
					vColor = '1 1 1';
				}

				if (getplayerkeyvalue(i, "*dead") == "1") {
					drawstring(vNewPos+'38 0', sprintf(_("SCORE_DEAD"), getplayerkeyvalue(i, "name")), '12 12', vColor, 1.0f, 0);
				} else if (getplayerkeyvalue(i, "*dead") == "2") {
					drawstring(vNewPos+'38 0', sprintf(_("SCORE_VIP"), getplayerkeyvalue(i, "name")), '12 12', vColor, 1.0f, 0);
				} else {
					drawstring(vNewPos+'38 0', getplayerkeyvalue(i, "name"), '12 12', vColor, 1.0f, 0);
				}
				
				
				// Spectators don't have stats worth caring about
				if (fTeam != TEAM_SPECTATOR) {
					drawstring_r(vNewPos+'320 0', getplayerkeyvalue(i, INFOKEY_P_FRAGS), '12 12', vColor, 1.0f, 0);
					drawstring_r(vNewPos+'384 0', getplayerkeyvalue(i, "*deaths"), '12 12', vColor, 1.0f, 0);
				}
				drawstring_r(vNewPos+'456 0', getplayerkeyvalue(i, INFOKEY_P_PING), '12 12', vColor, 1.0f, 0);
					
				if (getplayerkeyvalue(i, INFOKEY_P_VOIPSPEAKING) == "0") {
					drawstring_r(vNewPos+'508 0', "N", '12 12', '1 1 1', 1.0f, 0);
				} else {
					drawstring_r(vNewPos+'508 0', "Y", '12 12', '1 1 1', 1.0f, 0);
				}
				
				vNewPos += '0 16';
				iPlayerCount++;
			}
		}
	}
	
	// Reset in case it was changed
	vColor = HUD_GetChatColor(fTeam);
	
	// If we've got no spectators, don't draw them.
	if ((fTeam != TEAM_SPECTATOR) || (fTeam == TEAM_SPECTATOR && iPlayerCount > 0)) {
		// The name/title of the team
		drawstring(vecPos+'24 0', sScoreTeams[fTeam], '12 12', vColor, 1.0f, 0);
		drawfill(vecPos+'19 24', '493 1', vColor, 1.0f);
		
		// Draw the amount of rounds we've von
		if (fTeam == TEAM_CT) {
			drawstring_r(vecPos+'320 0', sprintf("%i", getstati(STAT_WON_CT)), '12 12', vColor, 1.0f, 0);
		} else if (fTeam == TEAM_T) {
			drawstring_r(vecPos+'320 0', sprintf("%i", getstati(STAT_WON_T)), '12 12', vColor, 1.0f, 0);
		}
		
		// Now we know the playercount, so let's calculate the position next to the Teamname String and print it
		vector vCountPos = vecPos+'24 6';
		vCountPos[0] += stringwidth(sScoreTeams[fTeam], FALSE, '12 12')+8;
		drawstring(vCountPos, sprintf(_("SCORE_PLAYERS"), iPlayerCount), '12 12', vColor, 1.0f, 0);
	}
	return vNewPos+'0 24';
}

void Scores_Draw(void) {
	vector vMainPos;
	vector vSize;
	
	vSize[0] = 540;
	vSize[1] = video_res[1] - 112;
	
	vMainPos = video_mins;
	vMainPos[0] += (video_res[0] / 2) - (vSize[0] / 2);
	vMainPos[1] += 56;

	// Draw the background
	drawfill(vMainPos, vSize, [0,0,0], 0.75f);

	// Sides
	drawfill(vMainPos, [vSize[0], 1], '0.35 0.35 0.35', 1.0f);
	drawfill([vMainPos[0], vMainPos[1]+vSize[1] - 1], [vSize[0], 1], '0.35 0.35 0.35', 1.0f);
	drawfill(vMainPos, [1, vSize[1]], '0.35 0.35 0.35', 1.0f);
	drawfill([vMainPos[0]+vSize[0] - 1, vMainPos[1]], [1, vSize[1]], '0.35 0.35 0.35', 1.0f);

	// Server title
	drawstring(vMainPos+'24 13', serverkey("hostname"), '12 12', UI_MAINCOLOR, 1.0f, 0);

	// Tabs like Score, Ping etc.
	drawstring(vMainPos+'280 32', _("SCORE_SCORE"), '12 12', UI_MAINCOLOR, 1.0f, 0);
	drawstring(vMainPos+'336 32', _("SCORE_DEATHS"), '12 12', UI_MAINCOLOR, 1.0f, 0);
	drawstring(vMainPos+'400 32', _("SCORE_LATENCY"), '12 12', UI_MAINCOLOR, 1.0f, 0);
	drawstring(vMainPos+'472 32', _("SCORE_VOICE"), '12 12', UI_MAINCOLOR, 1.0f, 0);

	vector vOffset = VGUI_Scores_DrawTeam(vMainPos+'0 50', TEAM_CT);
	vOffset = VGUI_Scores_DrawTeam(vOffset, TEAM_T);
	vOffset = VGUI_Scores_DrawTeam(vOffset, 0);
}


