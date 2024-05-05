#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <iostream>

#include "token.hpp"
#include "expression.hpp"
#include "sheet.hpp"
#include "exceptions.hpp"

///Kifejezéseket értelmező osztály
/**
A parser osztályt egy std::string-el inicializáljuk, amit a Parser konstruktora tokenekre bont.
Parser a parse tagfüggvényének segítségével megpróbálja értelmezni az adott kifejezést, és
amennyiben ez lehetséges (azaz a kifejezés szintaktikailag helyes), létre is hozza a kifejezést
dinamikus memóriaterületen, különben syntax_error típusú kivételt dob. A kifejezés értelmezése az
alábbi szabályok alapján zajlik:\n
	expression     → factor ( ( "-" | "+" ) factor )* ;\n
	factor         → unary ( ( "/" | "*" ) unary )* ;\n
	unary          → "-" unary | function | primary;\n
	function       → STRING "(" cell ":" cell ")";\n
	cell           → ('$')? STRING ('$' NUMBER)?;\n
	primary        → NUMBER | "(" expression ")" | cell;\n
Minden ilyen fent leírt szabályhoz tartozik egy-egy tagfüggvény, amelyeknek feladata, hogy a
tokenlistának éppen aktív (current) tokenjétől kezdve megpróbáljon értelmezni egy megfelelő
típusú kifejezést, ha ez lehetséges, akkor az ehhez szükséges tokeneket “elfogyasztja”, így a
következő ilyen tagfüggvény azokat a tokeneket már nem fogja tudni felhasználni. Így láthatjuk,
hogy a parse függvény igazából csak annyit tesz, hogy az első tokent állítja be aktív tokennek
(current=0) és meghívja az expression-t értelmező függvényt.
A kifejezések értelmezésének pontos menetének megvalósításához a program Robert Nystrom:
Crafting Interpreters c. könyvéből merít ihletet, amely az alábbi linken elérhető:
https://craftinginterpreters.com.

Megj.: az "-" unary mintára illeszkedő egy -1-el való szorzásra fordítja a parser, minden
más mintának saját osztálya van
*/
class Parser {
	std::vector<Token*> tokens; ///<az értelmezni kívánt kifejezés tokenizált alakban
	size_t current = 0; ///<az éppen feldolgozás alatt álló token indexe

	bool atEnd() const {return current >= tokens.size();} ///<ellenőrzi, hogy a feldolgozás végén járunk-e
	Token* prev() const {return tokens[current-1];} ///visszaadja az előző tokent
	bool check(Token_type tt) const {return !atEnd() && tokens[current]->getType()==tt;}
		///<ellenőrzi a jelenlegi token egy adott típusú-e
	bool match(Token_type tt);
		///<ellenőrzi a jelenlegi token egy adott típusú-e, ha igen, akkor tovább lépteti a feldolgozást
	///megpróbálja "elfogyasztani" a jelenlegi tokent ha az az adott típusú
	/**
	@param tt - elfogyasztani kívánt tokentípus
	@param msg - ha a jelenlegi token nem az adott típusú ez az üzenet kerül a syntax_error belsejébe
	*/
	Token* consume(Token_type tt, const char* msg);

	Expression* expression(Sheet* shp = NULL);
	Expression* factor(Sheet* shp = NULL);
	Expression* unary(Sheet* shp = NULL);
	Expression* function(Sheet* shp = NULL);
	Expression* primary(Sheet* shp = NULL);
	CellRefExpr* cell(Sheet* shp = NULL);
public:
	Parser(std::string input); ///<konstruktor: a megadott stringet tokenlistává alakítja
	Parser& operator=(const Parser& p); ///<értékadó operátor
	Parser(const Parser& p) {*this = p;} ///<másoló konstruktor

	void addToken(Token_type t); ///<hozzáad a tokenek listájához egy megadott típusú tokent
	void addToken(std::string s);
		///<hozzáad a tokenek listájához egy STRING típusú adattokent a paraméterként megadott stringgel mint belső adat
	void addToken(double n);
		///<hozzáad a tokenek listájához egy NUMBER típusú adattokent a paraméterként megadott számmal mint belső adat
	void addTokenFromStr(std::string& str_buffer);
		///<megpróbál kiolvasni egy számot a kapott stringből, ha sikeresen, akkor hozzádja NUMBER tokenként, ha nem akkor STRING tokenként

	///kifejezés értelmezése
	/**
	megpróbálja értelmezni a kifejezést az elejétől, ha sikertelen, syntax_error kivételt dob
	@param shp - ha a kifejezés taralmaz referenciákat, akkor erre a táblára fognak vonatkozni */
	Expression* parse(Sheet* shp = NULL);
	/**
	ha sikeres, akkor az adott tábla adott cellájába berakja a kifejezést
	@param shp - ha a kifejezés taralmaz referenciákat, akkor erre a táblára fognak vonatkozni
	@param ep - az értelmezett kifejezés ebbe a cellába kerül
	*/
	void parseTo(Sheet* shp, ExprPointer& ep);
		///<megpróbálja értelmezni a kifejezést az elejétől, ha sikertelen, syntax_error kivételt dob
	std::string show(); ///<kiírja a tokenlistáját egy std::stringbe

	~Parser() {
		for (size_t i = 0; i < tokens.size(); i++) {
			delete tokens[i];
		} ///<tokenlista felszabadítása
	}
};

#endif
