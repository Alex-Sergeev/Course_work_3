#include  <math.h>

struct dot
{
	int x;
	int y;

	const bool operator < (const dot &d2)
	{
		if (d2.y*this->x - d2.x*this->y > 0)
			return true;
		else if (d2.y*this->x - d2.x*this->y == 0)
			if (sqrt(pow(this->y, 2) + pow(this->x, 2)) < sqrt(pow(d2.y, 2) + pow(d2.x, 2)))
				return true;
		return false;
	}

	const bool operator==(const dot &d2)
	{
		if ((this->x == d2.x) && (this->y == d2.y))
			return true;
		return false;
	}

	dot operator + (const dot &d2)
	{
		dot result;
		result.x = this->x + d2.x;
		result.y = this->y + d2.y;
		return result;
	}
	dot operator - (const dot &d2)
	{
		dot result;
		result.x = this->x - d2.x;
		result.y = this->y - d2.y;
		return result;
	}
};

static const bool operator==(const dot &d1, const dot &d2)
{
	if ((d1.x == d2.x) && (d1.y == d2.y))
		return true;
	return false;
}

static const bool operator!=(const dot &d1, const dot &d2)
{
	return !(d1 == d2);
}


static const bool operator < (const dot &d1, const dot &d2)
{
	if (d2.y*d1.x - d2.x*d1.y > 0)
		return true;
	else if (d2.y*d1.x - d2.x*d1.y == 0)
		if (sqrt(pow(d1.y, 2) + pow(d1.x, 2)) < sqrt(pow(d2.y, 2) + pow(d2.x, 2)))
			return true;
	return false;
}
