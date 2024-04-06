#include <gtest/gtest.h>
#include <string>

#include "expression.hpp"
#include "sheet.hpp"

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
	CellRefExpr empty("a6");
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
}

Sheet sh(3,3, 5);
CellRefExpr* a1 = new CellRefExpr("a1", &sh);
CellRefExpr* b3 = new CellRefExpr("b3", &sh);
CellRefExpr* c2 = new CellRefExpr("c2", &sh);

TEST (Expression, Range){
	Range r1(a1->copy(), a1->copy());
	r1.beginIter();
	EXPECT_EQ(r1.next(), a1->getPtr());
	EXPECT_EQ(r1.next(), (ExprPointer*)NULL);

	Range r2(a1->copy(), b3->copy());
	EXPECT_EQ(r2.show(), "a1:b3");
	r2.beginIter();
	int db = 0;
	while (r2.next()!=NULL) {db++;}
	EXPECT_EQ(db, 6);

	Range r3(c2->copy(), a1->copy());
	EXPECT_EQ(r3.show(), "a1:c2");
	r3.beginIter();
	db = 0;
	while (r3.next()!=NULL) {db++;}
	EXPECT_EQ(db, 6);

	Range r4(b3->copy(), c2->copy());
	EXPECT_EQ(r4.show(), "b2:c3");
	r4.beginIter();
	db = 0;
	while (r4.next()) {db++;}
	EXPECT_EQ(db, 4);
}

TEST (Expression, Function){
	SumFunc sum = SumFunc(a1->copy(), b3->copy());
	EXPECT_THROW(sum.checkCyclic({**(a1->getPtr()+1)}), const char*);
	EXPECT_EQ(sum.eval(), 30);
	FunctionExpr* avg = newFunctionExpr(AVG, c2->copy(), a1->copy());
	EXPECT_NO_THROW(avg->checkCyclic({**(b3->getPtr())}));
	EXPECT_EQ(avg->eval(), 5);
	EXPECT_EQ(avg->show(), "avg(a1:c2)");
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
	delete opcpy;
}

TEST (Sheet, statics){
	EXPECT_EQ(Sheet::colLetter(4), "d");
	EXPECT_EQ(Sheet::colLetter(27), "aa");
	EXPECT_EQ(Sheet::colNumber("e"), 5);
	EXPECT_EQ(Sheet::colNumber("ab"), 28);
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


TEST (Deleting, deleting){
	delete a1;
	delete b3;
	delete c2;
}
