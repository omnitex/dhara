#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "./bch.h"

// poly is output from chunk_remainder()
static void pack_poly(const struct bch_def *def, bch_poly_t poly, uint8_t *ecc)
{
	int i;

	// for the specified amount of ECC bytes wanted
	for (i = 0; i < def->ecc_bytes; i++) {
        printf("Packing ECC byte %d by with inverted poly 0x%"PRIx64": ", i, ~poly);
		ecc[i] = ~poly;
        printf("Packed ECC byte %d: 0x%"PRIx64"\n", i, ecc[i]);
		poly >>= 8;
	}
}

// bch_poly_t is uint64_t
static bch_poly_t chunk_remainder(const struct bch_def *def,
				  const uint8_t *chunk, size_t len)
{
	bch_poly_t remainder = 0;
	int i;

    printf("Calculating remainder for chunk of length %zu\n", len);

	for (i = 0; i < len; i++) {
		int j;

		// XOR with 0xff inverts the bits
		// so we accumulate remainder by XORing with inverted byte
		// chunk[i] is only uint8_t, remainder is uint64_t
        printf("Processing byte %d: 0x%02x\n", i, chunk[i]);
        printf("  Before XOR: remainder = 0x%"PRIx64"\n", remainder);
        printf("  XOR with 0x%02x\n", chunk[i] ^ 0xff);
		remainder ^= chunk[i] ^ 0xff;
        printf("  After XOR: remainder = 0x%"PRIx64"\n", remainder);

		// for each bit in the current chunk byte
		// polynomial long division?
		for (j = 0; j < 8; j++) {

			// TODO what does this do?
			// if the lowest bit is 1
			if (remainder & 1)
				// we need to subtract (XOR) the generator polynomial
				remainder ^= def->generator;
			// shift right to process the next bit
			remainder >>= 1;
		}
        printf("  After division by gen poly (0x%" PRIx64 ") remainder = 0x%" PRIx64 "\n", def->generator, remainder);
	}

	return remainder;
}

void calculate_bch_example(struct bch_def def)
{
    // Example data
    uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    size_t len = sizeof(data);

    // Calculate remainder
    bch_poly_t rem = chunk_remainder(&def, data, len);
    printf("BCH remainder: 0x%" PRIx64 "\n", rem);

    // Pack ECC
    uint8_t ecc[8] = {0};
    pack_poly(&def, rem, ecc);
    printf("Packed ECC: ");
    for (int i = 0; i < def.ecc_bytes; ++i) {
        printf("%02x ", ecc[i]);
    }
    printf("\n");

}

int main(void)
{
    calculate_bch_example(bch_1bit);
    calculate_bch_example(bch_4bit);

    return 0;
}
