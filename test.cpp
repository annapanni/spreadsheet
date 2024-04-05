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

TEST (Expression, Range){
	Sheet sh(3,3); //cant test range independently
	CellRefExpr* a1 = new CellRefExpr("a1", &sh);
	CellRefExpr* b3 = new CellRefExpr("b3", &sh);
	CellRefExpr* c2 = new CellRefExpr("c2", &sh);

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

	delete a1;
	delete b3;
	delete c2;
}
