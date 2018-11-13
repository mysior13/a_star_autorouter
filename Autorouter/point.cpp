#include "point.h"
//#define layer_dist 1.5

point::point()
= default;

point::point(const unsigned short x_pos, const unsigned short y_pos, const unsigned short z_pos, const unsigned int val,
             const unsigned int g_scr) : x_(x_pos), y_(y_pos), z_(z_pos), f_score_(val), g_score_(g_scr)
{
}

unsigned point::get_f_score() const
{
	return f_score_;
}

unsigned point::get_g_score() const
{
	return g_score_;
}

void point::set_f_score(const unsigned int v)
{
	f_score_ = v;
}

void point::set_g_score(const unsigned int v)
{
	g_score_ = v;
}

double point::dist(const point* p2) const
{
	unsigned int xd;
	if (x_ > p2->x_)
		xd = x_ - p2->x_;
	else xd = p2->x_ - x_;

	unsigned int yd;
	if (y_ > p2->y_)
		yd = y_ - p2->y_;
	else yd = p2->y_ - y_;

	unsigned int straight, diagonally;

	if (xd < yd)
	{
		diagonally = xd;
		straight = yd - xd;
	}
	else
	{
		diagonally = yd;
		straight = xd - yd;
	}
	return diagonally * 1.4 + straight + 0.1; //+0.1 - strata przy rzutowaniu
}

bool point::operator==(const point* p2) const
{
	return (this == p2);
}
