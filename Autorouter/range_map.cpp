#include "range_map.h"

range_map::range_map(const range_map &rangemap)
{
	range_map_width_ = rangemap.range_map_width_;
	range_map_height_ = rangemap.range_map_height_;
	range_map_.resize(range_map_width_*range_map_height_);
	range_map_fill_ball_cache_.resize((RANGEMAP_MAX_SIZE * 2 + 1) * (RANGEMAP_MAX_SIZE * 2 + 1));
	for (unsigned int i = 0; i < range_map_width_*range_map_height_; i++)
		range_map_[i] = rangemap.range_map_[i];
	for (unsigned int i = 0; i < (RANGEMAP_MAX_SIZE * 2 + 1) * (RANGEMAP_MAX_SIZE * 2 + 1); i++)
		range_map_fill_ball_cache_[i] = rangemap.range_map_fill_ball_cache_[i];
}

range_map::range_map(const unsigned short range_map_width, const unsigned short range_map_height, double dxy) :range_map_width_(range_map_width), range_map_height_(range_map_height)
{
	//range_map_.resize(range_map_width * range_map_height);
	range_map_.assign(range_map_width * range_map_height, 9999);
	//std::fill_n(range_map_, range_map_width * range_map_height, 9999);
	// cache z odleglosciami - zmienna globalna + inicjalizacja
	range_map_fill_ball_cache_.resize((RANGEMAP_MAX_SIZE * 2 + 1) * (RANGEMAP_MAX_SIZE * 2 + 1));
	//range_map_fill_ball_cache_ = new unsigned short[(RANGEMAP_MAX_SIZE * 2 + 1) * (RANGEMAP_MAX_SIZE * 2 + 1)];

	int pos = 0;
	const short max_real_value = RANGEMAP_VALUE_MUL * static_cast<int>(sqrt(RANGEMAP_MAX_SIZE * RANGEMAP_MAX_SIZE * 2)) + 1;
	for (int y = -RANGEMAP_MAX_SIZE; y <= RANGEMAP_MAX_SIZE; ++y)
	{
		for (int x = -RANGEMAP_MAX_SIZE; x <= RANGEMAP_MAX_SIZE; ++x)
		{
			unsigned short realvalue = RANGEMAP_VALUE_MUL * sqrt(x * x + y * y);
			//if (realvalue<0) realvalue = 0;

			if (realvalue > max_real_value) realvalue = max_real_value; //sqrt(384*384+384*384)*5
			range_map_fill_ball_cache_[pos] = realvalue;
			++pos;
		};
	}

	////dodanie rangemapy od krawedzi

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od górnej
	{
		for (unsigned int j = i; j < range_map_width - i; ++j)
		{
			range_map_[i * range_map_width + j] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od dolnej
	{
		for (unsigned int j = i; j < range_map_width - i; ++j)
		{
			range_map_[(range_map_height - 1 - i) * range_map_width + j] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od lewej
	{
		for (unsigned int j = i; j < range_map_height - i; ++j)
		{
			range_map_[range_map_width * j + i] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od prawej
	{
		for (unsigned int j = i; j < range_map_height - i; ++j)
		{
			range_map_[range_map_width * (j + 1) - 1 - i] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}
	////koniec dodanie rangemapy od krawedzi
};


void range_map::clear_range_map()
{
	range_map_.assign(range_map_width_ * range_map_height_, 9999);
	// cache z odleglosciami - zmienna globalna + inicjalizacja

	////dodanie rangemapy od krawedzi

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od górnej
	{
		for (unsigned int j = i; j < range_map_width_ - i; ++j)
		{
			range_map_[i * range_map_width_ + j] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od dolnej
	{
		for (unsigned int j = i; j < range_map_width_ - i; ++j)
		{
			range_map_[(range_map_height_ - 1 - i) * range_map_width_ + j] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od lewej
	{
		for (unsigned int j = i; j < range_map_height_ - i; ++j)
		{
			range_map_[range_map_width_ * j + i] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}

	for (int i = 0; i < RANGEMAP_MAX_SIZE / 2; ++i) //od prawej
	{
		for (unsigned int j = i; j < range_map_height_ - i; ++j)
		{
			range_map_[range_map_width_ * (j + 1) - 1 - i] = (i + 1) * RANGEMAP_VALUE_MUL;
		}
	}
};

unsigned short range_map::get_range_map_value(const unsigned int x_y)
{
	return range_map_[x_y];
};

void range_map::print_to_file(const std::string& filename) const
{
	std::ofstream myfile(filename);
	myfile << "RangeMap" << std::endl;
	for (int i = 0; i < range_map_height_; ++i)
	{
		for (int j = 0; j < range_map_width_; ++j)
		{
			myfile << range_map_[i * range_map_width_ + j] << " ";
		}
		myfile << std::endl;
	}
	myfile.close();
};

void range_map::range_map_fill_ball(const unsigned short x0, const unsigned short y0, const unsigned short radius)
{

	int minx = x0 - RANGEMAP_MAX_SIZE_SIZE;
	if (minx < 0) minx = 0;
	int maxx = x0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	int miny = y0 - RANGEMAP_MAX_SIZE_SIZE;
	if (miny < 0) miny = 0;
	int maxy = y0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	const int len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		//vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + y * range_map_width_ + minx;

		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE);

		for (int i = 0; i < len; ++i)
		{
			unsigned short cache_value;
			if (*range_map_cache_line  < RANGEMAP_VALUE_MUL * (radius)) cache_value = 0;
			else cache_value = *range_map_cache_line - RANGEMAP_VALUE_MUL * (radius);


			if (*range_map_line > cache_value)
				*range_map_line = cache_value;
			++range_map_line;
			++range_map_cache_line;
			//*range_map_line = Min(*range_map_line, *range_map_cache_line );
		}
	};
};

void range_map::range_map_fill_rect(const unsigned short x0, const unsigned short y0, const unsigned short width, const unsigned short height)
{
	//1kw
	int minx = x0 - RANGEMAP_MAX_SIZE_SIZE;
	if (minx < 0) minx = 0;
	int maxx = x0 - width / 2;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	int miny = y0 - RANGEMAP_MAX_SIZE_SIZE;
	if (miny < 0) miny = 0;
	int maxy = y0 - height / 2;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	int len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE + height / 2) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE + width / 2);

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > *range_map_cache_line)
				*range_map_line = *range_map_cache_line;
			++range_map_line;
			++range_map_cache_line;
		}
	};

	//3kw
	minx = x0 + width / 2;
	if (minx < 0) minx = 0;
	maxx = x0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 - RANGEMAP_MAX_SIZE_SIZE;
	if (miny < 0) miny = 0;
	maxy = y0 - height / 2;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE + height / 2) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE - width / 2);

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > *range_map_cache_line)
				*range_map_line = *range_map_cache_line;
			++range_map_line;
			++range_map_cache_line;
		}
	};

	//7kw
	minx = x0 - RANGEMAP_MAX_SIZE_SIZE;
	if (minx < 0) minx = 0;
	maxx = x0 - width / 2;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 + height / 2;
	if (miny < 0) miny = 0;
	maxy = y0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE - height / 2) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE + width / 2);

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > *range_map_cache_line)
				*range_map_line = *range_map_cache_line;
			++range_map_line;
			++range_map_cache_line;
		}
	};

	//9kw
	minx = x0 + width / 2;
	if (minx < 0) minx = 0;
	maxx = x0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 + height / 2;
	if (miny < 0) miny = 0;
	maxy = y0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE - height / 2) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE - width / 2);

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > *range_map_cache_line)
				*range_map_line = *range_map_cache_line;
			++range_map_line;
			++range_map_cache_line;
		}
	};

	//5kw
	minx = x0 - width / 2;
	if (minx < 0) minx = 0;
	maxx = x0 + width / 2;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 - height / 2;
	if (miny < 0) miny = 0;
	maxy = y0 + height / 2;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + RANGEMAP_MAX_SIZE_SIZE * (RANGEMAP_MAX_SIZE_SIZE *
			2 + 1) + RANGEMAP_MAX_SIZE_SIZE;

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > range_map_cache_line[0])
				*range_map_line = range_map_cache_line[0];
			++range_map_line;
			//++range_map_cache_line;
		}
	};

	//2kw
	minx = x0 - width / 2;
	if (minx < 0) minx = 0;
	maxx = x0 + width / 2;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 - RANGEMAP_MAX_SIZE_SIZE;
	if (miny < 0) miny = 0;
	maxy = y0 - height / 2;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE + height / 2) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + RANGEMAP_MAX_SIZE_SIZE;

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > range_map_cache_line[0])
				*range_map_line = range_map_cache_line[0];
			++range_map_line;
			//++range_map_cache_line;
		}
	};

	//4kw
	minx = x0 - RANGEMAP_MAX_SIZE_SIZE;
	if (minx < 0) minx = 0;
	maxx = x0 - width / 2;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 - height / 2;
	if (miny < 0) miny = 0;
	maxy = y0 + height / 2;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + RANGEMAP_MAX_SIZE_SIZE * (RANGEMAP_MAX_SIZE_SIZE *
			2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE + width / 2);

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > *range_map_cache_line)
				*range_map_line = *range_map_cache_line;
			++range_map_line;
			++range_map_cache_line;
		}
	};

	//6kw
	minx = x0 + width / 2;
	if (minx < 0) minx = 0;
	maxx = x0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 - height / 2;
	if (miny < 0) miny = 0;
	maxy = y0 + height / 2;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + RANGEMAP_MAX_SIZE_SIZE * (RANGEMAP_MAX_SIZE_SIZE *
			2 + 1) + (minx - x0 + RANGEMAP_MAX_SIZE_SIZE - width / 2);

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > *range_map_cache_line)
				*range_map_line = *range_map_cache_line;
			++range_map_line;
			++range_map_cache_line;
		}
	};

	//8kw
	minx = x0 - width / 2;
	if (minx < 0) minx = 0;
	maxx = x0 + width / 2;
	if (maxx >= range_map_width_) maxx = range_map_width_ - 1;
	miny = y0 + height / 2;
	if (miny < 0) miny = 0;
	maxy = y0 + RANGEMAP_MAX_SIZE_SIZE;
	if (maxy >= range_map_height_) maxy = range_map_height_ - 1;
	len = maxx - minx + 1;
	for (int y = miny; y <= maxy; ++y)
	{
		vector<unsigned short>::iterator range_map_line = range_map_.begin() + y * range_map_width_ + minx;
		vector<unsigned short>::iterator range_map_cache_line = range_map_fill_ball_cache_.begin() + (y - y0 + RANGEMAP_MAX_SIZE_SIZE - height / 2) * (
			RANGEMAP_MAX_SIZE_SIZE * 2 + 1) + RANGEMAP_MAX_SIZE_SIZE;

		for (int i = 0; i < len; ++i)
		{
			if (*range_map_line > range_map_cache_line[0])
				*range_map_line = range_map_cache_line[0];
			++range_map_line;
			//++range_map_cache_line;
		}
	};
}

void range_map::range_map_fill_oval(const unsigned short x0, const unsigned short y0, const unsigned short width, const unsigned short height)
{
	if (width > height)
	{
		range_map_fill_rect(x0, y0, width - height, height);
		range_map_fill_ball(x0 - (width - height) / 2, y0, height / 2);
		range_map_fill_ball(x0 + (width - height) / 2, y0, height / 2);
	}
	else
	{
		range_map_fill_rect(x0, y0, width, height - width);
		range_map_fill_ball(x0, y0 - (height - width) / 2, width / 2);
		range_map_fill_ball(x0, y0 + (height - width) / 2, width / 2);
	}
}


