#pragma once

#include <fstream>
#include <vector>

//wybór siatki
#define RANGEMAP_MAX_SIZE 39// maksymalny promien kulki + maksymalny przeœwit (siatka 0.1 mm) 
#define RANGEMAP_MAX_SIZE_SIZE 39  // maksymalny promien kulki + maksymalny przeœwit (siatka 0.1 mm)
/*
#define RANGEMAP_MAX_SIZE 156  // maksymalny promien kulki + maksymalny przeœwit (siatka 0.025 mm) 
#define RANGEMAP_MAX_SIZE_SIZE 156 // maksymalny promien kulki + maksymalny przeœwit (siatka 0.025 mm) 
*/

#define RANGEMAP_VALUE_MUL 5
using namespace std;

class range_map
{
private:
	unsigned short range_map_width_=0;
	unsigned short range_map_height_=0;
	vector<unsigned short> range_map_;
	vector<unsigned short> range_map_fill_ball_cache_;
	
public:
	range_map() {};
	~range_map(){};
	range_map(const range_map &rangemap);
	range_map(const unsigned short range_map_width, const unsigned short range_map_height, double dxy);
	
	void clear_range_map();
	unsigned short get_range_map_value(const unsigned int x_y);

	void print_to_file(const std::string& filename) const;

	void range_map_fill_ball(const unsigned short x0, const unsigned short y0, const unsigned short radius);
	void range_map_fill_rect(const unsigned short x0, const unsigned short y0, const unsigned short width, const unsigned short height);
	void range_map_fill_oval(const unsigned short x0, const unsigned short y0, const unsigned short width, const unsigned short height);
};



