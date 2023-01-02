#include "signature.h"

void arC_signature_zero(arC_signature* s)
{
	arString_zero(&(s)->signature); 
	arString_zero(&(s)->local_signature); 
	arString_zero(&(s)->name);
}

BOOL arC_signature_is_zero(const arC_signature* s)
{
	return arString_is_zero(&s->signature);
}
