#include "priority_queue.h"
#include <algorithm>
#include <iterator>
#include <vector>

priority_queue::priority_queue() = default;

priority_queue::priority_queue(const unsigned int szer, const unsigned int wys, const unsigned int l_wastw): width_(szer), height_(wys), layer_(l_wastw)
{
	unsigned int const rozmiar = width_ * height_*layer_;
	in_queue_tab_.resize(rozmiar);
	for (unsigned int i = 0; i < rozmiar; ++i)
	{
		in_queue_tab_[i] = false;
	}
}

priority_queue::~priority_queue()
{
	//while (!point_vector_.empty())
		//point_vector_.pop_back();
	//delete in_queue_tab_;
}

void priority_queue::push(point* p1)
{
	in_queue_tab_[p1->get_z()*height_*width_+p1->get_y() * width_ + p1->get_x()] = true;
	const auto it = std::lower_bound(point_vector_.begin(), point_vector_.end(), p1, porownaj_punkty());
	point_vector_.insert(it, p1);
}

point* priority_queue::front()
{
	if (point_vector_.empty())return nullptr;
	return point_vector_.back();
}

point* priority_queue::pop()
{
	if (point_vector_.empty())return nullptr;

	point* p1 = point_vector_.back();
	in_queue_tab_[p1->get_z()*height_*width_ + p1->get_y() * width_ + p1->get_x()] = false;
	point_vector_.pop_back();
	return p1;
}

bool priority_queue::is_in_queue(point* p1) const
{
	if (!point_vector_.empty())
	{
		//unsigned int const index = p1->get_y() * width_ + p1->get_x();
		return in_queue_tab_[p1->get_z()*height_*width_ + p1->get_y() * width_ + p1->get_x()];
	}
	return false;
};

bool priority_queue::empty() const
{
	return point_vector_.empty();
};

void priority_queue::resort(point* p1, unsigned int fScore, unsigned int gScore)
{
	auto iter = std::lower_bound(point_vector_.begin(), point_vector_.end(), p1, porownaj_punkty());
	while (p1 != *iter)
		++iter;

	point_vector_.erase(iter);

	p1->set_f_score(fScore);
	p1->set_g_score(gScore);
	const auto it = std::lower_bound(point_vector_.begin(), point_vector_.end(), p1, porownaj_punkty());
	point_vector_.insert(it, p1);
}
