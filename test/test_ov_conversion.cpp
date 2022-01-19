#include <gtest/gtest.h>

#include "xeus-octave/utils.hpp"

TEST(OvConversionTest, Int) {
	const int t = 5;
	const octave_value v(5);
	int T;
	octave_value V;

	xeus_octave::utils::from_ov(v, T);
	xeus_octave::utils::to_ov(V, t);

	EXPECT_EQ(t, T);
	// EXPECT_EQ(v, V);
}
