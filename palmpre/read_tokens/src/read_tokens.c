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
#define KEY_OFFSET 0x14
#define MAX_KEY_LEN 0x10
#define VALUE_OFFSET ((KEY_OFFSET) + 0x10)
#define TOKEN_OFFSET 0x5000
#define TOKEN_SIZE 0x1000

struct token
{
	uint8_t *value;
	uint8_t *key;
};

void token_write_output(struct token *tok, FILE* out)
{
	fprintf(out, "%s=%s\n", tok->key, tok->value);
	fflush(out);
}

void token_free(struct token *tok)
{
	free(tok->key);
	free(tok->value);
}

int parse_token(uint8_t *data, uint32_t size, uint32_t* offset, struct token *new_token)
{
#ifdef DEBUG
	fprintf (stderr, "position %08x\n", *offset + TOKEN_OFFSET);
#endif
	int tmp = 0;
	uint8_t* cur_data = data + *offset;

	/* check for some value */
	tmp = (cur_data[8]  >>  0) |
		  (cur_data[9]  >>  8) |
		  (cur_data[10] >> 16) |
		  (cur_data[11] >> 24);
	if (tmp == 0x10000)
		return 0;

	/* FIXME CRC32 check */

	/* key starts at 0x14 and there seems to be no stored length so we have to
	   to calculate its length first */
	new_token->key = strndup (&cur_data[KEY_OFFSET], MAX_KEY_LEN);
	int value_len = (((int)cur_data[8])) + (((int)cur_data[9]) << 8);
	new_token->value = strndup(&cur_data[VALUE_OFFSET], value_len);
#ifdef DEBUG
	fprintf(stderr,"found new token (key = '%s', value = '%s')\n",
		   new_token->key,
		   new_token->value);
#endif
	*offset = *offset + value_len + VALUE_OFFSET;

	return 1;
}

uint32_t find_next_token(uint8_t *data, uint32_t size, uint32_t offset, struct token *next_token)
{
	while (offset < size) {
		if (data[offset+0] == 'T' &&
			data[offset+1] == 'O' &&
			data[offset+2] == 'K' &&
			data[offset+3] == 'N' &&
			data[offset+4] == 0x1) {
			if(parse_token(data, size, &offset, next_token))
				return offset;
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
	uint8_t in_data[TOKEN_SIZE];
	int fd;
	int use_stdout = 1;
	FILE *output = NULL;

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
			print_help();
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
	if (use_stdout)
	     output = stdout;
	else
	     output = fopen(output_name, "w");
	if (!output)
	     perror("open output");

	lseek(fd, 0x5000, SEEK_SET);
	read(fd, in_data, TOKEN_SIZE);
	
	uint32_t offset = 0;
	fprintf(output, "[tokens]\n");
	while(1)
	{
		struct token tok;
		offset = find_next_token(in_data, TOKEN_SIZE, offset, &tok);
		if (offset == 0) 
			break;
		token_write_output(&tok, output);
		token_free(&tok);
	}

	close(fd);

	return 0;
}

