#include <stddef.h>
#include "gamec.h"

struct nctableau nctableau;

int main(int argc, char *argv[])
{
	nctableau_init(&nctableau);

	nctableau_run(&nctableau);

	nctableau_finish(&nctableau);
}

