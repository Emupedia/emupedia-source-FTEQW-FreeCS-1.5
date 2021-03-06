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

/*enum
{
	DRAWFLAG_NORMAL,
	DRAWFLAG_ADDITIVE,
	DRAWFLAG_MODULATE, // FTE doesn't support this
	DRAWFLAG_2XMODULATE // FTE doesn't support this
};*/

enumflags
{
	EVF_FADEDROM,
	EVF_MODULATE,
	EVF_ONLYUSER
};

const string mat_fade_modulate =
	"{\n" \
		"{\n" \
			"map $whiteimage\n" \
			"rgbGen vertex\n" \
			"blendFunc GL_DST_COLOR GL_ONE_MINUS_SRC_ALPHA\n" \
			"alphaGen vertex\n" \
		"}\n" \
	"}\n";

void
Fade_Init(void)
{
	shaderforname("fade_modulate", mat_fade_modulate);
}

void
Fade_Update (int x, int y, int w, int h)
{
	if (pSeat->m_iFadeActive == FALSE) {
		return;
	}
	if (pSeat->m_flFadeStyle & EVF_FADEDROM) {
		if (pSeat->m_flFadeTime > pSeat->m_flFadeHold) {
			pSeat->m_flFadeAlpha -= (clframetime * (1.0f / pSeat->m_flFadeDuration)) * pSeat->m_flFadeMaxAlpha;
		}
	} else {
		if (pSeat->m_flFadeTime < pSeat->m_flFadeDuration) {
			pSeat->m_flFadeAlpha += (clframetime * (1.0f / pSeat->m_flFadeDuration)) * pSeat->m_flFadeMaxAlpha;
		} else {
			pSeat->m_flFadeAlpha -= (clframetime * (1.0f / pSeat->m_flFadeHold)) * pSeat->m_flFadeMaxAlpha;
		}
	}

	if (pSeat->m_flFadeAlpha > 1.0f) {
		pSeat->m_flFadeAlpha = 1.0f;
	} else if (pSeat->m_flFadeAlpha < 0.0f) {
		pSeat->m_flFadeAlpha = 0.0f;
	}

	if (pSeat->m_flFadeAlpha <= 0) {
		pSeat->m_iFadeActive = FALSE;
		return;
	}

	if (pSeat->m_flFadeStyle & EVF_MODULATE) {
		drawpic([x, y], "fade_modulate", [w, h], pSeat->m_vecFadeColor, pSeat->m_flFadeAlpha, 0);
	} else {
		drawfill([x, y], [w, h], pSeat->m_vecFadeColor, pSeat->m_flFadeAlpha, 0);
	}

	pSeat->m_flFadeTime += clframetime;
}

void
Fade_Parse (void)
{
	pSeat->m_vecFadeColor[0] = readfloat();
	pSeat->m_vecFadeColor[1] = readfloat();
	pSeat->m_vecFadeColor[2] = readfloat();
	pSeat->m_flFadeMaxAlpha = readfloat();
	pSeat->m_flFadeDuration = readfloat();
	pSeat->m_flFadeHold = readfloat();
	pSeat->m_flFadeStyle = readbyte();
	pSeat->m_flFadeTime = 0.0f;

	if (pSeat->m_flFadeStyle & EVF_FADEDROM) {
		pSeat->m_flFadeAlpha = 1.0f;
	} else {
		pSeat->m_flFadeAlpha = 0.0f;
	}

	pSeat->m_iFadeActive = TRUE;
}
