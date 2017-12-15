#include "ensitlm.h"
#include "Generator.h"
#include "bus.h"
#include "memory.h"
#include "LCDC.h"
#include "ROM.h"
#include "systemc.h"
#include "const.h"

using namespace std;

int sc_main(int argc, char **argv) {
	(void)argc;
	(void)argv;

  /* Instanciations */
	Generator g1("Generator1");
	Bus b("Bus");
	Memory m("Memory",0x15400);
	LCDC lcd("LCDC", sc_time(1.0 / 25, SC_SEC));
	ROM rom("Rom");

	sc_signal < bool , SC_MANY_WRITERS > irq_signal ( "IRQ" );

	/* Mapping */
	b.map(m.target, MEM_BASE_ADDR, MEM_SIZE);
	b.map(lcd.target_socket, LCD_REG_BASE, LCD_REG_SIZE);
	b.map(rom.socket, ROM_BASE_ADDR, ROM_SIZE);

	/* Connexions */
	g1.initiator.bind(b.target);
	b.initiator.bind(m.target);
	lcd.initiator_socket.bind(b.target);
	b.initiator.bind(lcd.target_socket);
	lcd.display_int.bind(irq_signal);
	g1.display_int.bind(irq_signal);
	b.initiator.bind(rom.socket);

	/* Simulation */
	sc_core::sc_start();
	return 0;
}
