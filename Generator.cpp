#include "Generator.h"
#include "LCDC_registermap.h"
#include "const.h"

#define Test 0
#define Blanche 0
#define Image 1

using namespace std;

void Generator::thread(void) {

	#if Test //Affichage du premier test
		ensitlm::data_t val = 0xFFFFFFFF;
		ensitlm::addr_t addr = 0x10000000;

		for(int i = 0; i < 0xFF; i++){
		val++;
		addr += 4;
		tlm::tlm_response_status rep = initiator.write(addr, val);
  	if(rep != 1){
	  	cout << "Error while write at " << addr << endl;
		}

		ensitlm::data_t valread;
		initiator.read(addr, valread);
		if(valread != val){
	  	cout 	<< "Error while writing at " << addr << ". "
		  			<< val << " expected but " << valread << " received"
		   			<< endl;
	  	abort();
		}
	}
	#endif

	#if Blanche //Affichage d'une image blanche
		ensitlm::data_t val = 0xFFFFFFFF;
		ensitlm::addr_t addr = VIDEO_MEM_BASE_ADDR;

		for (int i = 0; i < IMG_SIZE/4; i++){
			initiator.write(addr, val);
			addr +=4;
		}

		initiator.write(LCD_REG_BASE + LCDC_ADDR_REG, VIDEO_MEM_BASE_ADDR);
		initiator.write(LCD_REG_BASE + LCDC_INT_REG, 0x00000000);
		initiator.write(LCD_REG_BASE + LCDC_START_REG, 0x00000001);
	#endif

	#if Image
		ensitlm::data_t rom, video1, video2;

		int next = 0;

		  initiator.write(LCD_REG_BASE + LCDC_START_REG, 0x00000001);
		  initiator.write(LCD_REG_BASE + LCDC_ADDR_REG, VIDEO_MEM_BASE_ADDR);

  while(true){
    for(int i = 0 ; i< (IMG_SIZE/8) ; i++){
      initiator.read(ROM_BASE_ADDR + 4*i, rom);

      video1= (rom >> 4*3 & 0xF0 			)	| (rom >> 4*2 & 0xF000 		)|
								(rom >> 4*1 & 0xF00000	)	| (rom >> 4*0 & 0xF0000000);

			video2= (rom << 4*1 & 0xF0 			)	| (rom << 4*2 & 0xF000 		)|
								(rom << 4*3 & 0xF00000	)	| (rom << 4*4 & 0xF0000000);

      // Affichage sans défilement (Question 5)
			// initiator.write	(VIDEO_MEM_BASE_ADDR + 8*i 	,	ram1);
			// initiator.write	(VIDEO_MEM_BASE_ADDR + 8*i+4,	ram2);

			// Affichage avec défilement vertical (Question 6)
			initiator.write	(VIDEO_MEM_BASE_ADDR +
				((8*i + 320 * next)%(IMG_SIZE)) ,	video1);
			initiator.write	(VIDEO_MEM_BASE_ADDR +
				((8*i + 4 +	320 * next)%(IMG_SIZE)) ,	video2);
    }

    wait(display_done);
    wait(0.2,sc_core::SC_SEC);
		next++;
  }
	#endif
}

void Generator::lcd_interruption(){
	if (display_int.posedge()){
		initiator.write(LCD_REG_BASE + LCDC_INT_REG, 0x00000000);
		display_done.notify();
	}
}

Generator::Generator(sc_core::sc_module_name name) :
	sc_core::sc_module(name) {
	SC_THREAD(thread);
	SC_METHOD(lcd_interruption);
	sensitive << display_int;
}
