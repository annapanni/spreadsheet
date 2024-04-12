#include <gtest/gtest.h>
#include <string>

#include "expression.hpp"
#include "sheet.hpp"
#include "parser.hpp"

TEST(Expression, Number){
	NumberExpr n (3);
	EXPECT_EQ(n.eval(), 3);
	EXPECT_EQ(n.show(), "3");
	Expression* nc = n.copy();
	EXPECT_EQ(nc->eval(), 3);
	EXPECT_NO_THROW(n.checkCyclic({nc}));
	delete nc;
}
TEST(Expression, CellRef){
	CellRefExpr empty("sdf645");
	EXPECT_EQ(empty.show(), "sdf645");
	EXPECT_THROW(empty.eval(), const char*);
	Sheet sh(3, 3, 5);
	CellRefExpr c (std::string("b3"), &sh);
	EXPECT_EQ(c.eval(), 5);
	EXPECT_EQ(c.show(), "b3");
	EXPECT_EQ(c.getCol(), "b");
	EXPECT_EQ(c.getRow(), 3);
	EXPECT_EQ(c.getPtr(), &(sh[2][1]));
	Expression* cc = c.copy();
	EXPECT_EQ(cc->show(), "b3");
	EXPECT_NO_THROW(c.checkCyclic({cc}));
	EXPECT_THROW(c.checkCyclic({*(sh[2][1])}), const char*);
	EXPECT_THROW(cc->checkCyclic({*(sh[2][1])}), const char*);
	delete cc;

	CellRefExpr cell (std::string("b2"), &sh);
	cell.shift(1,-1);
	EXPECT_EQ(cell.getPtr(), &(sh[0][2]));
	cell.shift(-1, 2);
	EXPECT_EQ(cell.getPtr(), &(sh[2][1]));
	cell.shift(0, -1);
	EXPECT_EQ(cell.getPtr(), &(sh[1][1]));
	cell.shift(2, 0);
	EXPECT_THROW(cell.getPtr(), const char*);
}

Sheet sh(3,3, 5);
CellRefExpr* a1 = new CellRefExpr("a1", &sh);
CellRefExpr* b3 = new CellRefExpr("b3", &sh);
CellRefExpr* c2 = new CellRefExpr("c2", &sh);

TEST (Expression, Range){
	Range r1(a1->copy(), a1->copy());

	Range::iterator it = r1.begin();
	EXPECT_EQ(it++, a1->getPtr());
	EXPECT_EQ(it++, r1.end());

	Range r2(a1->copy(), b3->copy());
	EXPECT_EQ(r2.show(), "a1:b3");
	it = r2.begin();
	EXPECT_EQ(r2.getRelX(&sh[0][0]), 0);
	EXPECT_EQ(r2.getRelY(&sh[0][0]), 0);
	EXPECT_EQ(r2.getRelX(&sh[0][2]), 2);
	EXPECT_EQ(r2.getRelY(&sh[0][2]), 0);
	EXPECT_EQ(r2.getRelX(&sh[2][1]), 1);
	EXPECT_EQ(r2.getRelY(&sh[2][1]), 2);

	int db = 0;
	while (it++ != r2.end()) {db++;}
	EXPECT_EQ(db, 6);

	Range r3(c2->copy(), a1->copy());
	EXPECT_EQ(r3.show(), "a1:c2");
	it = r3.begin();
	db = 0;
	while (it++ != r3.end()) {db++;}
	EXPECT_EQ(db, 6);
}

TEST (Expression, Range2){
	Range r4(b3->copy(), c2->copy());
	EXPECT_EQ(r4.show(), "b2:c3");
	Range::iterator it = r4.begin();
	int db = 0;
	while (it++ != r4.end()) {db++;}
	EXPECT_EQ(db, 4);

	r4.shift(-1, 0);
	EXPECT_EQ(r4.show(), "a2:b3");

	CellRefExpr* a3 = a1->copy();
	a3->shift(0, 2);
	Range r5(a1->copy(), a3);
	EXPECT_EQ(r5.show(), "a1:a3");
	it = r5.begin();
	db = 0;
	while (it++ != r5.end()) {db++;std::cout << db << "\n";}
	EXPECT_EQ(db, 3);
}

TEST (Expression, Function){
	SumFunc sum = SumFunc(a1->copy(), b3->copy());
	EXPECT_THROW(sum.checkCyclic({**(a1->getPtr()+1)}), const char*);
	EXPECT_EQ(sum.eval(), 30);
	FunctionExpr* avg = newFunctionExpr(AVG, c2->copy(), a1->copy());
	EXPECT_NO_THROW(avg->checkCyclic({**(b3->getPtr())}));
	EXPECT_EQ(avg->eval(), 5);
	EXPECT_EQ(avg->show(), "avg(a1:c2)");
	avg->shift(0, 1);
	EXPECT_EQ(avg->show(), "avg(a2:c3)");
	delete avg;
	EXPECT_EQ(FunctionExpr::parseFname("avg"), AVG);
	EXPECT_EQ(FunctionExpr::parseFname("sum"), SUM);
	EXPECT_EQ(FunctionExpr::parseFname("ddfas"), INVALID);
}

TEST (Expression, Mult){
	Expression* opcpy;
	{
	Mult op = Mult(a1->copy(), b3->copy());
	EXPECT_EQ(op.eval(), 25);
	EXPECT_EQ(op.show(), "(a1*b3)");
	opcpy = op.copy();
	}
	EXPECT_EQ(opcpy->eval(), 25);
	EXPECT_EQ(opcpy->show(), "(a1*b3)");
	EXPECT_THROW(opcpy->checkCyclic({**(b3->getPtr())}), const char*);
	opcpy->shift(1, 0);
	EXPECT_EQ(opcpy->show(), "(b1*c3)");
	delete opcpy;
}

TEST (Expression, Div){
	Expression* opcpy;
	{
	Div op = Div(a1->copy(), b3->copy());
	EXPECT_EQ(op.eval(), 1);
	EXPECT_EQ(op.show(), "(a1/b3)");
	opcpy = op.copy();
	}
	EXPECT_EQ(opcpy->eval(), 1);
	EXPECT_EQ(opcpy->show(), "(a1/b3)");
	EXPECT_THROW(opcpy->checkCyclic({**(b3->getPtr())}), const char*);
	opcpy->shift(1, 0);
	EXPECT_EQ(opcpy->show(), "(b1/c3)");
	delete opcpy;
}

TEST (Expression, Add){
	Expression* opcpy;
	{
	Add op = Add(a1->copy(), b3->copy());
	EXPECT_EQ(op.eval(), 10);
	EXPECT_EQ(op.show(), "(a1+b3)");
	opcpy = op.copy();
	}
	EXPECT_EQ(opcpy->eval(), 10);
	EXPECT_EQ(opcpy->show(), "(a1+b3)");
	EXPECT_THROW(opcpy->checkCyclic({**(b3->getPtr())}), const char*);
	opcpy->shift(1, 0);
	EXPECT_EQ(opcpy->show(), "(b1+c3)");
	delete opcpy;
}

TEST (Expression, Sub){
	Expression* opcpy;
	{
	Sub op = Sub(a1->copy(), b3->copy());
	EXPECT_EQ(op.eval(), 0);
	EXPECT_EQ(op.show(), "(a1-b3)");
	opcpy = op.copy();
	}
	EXPECT_EQ(opcpy->eval(), 0);
	EXPECT_EQ(opcpy->show(), "(a1-b3)");
	EXPECT_THROW(opcpy->checkCyclic({**(b3->getPtr())}), const char*);
	opcpy->shift(1, 0);
	EXPECT_EQ(opcpy->show(), "(b1-c3)");
	delete opcpy;
}

TEST (Sheet, statics){
	EXPECT_EQ(Sheet::colLetter(4), "d");
	EXPECT_EQ(Sheet::colLetter(27), "aa");
	EXPECT_EQ(Sheet::colNumber("e"), 5);
	EXPECT_EQ(Sheet::colNumber("ab"), 28);
	EXPECT_EQ(Sheet::colLetter(Sheet::colNumber("abcdf")), "abcdf");
	EXPECT_EQ(Sheet::colNumber(Sheet::colLetter(4268)), 4268);
	EXPECT_THROW(Sheet::colNumber("*"), const char*);
	Sheet sh2;
}

TEST (Sheet, constrAndGetters){
	Sheet sh2;
	{
		Sheet sh(5, 6, 3);
		EXPECT_EQ(sh.getWidth(), 5);
		EXPECT_EQ(sh.getHeight(), 6);
		EXPECT_EQ(sh[0][0]->eval(), 3);
		sh[0][0] = new NumberExpr(7);
		EXPECT_EQ(sh[0][0]->eval(), 7);
		EXPECT_THROW(sh[6][0]->show(), const char*);
		sh2 = sh;
	}
	EXPECT_EQ(sh2[0][1]->eval(), 3);
	EXPECT_EQ(sh2[0][0]->eval(), 7);
}

TEST (Sheet, functions){
	Sheet sh(5, 6, 3);
	EXPECT_EQ(sh[0][0], *(sh.parseCell(1, 1)));
	EXPECT_EQ(sh[0][0], *(sh.parseCell("a", 1)));
	EXPECT_EQ(sh[0][2], *(sh.parseCell(3, 1)));
	EXPECT_EQ(sh[1][2], *(sh.parseCell("c", 2)));
	EXPECT_THROW(sh.parseCell("f", 2), const char*);
	EXPECT_EQ(sh.checkRow(6), true);
	EXPECT_EQ(sh.checkRow(0), false);
	EXPECT_EQ(sh.checkCol(5), true);
	EXPECT_EQ(sh.checkCol(0), false);

	sh[0][0] = new NumberExpr(7);
	Sheet sh2(2,2);
	sh.copyTo(sh2);
	EXPECT_EQ(sh2[0][0]->eval(), 7);
	sh2.resize(4, 3, 1.2);
	EXPECT_EQ(sh2.getWidth(), 4);
	EXPECT_EQ(sh2.getHeight(), 3);
	EXPECT_EQ(sh2[2][3]->eval(), 1.2);
}

TEST (Parser, constructorsAndTokens){
	Parser p3("dd+(23-34/(-12))");
	EXPECT_EQ(p3.show(), "string, plus, left br, number, minus, number, slash, left br, minus, number, right br, right br, ");
	{
		Parser p1("a+b+22*3");
		Parser p2 = p1;
		EXPECT_EQ(p1.show(), "string, plus, string, plus, number, star, number, ");
		EXPECT_EQ(p2.show(), "string, plus, string, plus, number, star, number, ");
		p3 = p2;
	}
	EXPECT_EQ(p3.show(), "string, plus, string, plus, number, star, number, ");
	Parser p("");
	p.addToken(PLUS);
	p.addToken("hali");
	p.addToken(5.2);
	p.addToken(STAR);
	std::string s = "45";
	p.addTokenFromStr(s);
	EXPECT_EQ(p.show(), "plus, string, number, star, number, ");
}

TEST (Parser, parsing){
	Expression* expr = Parser("-34+12*(45-20)").parse();
	EXPECT_EQ(expr->show(), "((-1*34)+(12*(45-20)))");
	EXPECT_EQ(expr->eval(), 266);
	delete expr;
	expr = Parser("a1+ sdf345* (234 +sum(c4:d34) )*2").parse();
	EXPECT_EQ(expr->show(), "(a1+((sdf345*(234+sum(c4:d34)))*2))");
	delete expr;
	Sheet sh(2,3,3);
	Parser("34+b2").parseTo(&sh, sh[0][0]);
	EXPECT_EQ(sh[0][0]->show(), "(34+b2)");
	EXPECT_EQ(sh[0][0]->eval(), 37);
}

TEST (Parser, parsingErrors){
	EXPECT_THROW(Parser("").parseThrow(), const char*);
	EXPECT_THROW(Parser("a").parseThrow(), const char*);
	EXPECT_THROW(Parser("34+").parseThrow(), const char*);
	EXPECT_THROW(Parser("/234").parseThrow(), const char*);
	EXPECT_THROW(Parser("32+34*").parseThrow(), const char*);
	EXPECT_THROW(Parser("hah(a1:b2)").parseThrow(), const char*);
	EXPECT_THROW(Parser("sum(a1)").parseThrow(), const char*);
	EXPECT_THROW(Parser("sum(a1:)").parseThrow(), const char*);
	EXPECT_THROW(Parser("sum(a1:b2").parseThrow(), const char*);
	EXPECT_THROW(Parser("12* 34 + ((12 +1)").parseThrow(), const char*);
}

TEST (Parser, evalErrors){
	Expression* expr = Parser("a4").parse();
	EXPECT_THROW(expr->eval(), const char*);
	delete expr;

	Sheet(1, 1);
	Parser("a1").parseTo(&sh, sh[0][0]);
	EXPECT_THROW(sh[0][0].evalMe(), const char*);
	Parser("asd1").parseTo(&sh, sh[0][0]);
	EXPECT_THROW(sh[0][0].evalMe(), const char*);
	Parser("a10").parseTo(&sh, sh[0][0]);
	EXPECT_THROW(sh[0][0].evalMe(), const char*);
}

TEST (Deleting, deleting){
	delete a1;
	delete b3;
	delete c2;
}
