#include "pcb_param.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

bool pads_compare(const pad p1, const pad p2)
{
	return p1.net->name < p2.net->name;
}

pcb_param::pcb_param()
= default;

pcb_param::pcb_param(ifstream& myfile, const double grid_value) : grid_(grid_value)
{
	string line;

	if (myfile.is_open())
	{
		double board_pos_x_max_ = 0;
		double board_pos_y_max_ = 0;

		while (line != "(gr_line")
			myfile >> line;;
		myfile >> line;
		myfile >> line;
		board_pos_x_ = stod(line);
		board_pos_x_max_= stod(line);

		myfile >> line;
		line.pop_back();
		board_pos_y_ = stod(line);
		board_pos_y_max_ = stod(line);

		myfile >> line;
		myfile >> line;
		if (stod(line) < board_pos_x_)board_pos_x_ = stod(line);
		if (stod(line) > board_pos_x_max_)board_pos_x_max_ = stod(line);

		myfile >> line;
		line.pop_back();
		if (stod(line) < board_pos_y_)board_pos_y_ = stod(line);
		if (stod(line) > board_pos_y_max_)board_pos_y_max_ = stod(line);

		while (!myfile.eof())
		{
			if (line == "(gr_line")
			{
				myfile >> line;
				myfile >> line;
				if (stod(line) < board_pos_x_)board_pos_x_ = stod(line);
				if (stod(line) > board_pos_x_max_)board_pos_x_max_ = stod(line);

				myfile >> line;
				line.pop_back();
				if (stod(line) < board_pos_y_)board_pos_y_ = stod(line);
				if (stod(line) > board_pos_y_max_)board_pos_y_max_ = stod(line);

				myfile >> line;
				myfile >> line;
				if (stod(line) < board_pos_x_)board_pos_x_ = stod(line);
				if (stod(line) > board_pos_x_max_)board_pos_x_max_ = stod(line);


				myfile >> line;
				line.pop_back();
				if (stod(line) < board_pos_y_)board_pos_y_ = stod(line);
				if (stod(line) > board_pos_y_max_)board_pos_y_max_ = stod(line);
			}
			else myfile >> line;
		}

		myfile >> line;
		board_width_ = board_pos_x_max_ - board_pos_x_;
		ui_board_width_ = static_cast<unsigned int>((board_width_ + 0.001) / grid_);
		myfile >> line;
		line.pop_back();
		board_height_ = board_pos_y_max_ - board_pos_y_;
		ui_board_height_ = static_cast<unsigned int>((board_height_ + 0.001) / grid_);
	}

	myfile.clear();
	myfile.seekg(0, myfile.beg);

	if (myfile.is_open())
	{
		do
			myfile >> line;
		while (line != "(links");
		myfile >> line;
		line.pop_back();
		links_nr_ = stoi(line);

		/*do
			myfile >> line;
		while (line != "(area");
		myfile >> line;
		board_pos_x_ = stod(line);
		myfile >> line;
		board_pos_y_ = stod(line);
		myfile >> line;
		board_width_ = stod(line) - board_pos_x_;
		ui_board_width_ = static_cast<unsigned int>((board_width_ + 0.001) / grid_);
		myfile >> line;
		line.pop_back();
		board_height_ = stod(line) - board_pos_y_;
		ui_board_height_ = static_cast<unsigned int>((board_height_ + 0.001) / grid_);*/

		do
			myfile >> line;
		while (line != "(modules");
		myfile >> line;
		line.pop_back();
		modules_nr_ = stoi(line);

		do
			myfile >> line;
		while (line != "(nets");
		myfile >> line;
		line.pop_back();
		nets_nr_ = stoi(line);

		do
			myfile >> line;
		while (line != "(layers");
		do
			myfile >> line;
		while (line != "(0");
		myfile >> line;
		const string front_layer_name = line;

		nets_tab_.resize(nets_nr_);

		while (line != "(net")
		{
			myfile >> line;
			if (line == "signal)")
				++layers_nr_;
		}

		myfile >> line;
		myfile >> line;
		line.pop_back();
		nets_tab_[0].name = line;

		for (int i = 1; i < nets_nr_; i++)
		{
			myfile >> line;
			myfile >> line;
			myfile >> line;
			line.pop_back();
			nets_tab_[i].name = line;
		}

		myfile >> line;
		while (!myfile.eof())
		{
			
			if (line == "(net_class")
			{
				while (line != "(clearance")
					myfile >> line;
				myfile >> line;
				line.pop_back();
				const double clearance = stod(line);

				while (line != "(trace_width")
					myfile >> line;
				myfile >> line;
				line.pop_back();
				const double trace_width = stod(line);

				while (line != "(via_dia")
					myfile >> line;
				myfile >> line;
				line.pop_back();
				const double via_dia = stod(line);

				while (line != "(uvia_dia")
					myfile >> line;
				myfile >> line;
				line.pop_back();
				const double uvia_dia = stod(line);

				myfile >> line;
				while (line != ")")
				{
					if (line == "(add_net")
					{
						myfile >> line;
						line.pop_back();

						for (int i = 0; i < nets_nr_; ++i)
						{
							if (line == nets_tab_[i].name)
							{
								nets_tab_[i].clearance = clearance;
								nets_tab_[i].trace_width = trace_width;
								nets_tab_[i].via_dia = via_dia;
								nets_tab_[i].uvia_dia = uvia_dia;

								nets_tab_[i].ui_clearance = static_cast<unsigned int>((clearance + 0.001) / grid_);
								nets_tab_[i].ui_trace_width = static_cast<unsigned int>((trace_width + 0.001) / grid_);
								nets_tab_[i].ui_via_dia = static_cast<unsigned int>((via_dia + 0.001) / grid_);
								nets_tab_[i].ui_uvia_dia = static_cast<unsigned int>((uvia_dia + 0.001) / grid_);
							}
						}
					}
					myfile >> line;
				}
			}

			if (line == "(module")
			{
				double module_pos_x, module_pos_y;
				int module_rotation = 0;

				while (line != "(layer")
					myfile >> line;
				myfile >> line;
				const bool is_top = (line == front_layer_name + ")");

				while (line != "(at")
					myfile >> line;

				myfile >> line;
				module_pos_x = stod(line);

				myfile >> line;
				if (line.back() == ')')
				{
					line.pop_back();
					module_pos_y = stod(line);
				}
				else
				{
					module_pos_y = stod(line);
					myfile >> line;
					line.pop_back();
					module_rotation = stoi(line);
				}

				while (line != "(module" && line != "(gr_line")
				{
					if (line == "(pad")
					{
						pad pad;
						pads_tab_.push_back(pad);

						if (is_top)
							pads_tab_.back().layer = 0;
						else
							pads_tab_.back().layer = layers_nr_ - 1;

						myfile >> line;
						if (line == "\"\"")
						{
							pads_tab_.pop_back();
							continue;
						}
						myfile >> line;

						pads_tab_.back().is_smd = (line == "smd");

						while ((line != "oval") && (line != "rect") && (line != "circle"))
							myfile >> line;

						if (line == "oval" || line == "circle")
							pads_tab_.back().is_oval = true;
						else
							pads_tab_.back().is_oval = false;

						while (line != "(at")
							myfile >> line;

						myfile >> line;
						const double pad_pos_x = stod(line);

						double pad_pos_y=0;
						short pad_rotation=0;
						myfile >> line;
						if (line.back() == ')')
						{
							line.pop_back();
							pad_pos_y = stod(line);
						}
						else
						{
							pad_pos_y = stod(line);
							myfile >> line;
							line.pop_back();
							pad_rotation= stod(line);
						}



						while (line != "(size")
							myfile >> line;

						myfile >> line;
						const double pad_width = stod(line);

						myfile >> line;
						line.pop_back();
						const double pad_height = stod(line);


						switch (module_rotation)
						{
						case 90:
							pads_tab_.back().x = pad_pos_y;
							pads_tab_.back().y = -pad_pos_x;
						
							break;
						case 180:
							pads_tab_.back().x = -pad_pos_x;
							pads_tab_.back().y = -pad_pos_y;
						
							break;
						case 270:
							pads_tab_.back().x = -pad_pos_y;
							pads_tab_.back().y = pad_pos_x;
						;
							break;
						default:
							pads_tab_.back().x = pad_pos_x;
							pads_tab_.back().y = pad_pos_y;
					
							break;
						}
						if (pad_rotation == 90 || pad_rotation == 270)
						{
							pads_tab_.back().width = pad_height;
							pads_tab_.back().height = pad_width;
						}
						else
						{
							pads_tab_.back().width = pad_width;
							pads_tab_.back().height = pad_height;
						}

						pads_tab_.back().ui_width = static_cast<unsigned int>((pads_tab_.back().width + 0.001) / grid_);
						pads_tab_.back().ui_height = static_cast<unsigned int>((pads_tab_.back().height + 0.001) / grid_);

						while (line != "(net")
							myfile >> line;
						myfile >> line;
						myfile >> line;
						line.pop_back();
						line.pop_back();

						for (int i = 0; i < nets_nr_; ++i)
						{
							if (line == nets_tab_[i].name)
							{
								pads_tab_.back().net = &nets_tab_[i];

								//wart bezwzgledna polozenia
								pads_tab_.back().x += module_pos_x;
								pads_tab_.back().y += module_pos_y;
								//wsp�rzedne wzgledem poczatku plytki
								pads_tab_.back().x -= board_pos_x_;
								pads_tab_.back().y -= board_pos_y_;

								pads_tab_.back().ui_x = static_cast<unsigned int>((pads_tab_.back().x + 0.001) / grid_);
								pads_tab_.back().ui_y = static_cast<unsigned int>((pads_tab_.back().y + 0.001) / grid_);
							}
						}
					}
					myfile >> line;
				}
			}
			else myfile >> line;
		}
		//po przejrzeniu calego pliku
		sort(pads_tab_.begin(), pads_tab_.end(), pads_compare);
	}
	else cout << "Unable to open file";
}

pcb_param::~pcb_param()
= default;

void pcb_param::print()
{
	cout << "Nr of layers: " << layers_nr_ << ", Nr of links: " << links_nr_ << ", Nr of nets: " << nets_nr_ << " " <<
		", Nr of modules: " << modules_nr_ <<
		endl;
	cout << "Board position: (" << board_pos_x_ << " " << board_pos_y_ << ") , width: " << board_width_ << ", height: " <<
		board_height_ << endl;

	cout << "\nNets:" << endl;
	for (int i = 0; i < nets_nr_; ++i)
	{
		cout << "  " << i << "\tname: " << left << setw(16) << nets_tab_[i].name;
		cout << "\ttrace width: " << nets_tab_[i].trace_width << "\tclearance: " << nets_tab_[i].clearance <<
			"\tvia diameter: " << nets_tab_[i].via_dia << "\tmicro via diameter: " << nets_tab_[i].uvia_dia << endl;
	}

	cout << "\nPads:" << endl;
	cout << "Nr of pads: " << pads_tab_.size() << endl;
	for (int i = 0; i < pads_tab_.size(); ++i)
	{
		cout << "Pad " << i;

		if (pads_tab_[i].is_smd)
			cout << "\tsmd";
		else
			cout << "\ttht";

		if (pads_tab_[i].is_oval)
			cout << "  oval";
		else
			cout << "  rect";

		cout << "  at: (" << pads_tab_[i].x << " " << pads_tab_[i].y << ")\twidth: " << pads_tab_[i].width << "    height: "
			<< pads_tab_[i].height;

		cout << "    layer: " << pads_tab_[i].layer << "    net: " << pads_tab_[i].net->name << endl;
	}
}

vector<pad>* pcb_param::get_pads_tab()
{
	vector<pad>* temp = &pads_tab_;
	return temp;
}

void pcb_param::get_board_size(double& width, double& height) const
{
	width = board_width_;
	height = board_height_;
}

int pcb_param::get_layers_nr() const
{
	return layers_nr_;
}

unsigned int pcb_param::get_nets_nr() const
{
	return nets_nr_;
}
