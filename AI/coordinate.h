#pragma once
#include <iostream>




struct pointOwner {
	POINT a;			// point owner
	POINT arr[100];	// exist a path from a to each arr[i]
	int n;
	void _init(POINT b) {
		a = b;
		n = 0;

	}
};
struct MyWeight
{
	int index;
	float weight;
	bool operator < (const struct MyWeight& str) const
	{
		return (weight < str.weight);
	}
};