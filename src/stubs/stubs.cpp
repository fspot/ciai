#include "stubs.h"

bool stubPresencePalette()
{
#ifdef ABS_PALETTE
	return false;
#elif
	return (rand() % PROBA_ABS_PALETTE != 0);
#endif
}

bool stubErrEmbalagePalette()
{
#ifdef ERR_EMBALAGE
	return true;
#elif
	return (rand() % PROBA_ERR_EMBALAGE == 0);
#endif
}