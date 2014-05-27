/*
 * uSynergy client -- Implementation for the embedded Synergy client library
 * version 1.0.0, July 7th, 2012

 * Copyright (c) 2012 Nick Bolton

 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.

 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 *  1. The origin of this software must not be misrepresented; you must not
 *  claim that you wrote the original software. If you use this software
 *  in a product, an acknowledgment in the product documentation would be
 *  appreciated but is not required.

 *  2. Altered source versions must be plainly marked as such, and must not be
 *  misrepresented as being the original software.

 *  3. This notice may not be removed or altered from any source
 *  distribution.
 */

#include "uSynergy.h"

extern uSynergyContext uSynergyLinuxContext;

int main(int argc, char **argv)
{
	char* ipStr;
	ipStr = malloc(strlen(argv[1]) + 1);

	uSynergyInit(&uSynergyLinuxContext, "android", 1024, 600);

	uSynergyLinuxContext.m_cookie->ipAddr = strcpy(ipStr, argv[1]);
	uSynergyLinuxContext.m_cookie->port = 24800;

	uSynergyStart(&uSynergyLinuxContext);
}
