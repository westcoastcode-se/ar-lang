#ifndef _ARC_SIGNATURE_H_
#define _ARC_SIGNATURE_H_

#include "../arString.h"

// A symbol signature
// TODO: This should be part of the base language code instead of the compiler code
typedef struct arC_signature
{
	// The full signature
	arString signature;
	// The signature in local format
	arString local_signature;
	// The name of the signature
	arString name;
} arC_signature;

// Get a zero signature
#define arC_signature_empty() { 0 }

// Set the signature to an empty
ARLANG_API void arC_signature_zero(arC_signature* s);

// Check to see if the signature is not set
ARLANG_API BOOL arC_signature_is_zero(const arC_signature* s);

#endif
