#include <gtest/gtest.h>
#include <string>
#include <sstream>

#include "exceptions.hpp"
#include "expressions/expression.hpp"
#include "sheet.hpp"
#include "parser.hpp"
#include "console.hpp"


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
	EXPECT_THROW(empty.eval(), eval_error);
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
	EXPECT_THROW(c.checkCyclic({(Expression*)sh[2][1]}), eval_error);
	EXPECT_THROW(cc->checkCyclic({(Expression*)sh[2][1]}), eval_error);
	delete cc;

	CellRefExpr cell (std::string("b2"), &sh);
	cell.shift(1,-1);
	EXPECT_EQ(cell.getPtr(), &(sh[0][2]));
	cell.shift(-1, 2);
	EXPECT_EQ(cell.getPtr(), &(sh[2][1]));
	cell.shift(0, -1);
	EXPECT_EQ(cell.getPtr(), &(sh[1][1]));
	cell.shift(2, 0);
	EXPECT_THROW(cell.getPtr(), eval_error);
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
	while (it++ != r5.end()) {db++;}
	EXPECT_EQ(db, 3);
}

TEST (Expression, Function){
	SumFunc sum = SumFunc(a1->copy(), b3->copy());
	EXPECT_THROW(sum.checkCyclic({(Expression*)*(a1->getPtr()+1)}), eval_error);
	EXPECT_EQ(sum.eval(), 30);
	FunctionExpr* avg = FunctionExpr::newFunctionExpr(AVG, c2->copy(), a1->copy());
	EXPECT_NO_THROW(avg->checkCyclic({(Expression*)*(b3->getPtr())}));
	EXPECT_EQ(avg->eval(), 5);
	EXPECT_EQ(avg->show(), "avg(a1:c2)");
	avg->shift(0, 1);
	EXPECT_EQ(avg->show(), "avg(a2:c3)");
	delete avg;
	EXPECT_EQ(FunctionExpr::parseFname("avg"), AVG);
	EXPECT_EQ(FunctionExpr::parseFname("sum"), SUM);
	EXPECT_EQ(FunctionExpr::parseFname("ddfas"), std::optional<FunctionName>{});
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
	EXPECT_THROW(opcpy->checkCyclic({(Expression*)*(b3->getPtr())}), eval_error);
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
	EXPECT_THROW(opcpy->checkCyclic({(Expression*)*(b3->getPtr())}), eval_error);
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
	EXPECT_THROW(opcpy->checkCyclic({(Expression*)*(b3->getPtr())}), eval_error);
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
	EXPECT_THROW(opcpy->checkCyclic({(Expression*)*(b3->getPtr())}), eval_error);
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
	EXPECT_THROW(Sheet::colNumber("*"), syntax_error);
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
		EXPECT_THROW(sh[6][0]->show(), std::out_of_range);
		sh[1][0] = new CellRefExpr("a1", &sh);
		sh2 = sh;
		Sheet sh3(sh);
		sh[0][0] = new NumberExpr(5);
		EXPECT_EQ(sh3[1][0]->eval(), 7);
	}
	EXPECT_EQ(sh2[0][1]->eval(), 3);
	EXPECT_EQ(sh2[0][0]->eval(), 7);
	EXPECT_EQ(sh2[1][0]->eval(), 7);

	EXPECT_EQ(sh2.getXCoord(&sh2[0][0]), 0);
	EXPECT_EQ(sh2.getYCoord(&sh2[0][0]), 0);
	EXPECT_EQ(sh2.getXCoord(&sh2[0][2]), 2);
	EXPECT_EQ(sh2.getYCoord(&sh2[0][2]), 0);
	EXPECT_EQ(sh2.getXCoord(&sh2[2][1]), 1);
	EXPECT_EQ(sh2.getYCoord(&sh2[2][1]), 2);
}

TEST (Sheet, functions){
	Sheet sh(5, 6, 3);
	EXPECT_EQ(sh[0][0], *(sh.parseCell(1, 1)));
	EXPECT_EQ(sh[0][0], *(sh.parseCell("a", 1)));
	EXPECT_EQ(sh[0][2], *(sh.parseCell(3, 1)));
	EXPECT_EQ(sh[1][2], *(sh.parseCell("c", 2)));
	EXPECT_THROW(sh.parseCell("f", 2), eval_error);
	EXPECT_EQ(sh.checkRow(6), true);
	EXPECT_EQ(sh.checkRow(0), false);
	EXPECT_EQ(sh.checkCol(5), true);
	EXPECT_EQ(sh.checkCol(0), false);

	sh[0][0] = new NumberExpr(7);
	sh[1][0] = new CellRefExpr("a1", &sh);
	Sheet sh2(2,2);
	sh.copyTo(sh2);
	sh[0][0] = new NumberExpr(5);
	EXPECT_EQ(sh2[0][0]->eval(), 7);
	EXPECT_EQ(sh2[1][0]->eval(), 7);
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
	expr = Parser("$a1+ $sdf$345* (234 +sum($c4:d$34) )*2").parse();
	EXPECT_EQ(expr->show(), "($a1+(($sdf$345*(234+sum($c4:d$34)))*2))");
	delete expr;
	expr = Parser("a$1+ $sdf$345* (234 +sum($c4:c$4) )*$d$4").parse();
	EXPECT_EQ(expr->show(), "(a$1+(($sdf$345*(234+sum($c4:c$4)))*$d$4))");
	delete expr;
	Sheet sh(2,3,3);
	Parser("34+b2").parseTo(&sh, sh[0][0]);
	EXPECT_EQ(sh[0][0]->show(), "(34+b2)");
	EXPECT_EQ(sh[0][0]->eval(), 37);
}

TEST (Parser, parsingErrors){
	EXPECT_THROW(Parser("").parse(), syntax_error);
	EXPECT_THROW(Parser("a").parse(), syntax_error);
	EXPECT_THROW(Parser("a1s").parse(), syntax_error);
	EXPECT_THROW(Parser("$a").parse(), syntax_error);
	EXPECT_THROW(Parser("a$").parse(), syntax_error);
	EXPECT_THROW(Parser("$a$").parse(), syntax_error);
	EXPECT_THROW(Parser("$1").parse(), syntax_error);
	EXPECT_THROW(Parser("$1$").parse(), syntax_error);
	EXPECT_THROW(Parser("34+").parse(), syntax_error);
	EXPECT_THROW(Parser("/234").parse(), syntax_error);
	EXPECT_THROW(Parser("32+34*").parse(), syntax_error);
	EXPECT_THROW(Parser("hah(a1:b2)").parse(), syntax_error);
	EXPECT_THROW(Parser("sum($a1:b$)").parse(), syntax_error);
	EXPECT_THROW(Parser("sum(a1)").parse(), syntax_error);
	EXPECT_THROW(Parser("sum(a1:)").parse(), syntax_error);
	EXPECT_THROW(Parser("sum(a1:b2").parse(), syntax_error);
	EXPECT_THROW(Parser("12* 34 + ((12 +1)").parse(), syntax_error);
}

TEST (Parser, evalErrors){
	Expression* expr = Parser("a4").parse();
	EXPECT_THROW(expr->eval(), eval_error);
	delete expr;

	Sheet(1, 1);
	Parser("a1").parseTo(&sh, sh[0][0]);
	EXPECT_THROW(sh[0][0].evalMe(), eval_error);
	Parser("asd1").parseTo(&sh, sh[0][0]);
	EXPECT_THROW(sh[0][0].evalMe(), eval_error);
	Parser("a10").parseTo(&sh, sh[0][0]);
	EXPECT_THROW(sh[0][0].evalMe(), std::runtime_error);
}

TEST (Console, functions){
	std::stringstream oss, iss;
	Console con(oss, iss);
	iss << "10 10 "; con.createNew();
	iss << "a1 1 "; con.set();
	iss << "b1 a1/2 "; con.set();
	iss << "a2 a1+2 "; con.set();
	iss << "a2 b10 "; con.pull();
	iss << "b10 "; con.show();
	EXPECT_EQ(oss.str(), "(b9+2) = 18.5\n");
	oss.str("");
	iss << "a1 4 "; con.set();
	iss << "b10 "; con.show();
	EXPECT_EQ(oss.str(), "(b9+2) = 20\n");
	oss.str("");
	iss << "d1 sum($a$1:a2) "; con.set();
	iss << "d1 e10 "; con.pull();
	iss << "d1 "; con.show();
	iss << "e9 "; con.show();
	iss << "e10 "; con.show();
	EXPECT_EQ(oss.str(),
		"sum($a$1:a2) = 10\nsum($a$1:b10) = 240\nsum($a$1:b11) = evaluation error: index out of range\n");
	oss.str("");
	con.exit();
	EXPECT_EQ(con.isClosed(), true);
}

TEST (Console, readCommand){
	std::stringstream oss, iss;
	Console con(oss, iss);
	iss << "new 5 5 set a1 3 set b1 4 show a1 show b1 ";
	for (int i = 0; i < 5; i++) {con.readCommand();}
	EXPECT_EQ(oss.str(), "3 = 3\n4 = 4\n");
	oss.str("");

	iss  << "set a2 sum(a1:b2) show a2 ";
	for (int i = 0; i < 2; i++) {con.readCommand();}
	EXPECT_EQ(oss.str(), "sum(a1:b2) = evaluation error: cyclic reference\n");
	oss.str("");

	iss  << "pull a1 b1 show b1 ";
	for (int i = 0; i < 2; i++) {con.readCommand();}
	EXPECT_EQ(oss.str(), "3 = 3\n");
	oss.str("");

	iss  << "resize 1 1 print";
	for (int i = 0; i < 2; i++) {con.readCommand();}
	EXPECT_EQ(oss.str(), "  a\t\n1|3\t\n");
	oss.str("");
}

TEST (Console, fileManagement){
	std::stringstream oss1, iss1, oss2, iss2;
	Console con1(oss1, iss1);
	Console con2(oss2, iss2);
	iss1 << "3 10 "; con1.createNew();
	iss1 << "a1 1 "; con1.set();
	iss1 << "b1 a1/2 "; con1.set();
	iss1 << "a2 a1+2 "; con1.set();
	iss1 << "a2 b10 "; con1.pull();
	iss1 << "file_test "; con1.save();
	iss2 << "file_test "; con2.load();
	con2.print();
	con1.print();
	EXPECT_EQ(oss1.str(), oss2.str());
	iss1 << "b4 "; con1.show();
	iss2 << "b4 "; con2.show();
	EXPECT_EQ(oss1.str(), oss2.str());
	oss1.str("");
	oss2.str("");
	iss1 << "file_test "; con1.exportValues();
	iss2 << "file_test "; con2.load();
	con2.print();
	con1.print();
	EXPECT_EQ(oss1.str(), oss2.str());
	iss1 << "b4 "; con1.show();
	iss2 << "b4 "; con2.show();
	EXPECT_NE(oss1.str(), oss2.str());
}

TEST (Deleting, deleting){
	delete a1;
	delete b3;
	delete c2;
}
