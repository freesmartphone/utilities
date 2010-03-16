/* 
 * (c) 2009 by Simon Busch <morphis@gravedo.de>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>

#define DEFAULT_TOKEN_INPUT			"/dev/mmcblk0p1"

struct token
{
	uint8_t *value;
	uint32_t value_len;
	uint8_t *key;
	uint32_t key_len;
};

int use_stdout = 1;
FILE *output = NULL;

void token_write_output(struct token *tok)
{
	FILE *out = use_stdout ? stdout : output;
	fprintf(out, "%s=%s\n", tok->key, tok->value);
	fflush(out);
}

void token_free(struct token *tok)
{
	free(tok->key);
	free(tok->value);
}

int parse_token(uint8_t *data, uint32_t size, uint32_t offset, struct token *new_token)
{
	int tmp = 0;

	/* check for some value */
	tmp = (data[offset+8]  >>  0) |
		  (data[offset+9]  >>  8) |
		  (data[offset+10] >> 16) |
		  (data[offset+11] >> 24);
	if (tmp == 0x10000)
		return 0;

	/* FIXME CRC32 check */

	/* key starts at 0x14 and there seems to be no stored length so we have to
	   to calculate its length first */
	uint8_t *p = data + offset + 0x14;
	new_token->key_len = 0;
	while (*p != 0) {
		new_token->key_len++;
		p++;
	}
	new_token->key = (uint8_t*)malloc(new_token->key_len);
	p = data + offset + 0x14;
	memcpy(new_token->key, p, new_token->key_len);
	new_token->key[new_token->key_len] = '\0';

	/* copy value */
	new_token->value_len = (data[offset+8] << 0) | (data[offset+9] << 8);
	new_token->value = (uint8_t*)malloc(new_token->value_len);
	p = data + offset + 0x24;
	memcpy(new_token->value, p, new_token->value_len);

#ifdef DEBUG
	printf("found new token (key = '%s', value = '%s')\n",
		   new_token->key,
		   new_token->value);
#endif

	return 1;
}

uint32_t find_next_token(uint8_t *data, uint32_t size, uint32_t offset, struct token *next_token)
{
	int tmp = 0;
	while (offset < size) {
		if (data[offset+0] == 0x54 &&
			data[offset+1] == 0x4f &&
			data[offset+2] == 0x4b &&
			data[offset+3] == 0x4e &&
			data[offset+4] == 0x1) {
			if(parse_token(data, size, offset, next_token))
				return offset+1;
		}
		offset += 1;
	}

	return 0;
}

void print_help()
{
	printf("usage: read_tokens [options]\n");
	printf("\t--input, -i   specify the input file (default: /dev/mmcblk0p1)\n");
	printf("\t--output, -o  specifiy the output file (default: stdout)\n");
}

int main (int argc, char *argv[])
{
	opterr = 0;
	int option_index;
	int chr;
	char input_name[255];
	char output_name[255];
	input_name[0] = '\0';
	uint8_t in_data[0x40000];
	int fd;

	snprintf(input_name, 255, DEFAULT_TOKEN_INPUT);

	struct option opts[] = {
		{ "help", no_argument, 0, 'h' },
		{ "input", required_argument, 0, 'i' },
		{ "output", required_argument, 0, 'o' },
	};

	while (1) {
		option_index = 0;
		chr = getopt_long(argc, argv, "i:o:h", opts, &option_index);

		if (chr == -1)
			break;

		switch (chr) {
		case 'i':
			snprintf(input_name, 255, "%s", optarg);
			break;
		case 'o':
			snprintf(output_name, 255, "%s", optarg);
			use_stdout = 0;
			break;
		case 'h':
			//print_help();
			return 0;
		default:
			break;
		}
	}

	fd = open(input_name, O_RDONLY | O_SYNC);
	if (fd < 0) {
		perror("open()");
		exit(1);
	}
	read(fd, in_data, 0x40000);
	
	uint32_t offset = 0x5000;
	fprintf(use_stdout ? stdout : output, "[tokens]\n");
	while(1)
	{
		struct token tok;
		offset = find_next_token(in_data, 0x40000, offset, &tok);
		if (offset == 0) 
			break;
		token_write_output(&tok);
		token_free(&tok);
	}

	close(fd);

	return 0;
}

