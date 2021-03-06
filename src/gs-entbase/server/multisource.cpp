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

/*QUAKED multisource (1 0 0) (-8 -8 -8) (8 8 8)
"targetname"    Name
"target"        Target when triggered.
"killtarget"    Target to kill when triggered.

STUB!
*/

class multisource:CBaseTrigger
{
	void(void) multisource;

	virtual int(void) GetValue;
	virtual void(void) Trigger;
};

int
multisource::GetValue(void)
{
	entity a;
	int out = TRUE;

	/* normal triggers */
	for (a = world; (a = find(a, CBaseTrigger::m_strTarget, m_strTargetName));) {
		CBaseTrigger tTemp = (CBaseTrigger) a;
#ifdef GS_DEVELOPER
		dprint("[^1MULTISOURCE^7] ");
		dprint(tTemp.classname);
		if (tTemp.GetValue() == FALSE) {
			dprint(" is ^1OFF^7, name: ");
			out = FALSE;
		} else {
			dprint(" is ^2ON^7, name: ");
		}
		dprint(tTemp.m_strTargetName);
		dprint("\n");
#else
		/* exit out immediately as there's no point unless in-dev */
		if (tTemp.GetValue() == FALSE) {
			return FALSE;
		}
#endif
	}

	return out;
}

void
multisource::Trigger(void)
{
	if (GetValue() == FALSE) {
		return;
	}

	CBaseTrigger::UseTargets();
}

void
multisource::multisource(void)
{
	CBaseTrigger::CBaseTrigger();
}
