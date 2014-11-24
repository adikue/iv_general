#include <stdlib.h>
#include <stdio.h>
#include "base64.h"

int main(){
	char base64[10];
	char bin[3];
	int r;
	r = bin_to_base64("M", 1, base64, 10);
	printf("Conv bin->base64 result = %d bin = %s, base64 = %s\n", r, "Man", base64);	
	r = base64_to_bin(base64, 4, bin, 3);
	printf("Conv base64->bin result = %d  base64 = %s bin=%s\n", r, base64, bin);
}