#pragma once

#include <string>
#include <vector>
#include <algorithm>

struct net
{
	std::string name;
	double clearance;
	double trace_width;
	double via_dia;
	double uvia_dia;

	unsigned int ui_clearance;
	unsigned int ui_trace_width;
	unsigned int ui_via_dia;
	unsigned int ui_uvia_dia;
};

struct pad
{
	double x, y;
	unsigned int layer;
	double width, height;
	unsigned int ui_x, ui_y;
	unsigned int ui_width, ui_height;

	bool is_oval;
	bool is_smd;
	net* net;
};

class pcb_param
{
private:
	double grid_{};

	int links_nr_ = 0; //raczej niepotrzebne
	int nets_nr_ = 0;
	int modules_nr_ = 0;
	int layers_nr_ = 0;

	double board_pos_x_ = 0;
	double board_pos_y_ = 0;
	double board_width_ = 0;
	double board_height_ = 0;
	double ui_board_width_ = 0;
	double ui_board_height_ = 0;

	std::vector<net> nets_tab_;
	std::vector<pad> pads_tab_;
public:
	pcb_param();
	pcb_param(std::ifstream& myfile, double grid_value);		//w konstruktorze odczyt wartosci z pliku z programu KiCad
	~pcb_param();

	std::vector<pad>* get_pads_tab();							//przekazanie wskaznika na wektor padow
	void get_board_size(double& width, double& height) const;	//pobranie wymiarow plytki
	int get_layers_nr() const;									//pobranie liczby warstw
	unsigned int get_nets_nr() const;							//pobranie liczby sieci polaczen

	void print();												//wyswietlenie informacji o plytce
};
