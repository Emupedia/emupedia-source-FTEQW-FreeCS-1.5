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

void
Game_Parse_Event(float fHeader)
{
	if (fHeader == EV_CHAT) {
		float fSender = readbyte();
		float fTeam = readbyte();
		string sMessage = readstring();

		CSQC_Parse_Print(sprintf("%s: %s", getplayerkeyvalue(fSender, "name"), sMessage), PRINT_CHAT);
	} else if (fHeader == EV_CHAT_TEAM) {
		float fSender2 = readbyte();
		float fTeam2 = readbyte();
		string sMessage2 = readstring();

		CSQC_Parse_Print(sprintf("[TEAM] %s: %s", getplayerkeyvalue(fSender2, "name"), sMessage2), PRINT_CHAT);
	} else if (fHeader == EV_CHAT_VOX) {
		Sound_PlayVOX(readstring());
	} else if (fHeader == EV_VIEWMODEL) {
		View_PlayAnimation(readbyte());
	} else if (fHeader == EV_WEAPON_PICKUP) {
		int w = readbyte();

		if (autocvar_cl_autoweaponswitch == 1) {
			sendevent("PlayerSwitchWeapon", "i", w);
		}

		player pl = (player)pSeat->m_ePlayer;
		if (getplayerkeyfloat(pl.entnum-1, "*team") == TEAM_CT) {
			setcustomskin(pSeat->m_eViewModel, "", "geomset 0 2\n");
		} else {
			setcustomskin(pSeat->m_eViewModel, "", "geomset 0 1\n");
		}

		HUD_WeaponPickupNotify(w);
	} else if (fHeader == EV_RADIOMSG) {
		Radio_PlayMessage(readbyte());
	} else if (fHeader == EV_RADIOMSG2) {
		Radio_PlayPlayerMessage(readbyte(), readbyte());
	}
}
