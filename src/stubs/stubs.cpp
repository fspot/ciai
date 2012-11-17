#include "stubs.h"

bool stubPresencePalette()
{
#ifdef ABS_PALETTE
	return false;
#else if
	return (rand() % PROBA_ABS_PALETTE != 0);
#endif
}

bool stubErrEmbalagePalette()
{
#ifdef ERR_EMBALAGE
	return true;
#else if
	return (rand() % PROBA_ERR_EMBALAGE == 0);
#endif
}
