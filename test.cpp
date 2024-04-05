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
	CellRefExpr empty("a", 6);
	EXPECT_THROW(empty.eval(), const char*);
	Expression* np = new NumberExpr(5);
	ExprPointer ep = np;
	CellRefExpr c (std::string("b"), 3, &ep);
	EXPECT_EQ(c.eval(), 5);
	EXPECT_EQ(c.show(), "b3");
	EXPECT_EQ(c.getCol(), "b");
	EXPECT_EQ(c.getRow(), 3);
	EXPECT_EQ(c.getContent(), &ep);
	Expression* cc = c.copy();
	EXPECT_EQ(cc->show(), "b3");
	EXPECT_NO_THROW(c.checkCyclic({cc}));
	EXPECT_THROW(c.checkCyclic({np}), const char*);
	EXPECT_THROW(cc->checkCyclic({np}), const char*);
	delete cc;
}

TEST (Expression, Range){
	Sheet sh(3,3); //cant test range independently
	CellRefExpr* a1 = new CellRefExpr("a", 1, &(sh[0][0]));
	CellRefExpr* b3 = new CellRefExpr("b", 3, &(sh[1][2]));
	CellRefExpr* c2 = new CellRefExpr("c", 2, &(sh[2][1]));

	Range r1(a1->copy(), a1->copy(), 3);
	r1.beginIter();
	EXPECT_EQ(r1.next(), a1->getContent());
	EXPECT_EQ(r1.next(), (ExprPointer*)NULL);

	Range r2(a1->copy(), b3->copy(), 3);
	EXPECT_EQ(r2.show(), "a1:b3");
	r2.beginIter();
	int db = 0;
	while (r2.next()!=NULL) {db++;}
	EXPECT_EQ(db, 6);

	Range r3(c2->copy(), a1->copy(), 3);
	EXPECT_EQ(r3.show(), "a1:c2");
	r3.beginIter();
	db = 0;
	while (r3.next()!=NULL) {db++;}
	EXPECT_EQ(db, 6);

	Range r4(b3->copy(), c2->copy(), 3);
	EXPECT_EQ(r4.show(), "b2:c3");
	r4.beginIter();
	db = 0;
	ExprPointer* c;
	sh[1][1] = new NumberExpr(3);
	sh[1][2] = new NumberExpr(4);
	sh[2][1] = new NumberExpr(5);
	sh[2][2] = new NumberExpr(6);
	while ((c = r4.next())) {std::cout << c->evalMe(); db++;}
	EXPECT_EQ(db, 4);

	delete a1;
	delete b3;
	delete c2;
}
