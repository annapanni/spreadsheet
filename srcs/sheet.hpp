#ifndef SHEET_HPP
#define SHEET_HPP

#include <string>
#include <vector>
#include <math.h>

#include "expression_core.hpp"
#include "exceptions.hpp"

///Számolótáblát reprezentáló osztály
/**
A Sheet osztály egy N×M méretű dinamikus memóriaterületen sorfolytonosan tárolja el az adott
cellában lévő kifejezés értékét az ExprPointer osztály pédényaiként.
*/
class Sheet {
	ExprPointer* table; ///<a táblázat tartalma sorfolytonosan
	size_t width; ///<tábla szélessége
	size_t height; ///<tábla magassága
public:
	Sheet() : table(NULL), width(0), height(0) {} ///<konstruktor
	Sheet(const Sheet&); ///<másoló konstruktor
	/**
	@param w - létrehozandó tábla szélessége
	@param h - létrehozandó tábla magassága
	@param fill - a létrejövő tábla minden celláját ezzel a számmal inicializálja
	*/
	Sheet(size_t w, size_t h, double fill = 0); ///<konstruktor adott számmal inicializálással
	size_t getWidth() const {return width;} ///<tábla szélességének lekérdezése
	size_t getHeight() const {return height;}  ///<tábla magasságának lekérdezése
	Sheet& operator=(const Sheet&); ///<értékadó operátor
	ExprPointer* operator[](size_t i) {
		if (i < height)
			return table + i*width;
		throw std::out_of_range("");
	} ///<adott sor lekérdezése 0-tól indexelve
	ExprPointer* parseCell(int col, int row) const;
		///<tábla adott cellájára mutató pointer visszaadása oszlopszám és sorszám alapján
	ExprPointer* parseCell(std::string col, int row) const;
		///<tábla adott cellájára mutató pointer visszaadása oszlopbetű és sorszám alapján
	bool checkRow(int r) const {return r <= (int)height && r > 0;}
		///<ellenőrzi, hogy a táblázatban szerepel-e adott sorszámú sor (1-től indexelve)
	bool checkCol(int col) const {return col <= (int)width && col > 0;}
		///<ellenőrzi, hogy a táblázatban szerepel-e adott sorszámú oszlop (1-től indexelve)

	int getYCoord(ExprPointer* cell) const;
		///<visszaadja, hogy egy adott cellára mutató pointer melyik sorban van (0-tól indexelve)
	int getXCoord(ExprPointer* cell) const;
		///<visszaadja, hogy egy adott cellára mutató pointer melyik oszlopban van (0-tól indexelve)
	/**
	a paraméterként kapott tábla mérete nem változik meg, tehát ha kisebb volt,
	akkor az adatok részét nem másolja át, ha nagyobb volt, akkor a paraméterként
	kapott tábla fennmaradó részében az adatok változatlanok maradnak
	*/
	void copyTo(Sheet& sh) const; ///<átmásolja a tábla tartalmát egy másik, paraméterként kapott táblába
	/**
	Ha kisebbre méretezzük a táblát, akkor az adatok egy része elveszik, ha nagyobbra,
	akkor a fill paraméterben megadott számmal tölti ki az újonnan keletkező részt
	*/
	void resize(size_t w, size_t h, double fill = 0); ///<átméretezi a táblát

	void formattedPrint(std::ostream& os = std::cout) const;
		///<kiértékeli és kiírja a cellák értékét, illetve az oszlop és sorszámokat a kapott ostream-re
	void printValues(std::ostream& os = std::cout) const;
		///<kiértékeli és kiírja a cellák értékét vesszővel elválasztva a kapott ostream-re
	void printExpr(std::ostream& os = std::cout) const;
		///<kiírja a cellákban található kifejezéseket a kapott ostream-re

	~Sheet(){delete[] table;} ///<felszabadítja a táblát

	static int colNumber(std::string); ///<oszlopbetű oszlopszámra alakítása (1-től indexelve)
	static std::string colLetter (int); ///<oszlopszám oszlopbetűre alakítása (1-től indexelve)
};


#endif
