#pragma once

#include "point.h"
#include <vector>

class path
{
private:
	unsigned int ui_path_width_;
	unsigned int ui_clearance_;
	unsigned int ui_board_width_;
	unsigned int ui_board_height_;
	unsigned int ui_via_diam_;
	std::vector<point*> path_vector_;
public:
	path();
	path(point* current, std::vector<point*>& came_from, unsigned int path_width, unsigned int via_diam,
	     unsigned int board_width, unsigned int board_height, unsigned int clearance);
	~path();

	bool empty() const;
	void check_vias(std::vector<bool>& vias_tab);

	std::vector<point*>* get_path_pointer();
	unsigned int get_path_width() const;
	unsigned int get_via_diam() const;
	unsigned int get_clearance() const;
};
