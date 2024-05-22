#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>

/**
*Tokenek lehetséges típusai.
*A kifejezéseket az értelmező ilyen típusú tokenekre bontja
*/
enum Token_type {
	MINUS, PLUS, SLASH, STAR, LEFT_BR, RIGHT_BR, COLON,
	DOLLAR, NUMBER, STRING
};

///Tokenek osztálya: Az kifejezés értelmező ezen osztály példányaival tárolja el a kifejezéseket
class Token {
protected:
	Token_type type; ///<token típusa
public:
	explicit Token() {} ///<alapértelmezett konstruktor
	explicit Token(Token_type t) : type(t) {} ///<konstruktor típus megadásával
	Token_type getType() const {return type;} ///<típus lekérdezése

	std::string show() const; ///<token megjelenítése std::string-ként
	virtual Token* copy() {return new Token(type);} ///<dinamikusan foglalt memóriaterületen visszaadott másolat
	static Token_type parseTokenType(char c); ///<karakterhez megfelelő tokentípus rendelése
	virtual ~Token(){} ///<destruktor
};

///Adattartalommal rendelkezó tokenek osztálya.
/**A token osztály leszármazottja az olyan tokeneknek, amelyek a típusokon felül más adattartalommal is rendelkeznek.
*/
template <typename T>
class DataToken : public Token {
	T content; ///<a token adattartalma
public:
	explicit DataToken(Token_type tt, const T& s) : Token(tt), content(s) {} ///<konstruktor típus és adat megadásával
	T getContent() const {return content;} ///<belső adat lekérdezése
	Token* copy() const {return new DataToken(type, content);} ///<dinamikusan foglalt memóriaterületen visszaadott másolat
};

#endif
