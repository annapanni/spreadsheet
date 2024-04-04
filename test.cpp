#include <gtest/gtest.h>

TEST(Proba, osszead){
	EXPECT_EQ(2+3, 5);
	EXPECT_EQ(2+3, 5);
	EXPECT_EQ(2+3, 5);
	EXPECT_EQ(2+3, 7);
}

TEST(Proba, kivon){
	EXPECT_EQ(2-1, 1);
}

TEST(Proba, szoroz){
	EXPECT_EQ(2*3, 6);
	EXPECT_EQ(2*3, 8);
}
