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
	std::ostream& os; ///<a kimenettel rendelkező parancsok kimenetét ide írja a konzol
	std::istream& is; ///<a parancsok nevét és paramétereit innen olvassa a konzol
	bool closed = false; ///<bezárták-e a konzolt
public:
	Console() : os(std::cout), is(std::cin) {}
		///<alapértelmezett konstruktor, input és outputstream-je a std::cin és std::cout
	Console(Sheet sh, std::ostream& os, std::istream& is) : sh(sh), os(os), is(is) {}
		///<konstruktor tábla, input- és outputstreamek megadásával
	Console(std::ostream& os, std::istream& is) : os(os), is(is) {}
		///<konstruktor csak input- és outputstreamek megadásával

	bool isClosed() const {return closed;} ///<visszaadja, bezárták-e a konzolt
	void help(); ///<kiírja az os-re az elérhető parancsokat

	///új táblát hoz létre (ha volt előző, azt eldobja)
	/**paramétereit az is-ről olvassa: új tábla szélesség és magassága*/
	void createNew();
	///átméretezi a táblát, ha kisebb lesz, a fennmaradó adat elveszik
	/**paramétereit az is-ről olvassa: tábla új szélesség és magassága*/
	void resize();
	void print() {sh.formattedPrint(os);} ///<kiírja az os-re a tábla tartalmát oszlop- és sorszámokkal
	void exportValues(); ///<is-ről bekért fájlnevű fájlba kiírja a táblában tárolt értékeket vesszővel elválasztva
	void save(); ///<is-ről bekért fájlnevű fájlba kiírja a táblában tárolt kifejezéseket vesszővel elválasztva
	void load(); ///<is-ről bekért fájlnevű fájlból beolvassa a vesszővel elválasztott kifejezéseket
	void set(); ///<is-ről bekért cellába beállítja a megadott kifejezést (amennyiben szintaktikailag helyes)
	///automatikusan kitölti a kezdőcellában található értékkel a cellákat a második paraméterben kapott celláig egy téglalapban
	/**
	a kezdőcellában található kifejezést átmásolja a két cella által meghatározott
	téglalap minden cellájába, ezen felül minden nem abszolút hivatkozást eltol a kezdőcellától
	vett relatív pozíciójának megfelelően (ld. CellRefExpr::shift)
	*/
	void pull();
	void show(); ///<kiírja az os-re a is-ről olvasott cella tartalmát és értékét
	void exit() {closed = true;} ///<bezárja a konzolt

	///beolvassa és értelmezi az is-re beírt parancs nevét, és meghívja a megfelelő tagfüggvényt
	/**ha helytelen parancsnevet kap, hibaüzenetet ír az os-re*/
	void readCommand();
};


#endif
