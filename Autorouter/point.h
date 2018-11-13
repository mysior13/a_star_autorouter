#pragma once

class point
{
private:
	unsigned short x_ = 0, y_ = 0, z_ = 0;
	unsigned int f_score_ = 0;
	unsigned int g_score_ = 0;
public:
	point();

	point(unsigned short x_pos, unsigned short y_pos, unsigned short z_pos, unsigned int val = 0,
	      unsigned int g_scr = 0);

	short get_x() const { return x_; }
	short get_y() const { return y_; }
	short get_z() const { return z_; }

	unsigned int get_f_score() const;
	unsigned int get_g_score() const;
	void set_f_score(unsigned int v);
	void set_g_score(unsigned int v);

	double dist(const point* p2) const;

	bool operator==(const point* p2) const;
};
