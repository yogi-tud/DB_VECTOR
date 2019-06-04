//
// Created by johannes on 14.05.19.
//





int compressV(uint64_t *input, uint64_t *out, uint8_t masking);
int compressAll(uint64_t *input, uint64_t *out, uint8_t * masking, int size);
__mmask8 createMask(uint64_t* maskin);