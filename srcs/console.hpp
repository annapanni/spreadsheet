#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "parser.hpp"
#include "sheet.hpp"
#include "exceptions.hpp"

///Felhasználói felület biztosítására szolgáló osztály
/**
A adott input- és  outputstream-ről, a parancsokat
tud beolvasni, illetve a parancsok eredményét ki tudja írni. Az osztály mindig egy darab
táblázatot tartalmaz, a kapott utasításokat ezen hajtja végre. A parancsok kulcsszavait a
readCommand tagfüggvény értelmezi, és ő hívja meg a megfelelő parancsot lekezelő
tagfüggvényt. Az adott tagfüggvény az inputstreamről beolvassa a parancs paramétereit és
végrehajtja a azt. Ha a felhasználó szintaktikailag hibás parancsot ad, akkor a konzol kapja
el a program által generált kivételeket, és hibaüzenetet ír az outputstreamre.
*/
class Console {
	Sheet sh; ///<a táblázat, amelyen a parancsok végrehajtódnak
	std::ostream& ostream; ///<a kimenettel rendelkező parancsok kimenetét ide írja a konzol
	std::istream& istream; ///<a parancsok nevét és paramétereit innen olvassa a konzol
	bool closed = false; ///<bezárták-e a konzolt
public:
	Console() : ostream(std::cout), istream(std::cin) {}
		///<alapértelmezett konstruktor, input és outputstream-je a std::cin és std::cout
	Console(const Sheet& sh, std::ostream& ostream, std::istream& istream) : sh(sh), ostream(ostream), istream(istream) {}
		///<konstruktor tábla, input- és outputstreamek megadásával
	Console(std::ostream& ostream, std::istream& istream) : ostream(ostream), istream(istream) {}
		///<konstruktor csak input- és outputstreamek megadásával

	bool isClosed() const {return closed;} ///<visszaadja, bezárták-e a konzolt
	void help(); ///<kiírja az ostream-re az elérhető parancsokat

	//*** Az alábbi parancsok a tesztelés megkönnyítésének érdekében publikusak, lehetnének privátak
			///új táblát hoz létre (ha volt előző, azt eldobja)
			/**paramétereit az istream-ről olvassa: új tábla szélesség és magassága*/
			void createNew();
			///átméretezi a táblát, ha kisebb lesz, a fennmaradó adat elveszik
			/**paramétereit az istream-ről olvassa: tábla új szélesség és magassága*/
			void resize();
			void print() {sh.formattedPrint(ostream);} ///<kiírja az ostream-re a tábla tartalmát oszlop- és sorszámokkal
			void exportValues(); ///<istream-ről bekért fájlnevű fájlba kiírja a táblában tárolt értékeket vesszővel elválasztva
			void save(); ///<istream-ről bekért fájlnevű fájlba kiírja a táblában tárolt kifejezéseket vesszővel elválasztva
			void load(); ///<istream-ről bekért fájlnevű fájlból beolvassa a vesszővel elválasztott kifejezéseket
			void set(); ///<istream-ről bekért cellába beállítja a megadott kifejezést (amennyiben szintaktikailag helyes)
			///automatikusan kitölti a kezdőcellában található értékkel a cellákat a második paraméterben kapott celláig egy téglalapban
			/**
			a kezdőcellában található kifejezést átmásolja a két cella által meghatározott
			téglalap minden cellájába, ezen felül minden nem abszolút hivatkozást eltol a kezdőcellától
			vett relatív pozíciójának megfelelően (ld. CellRefExpr::shift)
			*/
			void pull();
			void show(); ///<kiírja az ostream-re a istream-ről olvasott cella tartalmát és értékét
			void exit() {closed = true;} ///<bezárja a konzolt
	// A fenti parancsok a tesztelés megkönnyítésének érdekében publikusak, lehetnének privátak

	///beolvassa és értelmezi az istream-re beírt parancs nevét, és meghívja a megfelelő tagfüggvényt
	/**ha helytelen parancsnevet kap, hibaüzenetet ír az ostream-re*/
	void readCommand();
};


#endif
