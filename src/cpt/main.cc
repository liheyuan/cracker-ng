
/*!
 * \file main.cc
 * \brief ccrypt module for Cracker-ng.
 * \author Mickaël 'Tiger-222' Schoentgen
 * \date 2012.11.22
 *
 * Copyright (C) 2012 Mickaël 'Tiger-222' Schoentgen.
 */


#include "./main.h"

// C'est parti mon kiki !
Cracker::Cracker(std::string filename, std::string from) :
	filename(filename), from(from),
	filei(filename.c_str(), std::ios::in | std::ios::binary
) {
	printf(" . Working ...\n");
}

Cracker::~Cracker() {
	printf(" ^ Ex(c)iting.\n");
}

void Cracker::crack() {
	std::string chosen_one;
	size_t num = 0;
	unsigned int found = 0;
	functions_ng::statistics s = { &num, &found };
	pthread_t stat;
	ccrypt_stream_s *b = new ccrypt_stream_s;
	ccrypt_state_s *st = new ccrypt_state_s;
	roundkey *rkks     = new roundkey;
	roundkey rkk_hash;
	char *encryption_header = new char[32];
	char *p                 = new char[PWD_MAX];
	char *buffer            = new char[PWD_MAX];
	char *inbuf             = new char[32];
	FILE *input;

	// Read encrypted data
	this->filei.seekg(0, std::ios::beg);
	this->filei.read(encryption_header, 32);
	this->filei.close();

	// Initializing
	st->rkks = rkks;

	// Read from input ...
	if ( this->from == "STDIN" ) {
		input = stdin;
	} else {
		input = fopen(this->from.c_str(), "r");
	}

	// Let's go!
	pthread_create(&stat, NULL, functions_ng::stats, reinterpret_cast<void*>(&s));
	while ( functions_ng::read_stdin(buffer, PWD_MAX, input, p) ) {
		ccdecrypt_init(b, st, p, rkk_hash);
		memcpy(inbuf, encryption_header, 32);
		b->next_in = inbuf;
		b->avail_in = 32;
		if ( ccdecrypt(b) == 0 ) {
			chosen_one = p;
			found = 1;
			break;
		}
		++num;
	}
	delete[] inbuf;
	delete[] p;
	delete[] encryption_header;
	delete rkks;
	delete st;
	delete b;
	if ( this->from != "STDIN" ) {
		fclose(input);
	}
	if ( found == 0 ) {
		found = 2;
	}
	pthread_join(stat, reinterpret_cast<void**>(NULL));
	functions_ng::result(chosen_one);
}


int main(int argc, char *argv[]) {
	std::string filename, input;
	functions_ng::arguments argz = {
		MODULE, std::string(VERSION), filename, input, (size_t)argc, argv
	};

	if ( !functions_ng::argz_traitment(argz) ) {
		return 0;
	}
	printf(" ~ %s Cracker-ng v.%s { Tiger-222 }\n", MODULE, VERSION);
	printf(" - File......: %s\n", argz.filename.c_str());
	printf(" - Input.....: %s\n", argz.input.c_str());

	// Who I am? I'm a champion!
	Cracker zizi(argz.filename, argz.input);
	zizi.crack();
	return 0;
}
