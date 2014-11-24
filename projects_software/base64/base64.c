#include "base64.h"

int bin_to_base64(char *bin, uint32_t bin_length, char *base64, uint32_t base64_maxlen)
{
	uint32_t output_length = 4 * ((bin_length + 2) / 3);

	if (output_length > base64_maxlen)
		return 1;

	for (int i = 0, j = 0; i < bin_length;) {
		uint32_t octet_a = i < bin_length ? (unsigned char)bin[i++] : 0;
		uint32_t octet_b = i < bin_length ? (unsigned char)bin[i++] : 0;
		uint32_t octet_c = i < bin_length ? (unsigned char)bin[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		base64[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		base64[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		base64[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		base64[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (int i = 0; i < mod_table[bin_length % 3]; i++)
		base64[output_length - 1 - i] = '=';

	return 0;
}

int base64_to_bin(char *base64, uint32_t base64_len, char *bin, uint32_t bin_maxlength)
{
	if (base64_len % 4 != 0) return 2;//if length is not base64

	uint32_t output_length = base64_len / 4 * 3;
	if (base64[base64_len - 1] == '=') (output_length)--;
	if (base64[base64_len - 2] == '=') (output_length)--;

	if (output_length > bin_maxlength)
		return 1;

	for (int i = 0, j = 0; i < base64_len;) {
		uint32_t sextet_a = base64[i] == '=' ? 0 & i++ : decoding_table[base64[i++]];
		uint32_t sextet_b = base64[i] == '=' ? 0 & i++ : decoding_table[base64[i++]];
		uint32_t sextet_c = base64[i] == '=' ? 0 & i++ : decoding_table[base64[i++]];
		uint32_t sextet_d = base64[i] == '=' ? 0 & i++ : decoding_table[base64[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
		+ (sextet_b << 2 * 6)
		+ (sextet_c << 1 * 6)
		+ (sextet_d << 0 * 6);

		if (j < output_length) bin[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < output_length) bin[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < output_length) bin[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return 0;
}

