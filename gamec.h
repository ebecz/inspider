#ifndef __GAMEC_H__
#define __GAMEC_H__

#include <stddef.h>
#include "game.h"

#define GAME_MSG_SIZE 1280

struct nctableau {
	struct tableau tableau;
	wchar_t msg[GAME_MSG_SIZE];
};

void nctableau_init(struct nctableau *nctableau);

void nctableau_run(struct nctableau *nctableau);

void nctableau_finish(const struct nctableau *nbtableau);

#endif
