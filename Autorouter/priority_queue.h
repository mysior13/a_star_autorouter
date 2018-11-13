#pragma once

#include "point.h"
#include <vector>

using namespace std;

struct porownaj_punkty
{
	bool operator()(const point* lhs, const point* rhs) const
	{
		if (lhs->get_f_score() > rhs->get_f_score())
			return true;
		if (lhs->get_f_score() < rhs->get_f_score())
			return false;
		
		if (lhs->get_g_score() < rhs->get_g_score())
			return true;
		/*if (lhs->get_g_score() > rhs->get_g_score())
			return false;
		if (lhs->get_direction() < rhs->get_direction())
			return true;*/
		else return false;
		
	}
};


class priority_queue
{
public:
	priority_queue();
	priority_queue(unsigned int szer, unsigned int wys, unsigned int l_warst);
	~priority_queue();
	void push(point*); //funkcja dodaj¹ca proces do kolejki
	point* front(); //funkcja pobieraj¹ca z kolejki proces o najwiêkszym priorytecie(nie usuwa go)
	point* pop(); //funkcja usuwaj¹ca z kolejki proces o najwiêkszym priorytecie
	void resort(point* p1, unsigned int fScore, unsigned int gScore);
	bool is_in_queue(point*) const;
	bool empty() const;
private:
	vector<point*> point_vector_;
	vector<bool> in_queue_tab_;
	unsigned int width_;
	unsigned int height_;
	unsigned int layer_;
};
