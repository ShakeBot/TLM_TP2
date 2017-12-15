#ifndef GENERATOR_H
#define GENERATOR_H

#include "ensitlm.h"

SC_MODULE(Generator) {
	ensitlm::initiator_socket<Generator> initiator;

	sc_core::sc_in <bool> display_int;
	sc_core::sc_event display_done;

	void thread(void);
	void lcd_interruption(void);

	SC_CTOR(Generator);
};

#endif
