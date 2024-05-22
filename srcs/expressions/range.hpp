#ifndef RANGE_HPP
#define  RANGE_HPP


#include <string>

#include "expression_core.hpp"
#include "cell.hpp"
#include "../exceptions.hpp"

///Cellahivatkozások egy téglalap alakú tartományát reprezentáló osztály
/**A téglalapot a bal felső és jobb alsó cellája határozza meg, és elvárás, hogy
ez a két cella egy számolótáblán legyen. */
class Range {
	///a tartomány bal felső cellájára vonatkozó hivatkozás, elvárás, hogy ez azonos táblán legyen, mint a jobb alsó cellahivatkozás
	CellRefExpr* topCell;
	///a tartomány jobb alsó cellájára vonatkozó hivatkozás, elvárás, hogy ez azonos táblán legyen, mint a bal felső cellahivatkozás
	CellRefExpr* bottomCell;
public:
	class iterator; //<tartományt sorfolytonosan bejáró iterátor

	///konstruktor a tartomány két sarokcellájára mutató hivatkozás megadásával
	/**A két megadott sarokcella-hivatkozás bármely sorrendben, bármely átló szerint megadható,
	azonban a range mindig bal felső - jobb felső sorrendben tárolja el őket.
	Ezért a konstruktor új referenciákat készít és ezeket tárolja el végül és bejárni
	is a bal felső cellából kezdi így a bejárást.
	*/
	explicit Range(CellRefExpr* top, CellRefExpr* bottom);
	///másoló konstruktor
	Range(const Range& r) : topCell(r.topCell->copy()), bottomCell(r.bottomCell->copy()) {}
	Range& operator=(const Range& r); ///<értékadás operátor
	iterator begin() const; ///<tartomány első cellájára mutató iterátor visszaadása
	iterator end() const; ///<tartomány utolsó cellája utáni cellára mutató iterátor visszaadása
	///tartomány megjelenítése std::string-ként "a1:c4" formátumban
	std::string show() const {return topCell->show() + ":" + bottomCell->show();}
	///eltolja a taromány sarokcelláit adott sorral és oszloppal, amennyiben a sor/oszlop nem abszolút
	void shift(int dx, int dy) {topCell->shift(dx, dy); bottomCell->shift(dx, dy);}
	///a taromány sarokcelláinak célpontját áthelyezi egy másik számolótáblára
	void relocate(Sheet* shp) {topCell->relocate(shp); bottomCell->relocate(shp);}
	///sarokcella hivatkozások felszabadítása
	~Range(){
		delete topCell;
		delete bottomCell;
	}

	///Tartományt sorfolytonosan bejáró iterátor
	class iterator {
		size_t rangeWidth; ///<tartomány szélessége (bejáráshoz szükséges)
		size_t tableWidth; ///<tábla szélessége (bejáráshoz szükséges)
		ExprPointer* actRow; ///<aktuális sor kezdőcellájára mutató pointer
		ExprPointer* actCell; ///<aktuális cellára mutató pointer
	public:
		///konstruktor
		/**
		csak a tartománybeli sorok elejéről lehet indítani az iterátort
		@param rangeWidth - a bejárandó tartomány szélessége
		@param tableWidth - a bejárandó tartományt taralmazó tábla szélessége
		@param beginPtr - a cella, melyre az iterátor kezdetben mutat
		*/
		iterator(size_t rangeWidth, size_t tableWidth, ExprPointer* beginPtr)
			: rangeWidth(rangeWidth), tableWidth(tableWidth), actRow(beginPtr), actCell(beginPtr) {}
		///iterátor tartalmának kiolvasása
		ExprPointer& operator*() const {return *actCell;}
		ExprPointer* operator->() const {return actCell;} ///<iterátor tartalmának tagjainak elérése
		bool operator==(const ExprPointer* ep) const {return actCell == ep;} ///<egyenlőség ExprPointer*-el
		bool operator==(const iterator& it) const {return actCell == it.actCell;} ///<egyenlőség egy másik iterátorral
		bool operator!=(const iterator& it) const {return actCell != it.actCell;} ///<egyenlőtlenség egy másik iterátorral
		iterator& operator++(); ///<preinkremens
		iterator operator++(int); ///<posztinkremens
	};
};


#endif
