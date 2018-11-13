#include "path.h"

path::path(): ui_path_width_(0), ui_clearance_(0), ui_board_width_(0), ui_board_height_(0), ui_via_diam_(0)
{
}

path::path(point* current, std::vector<point*>& came_from, const unsigned int path_width, const unsigned int via_diam,
           const unsigned int board_width, const unsigned int board_height,
           const unsigned int clearance) :
	ui_path_width_(path_width), ui_clearance_(clearance), ui_board_width_(board_width), ui_board_height_(board_height),
	ui_via_diam_(via_diam)
{
	point* curr = current;
	while (curr != nullptr)
	{
		path_vector_.push_back(curr);
		const unsigned int index = curr->get_z() * board_height * board_width + curr->get_y() * board_width + curr->get_x();
		curr = came_from[index];
	}
}

path::~path()
{
	while (!path_vector_.empty())
		path_vector_.pop_back();
}

void path::check_vias(std::vector<bool>& vias_tab)
{
	point* curr = *(path_vector_.begin());

	for (auto it = path_vector_.begin(); it != path_vector_.end(); ++it)
	{
		point* prev = curr;
		curr = *it;

		if (curr->get_z() != prev->get_z())
		{
			int k1, k2;
			if (curr->get_z() < prev->get_z())
			{
				k1 = curr->get_z();
				k2 = prev->get_z();
			}
			else
			{
				k2 = curr->get_z();
				k1 = prev->get_z();
			}
			for (k1; k1 <= k2; ++k1)
				vias_tab[k1 * ui_board_height_ * ui_board_width_ + curr->get_y() * ui_board_width_ + curr->get_x()] = true;
		}
	}
}

std::vector<point*>* path::get_path_pointer()
{
	return &path_vector_;
}

bool path::empty() const
{
	return path_vector_.empty();
}

unsigned path::get_path_width() const
{
	return ui_path_width_;
}

unsigned path::get_via_diam() const
{
	return ui_via_diam_;
}

unsigned path::get_clearance() const
{
	return ui_clearance_;
}
