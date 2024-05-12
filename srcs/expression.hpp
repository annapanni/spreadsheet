#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <iostream>
#include <vector>

#include "expression_core.hpp"
#include "sheet.hpp"
#include "exceptions.hpp"
#include "token.hpp"

///Cellát azonosító sor- és oszlopadat eltárolására szolgáló osztály.
/** oszlop és sorindexeket is 1-től indexelve tárolja, paraméterként is így várja.*/
class CellId {
	int colNum; ///<oszlop sorszáma 1-től indexelve
	int row; ///<sorszám 1-től indexelve
public:
	CellId(std::string col, int row) : colNum(Sheet::colNumber(col)), row(row) {}
			///<konstruktor oszlopjelölő betű és sorszám megadásával
	CellId(std::string); ///<konstruktor "[oszlopbetű][sorszám]" formátumú bemenettel
	int getColNum() const {return colNum;} ///<oszlopszám lekérdezése
	int getRow() const {return row;} ///<sorszám lekérdezése
	void setColNum(int c) {colNum = c;} ///<oszlopszám beállítása
	void setRow(int r) {row = r;} ///<sorszám beállítása
	std::string colLetter() const {return Sheet::colLetter(colNum);} ///<oszlopbetű lekérdezése
};

///Cellahivatkozást reprezentáló kifejezés osztály.
/**A hivatkozás egy tábla (Sheet) egy cellájára mutathat oszlop és sor megadásával.
Mind az oszlopa, mind a sora egymástól független lehetnek abszolútak.*/
class CellRefExpr : public Expression {
	CellId cell; ///<cellát azonosító sor- és oszlopadat
	Sheet* refSheet; ///<tábla, amelyre a hivatkozás mutat
	bool absCol; ///<oszlopát tekintve abszolút-e a hivatkozás
	bool absRow; ///<sorát tekintve abszolút-e a hivatkozás
public:
	///konstruktor oszlopjelölő betű és sorszám megadásával
	/**
	@param col - oszlopbetű
	@param row - sorszám
	@param refSheet - tábla, amelyre a hivatkozás mutat
	@param absCol - abszolút hivatkozás-e az oszlop
	@param absRow - abszolút hivatkozás-e a sor
	 */
	CellRefExpr(std::string col, int row, Sheet* refSheet = nullptr, bool absCol=false, bool absRow=false)
		: cell(CellId(col, row)), refSheet(refSheet), absCol(absCol), absRow(absRow) {}
	///konstruktor "[oszlopbetű][sorszám]" formátumú bemenettel
	/**
	@param str - cella jelölője ("[oszlopbetű][sorszám]")
	@param refSheet - tábla, amelyre a hivatkozás mutat
	@param absCol - abszolút hivatkozás-e az oszlop
	@param absRow - abszolút hivatkozás-e a sor
	 */
	CellRefExpr(std::string str, Sheet* refSheet = nullptr, bool absCol=false, bool absRow=false)
		: cell(CellId(str)), refSheet(refSheet), absCol(absCol), absRow(absRow) {}
	std::string getCol() const {return cell.colLetter();} ///<oszlopbetű lekérdezése
	int getRow() const {return cell.getRow();} ///<sorszám lekérdezése
	Sheet* getSheet() const {return refSheet;} ///<hivatkozás által mutatott tábla lekérdezése
	ExprPointer* getPtr() const {if (refSheet == nullptr) throw eval_error("uninitialized cell");
		return refSheet->parseCell(cell.getColNum(), cell.getRow());}
		///<hivatkozás által mutatott cellára mutató pointer lekérdezése
	bool getAbsCol() const {return absCol;} ///<oszlop abszolút voltának lekérdezése
	bool getAbsRow() const {return absRow;} ///<sor abszolút voltának lekérdezése
	double eval() const; ///<hivatkozás által mutatott cella kiértékelése
	void checkCyclic(std::vector<Expression*>) const;
	std::string show() const {return (absCol?"$":"") + cell.colLetter() + (absRow?"$":"") + std::to_string(cell.getRow());}
	CellRefExpr* copy() const {return new CellRefExpr(*this);}
	///Eltolja a hivatkozást adott sorral és oszloppal, amennyiben a sor/oszlop nem abszolút
	/**
	@param dx - sor eltolásának mértéke (akár negatív)
	@param dy - oszlop eltolásának mértéke (akár negatív)
	*/
	void shift(int dx, int dy);
	void relocate(Sheet* shp) {refSheet = shp;} ///<a cellahivatkozás célpontját áthelyezi egy másik számolótáblára
};

///Cellahivatkozások egy téglalap alakú tartományát reprezentáló osztály
/**A téglalapot a bal felső és jobb alsó cellája határozza meg, és elvárás, hogy
ez a két cella egy számolótáblán legyen. */
class Range {
	CellRefExpr* topCell;
		///<a tartomány bal felső cellájára vonatkozó hivatkozás, elvárás, hogy ez azonos táblán legyen, mint a jobb alsó cellahivatkozás
	CellRefExpr* bottomCell;
		///<a tartomány jobb alsó cellájára vonatkozó hivatkozás, elvárás, hogy ez azonos táblán legyen, mint a bal felső cellahivatkozás
public:
	class iterator; //<tartományt sorfolytonosan bejáró iterátor
	///konstruktor a tartomány két sarokcellájára mutató hivatkozás megadásával
	/**A két megadott sarokcella-hivatkozás bármely sorrendben, bármely átló szerint megadható,
	azonban a range mindig bal felső - jobb felső sorrendben tárolja el őket.
	Ezért a konstruktor új referenciákat készít és ezeket tárolja el végül és bejárni
	is a bal felső cellából kezdi így a bejárást.
	*/
	Range(CellRefExpr* bg, CellRefExpr* ed);
	Range(const Range& r) : topCell(r.topCell->copy()), bottomCell(r.bottomCell->copy()) {}
		///<másoló konstruktor
	Range& operator=(const Range& r); ///<értékadás operátor
	iterator begin() const; ///<tartomány első cellájára mutató iterátor visszaadása
	iterator end() const; ///<tartomány utolsó cellája utáni cellára mutató iterátor visszaadása
	std::string show() const {return topCell->show() + ":" + bottomCell->show();}
		///<tartomány megjelenítése std::string-ként "a1:c4" formátumban
	void shift(int dx, int dy) {topCell->shift(dx, dy); bottomCell->shift(dx, dy);}
		///<eltolja a taromány sarokcelláit adott sorral és oszloppal, amennyiben a sor/oszlop nem abszolút
	void relocate(Sheet* shp) {topCell->relocate(shp); bottomCell->relocate(shp);}
		///<a taromány sarokcelláinak célpontját áthelyezi egy másik számolótáblára
	~Range(){
		delete topCell;
		delete bottomCell;
	} ///<sarokcella hivatkozások felszabadítása
	///Tartományt sorfolytonosan bejáró iterátor
	class iterator {
		size_t rangeWidth; ///<tartomány szélessége (bejáráshoz szükséges)
		size_t tableWidth; ///<tábla szélessége (bejáráshoz szükséges)
		ExprPointer* actRow; ///<aktuális sor kezdőcellájára mutató pointer
		ExprPointer* actCell; ///<aktuális cellára mutató pointer
	public:
		iterator() : rangeWidth(0), tableWidth(0), actRow(nullptr), actCell(nullptr) {} ///<üres iterátor létrehozása
		///konstruktor
		/**
		csak a tartománybeli sorok elejéről lehet indítani az iterátort
		@param rw - a bejárandó tartomány szélessége
		@param tw - a bejárandó tartományt taralmazó tábla szélessége
		@param bp - a cella, melyre az iterátor kezdetben mutat
		*/
		iterator(size_t rw, size_t tw, ExprPointer* bp)
			: rangeWidth(rw), tableWidth(tw), actRow(bp), actCell(bp) {}
		ExprPointer& operator*() const {if (actCell==nullptr) throw std::runtime_error("empty iterator"); return *actCell;}
			///<iterátor tartalmának kiolvasása, runtime_error-t dob ha üres iterátorból olvasunk
		ExprPointer* operator->() const {return actCell;} ///<iterátor tartalmának tagjainak elérése
		bool operator==(const ExprPointer* ep) const {return actCell == ep;} ///<egyenlőség ExprPointer*-el
		bool operator==(const iterator& it) const {return actCell == it.actCell;} ///<egyenlőség egy másik iterátorral
		bool operator!=(const iterator& it) const {return actCell != it.actCell;} ///<egyenlőtlenség egy másik iterátorral
		iterator& operator++(); ///<preinkremens
		iterator operator++(int); ///<posztinkremens
	};
};

///elérhető függvények nevei
enum FunctionName {
	INVALID, AVG, SUM
};

///Tartományon elvégezhető függvénykifejezések absztrakt osztálya
class FunctionExpr : public Expression {
protected:
	Range range; ///<tartomány, melyen a függvény végrehajtódik
public:
	FunctionExpr(Range r) : range(r) {} ///<konstruktor
	FunctionExpr(CellRefExpr* topCell, CellRefExpr* bottomCell) : range(topCell, bottomCell) {} ///<konstruktor
	void checkCyclic(std::vector<Expression*>) const;
	void shift(int dx, int dy) {range.shift(dx, dy);}
	void relocate(Sheet* shp) {range.relocate(shp);}
	virtual ~FunctionExpr(){}
	static FunctionName parseFname(std::string name){
		if (name == "avg") return AVG;
		if (name == "sum") return SUM;
		return INVALID;
	} ///<értelmezi a függvények neveit (case sensitive)
	static FunctionExpr* newFunctionExpr(FunctionName fn, CellRefExpr* topCell, CellRefExpr* bottomCell);
		///<létrehoz egy megfelelő típusú függvényt a neve alapján
};

///Tartomány átlagát vevő függvény osztály
class AvgFunc : public FunctionExpr {
public:
	AvgFunc(Range r) : FunctionExpr(r) {}
	AvgFunc(CellRefExpr* topCell, CellRefExpr* bottomCell) : FunctionExpr(topCell, bottomCell) {}
	double eval() const;
	std::string show() const {return "avg(" + range.show() + ")";}
	Expression* copy() const {return new AvgFunc(range);}
};

///Tartományt összegző függvény osztály
class SumFunc : public FunctionExpr {
public:
	SumFunc(Range r) : FunctionExpr(r) {}
	SumFunc(CellRefExpr* topCell, CellRefExpr* bottomCell) : FunctionExpr(topCell, bottomCell) {}
	double eval() const;
	std::string show() const {return "sum(" + range.show() + ")";}
	Expression* copy() const {return new SumFunc(range);}
};

///bináris műveleteket reprezentáló absztrakt osztály
class Operator : public Expression {
protected:
	Expression* lhs; ///<bal oldali operandus
	Expression* rhs; ///<jobb oldali operandus
public:
	///konstruktor
	/**
	@param lhs - bal oldali operandus
	@param rhs - jobb oldali operandus
	*/
	Operator(Expression* lhs, Expression* rhs) : lhs(lhs), rhs(rhs) {}
	Operator(const Operator& op) : lhs(op.lhs->copy()), rhs(op.rhs->copy()) {} ///<másoló konstruktor
	Operator& operator=(const Operator& op); ///<értékadás operátor
	void checkCyclic(std::vector<Expression*> prevs) const {lhs->checkCyclic(prevs); rhs->checkCyclic(prevs);}
	void shift(int dx, int dy) {lhs->shift(dx, dy); rhs->shift(dx, dy);}
	void relocate(Sheet* shp) {lhs->relocate(shp); rhs->relocate(shp);}
	virtual ~Operator(){
		delete lhs;
		delete rhs;
	} ///<felszabadítja az operandusait
	static Operator* operandFromToken(Token_type ttype, Expression* lhs, Expression* rhs);
		///<adott tokentípusnak megfelelő műveletet hoz létre
};

///Szorzás műveletet reprezentáló osztály
class Mult : public Operator {
public:
	Mult(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() * rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "*" + rhs->show() + ")";}
	Expression* copy() const {return new Mult(lhs->copy(), rhs->copy());}
};

///Osztás műveletet reprezentáló osztály
class Div : public Operator {
public:
	Div(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() / rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "/" + rhs->show() + ")";}
	Expression* copy() const {return new Div(lhs->copy(), rhs->copy());}
};

///Összeadás műveletet reprezentáló osztály
class Add : public Operator {
public:
	Add(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() + rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "+" + rhs->show() + ")";}
	Expression* copy() const {return new Add(lhs->copy(), rhs->copy());}
};

///Kivonás műveletet reprezentáló osztály
class Sub : public Operator {
public:
	Sub(Expression* lhs, Expression* rhs) : Operator(lhs, rhs) {}
	double eval() const {return lhs->eval() - rhs->eval();}
	std::string show() const {return "(" + lhs->show() + "-" + rhs->show() + ")";}
	Expression* copy() const {return new Sub(lhs->copy(), rhs->copy());}
};

#endif
