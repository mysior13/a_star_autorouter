/*  W CELU EDYCJI ROZDZIELCZOSCI SIATKI ZMIENIC PARAMETRY dxy i layer_cost w main.cpp oraz RANGEMAP_MAX_SIZE i RANGEMAP_MAX_SIZE_SIZE w range_map.h   */
/*  PLIK WEJSCIOWY ZMIENIANY NA POCZATKU FUNKCJI main */
#include "priority_queue.h"
#include "pcb_param.h"
#include "lodepng.h"
#include "lodepng_custom.h"
#include "range_map.h"
#include "path.h"
#include "iostream"

//pomiar czasu
#include <chrono>
using  ns = chrono::nanoseconds;
using get_time = chrono::steady_clock;

using namespace std;
#define RANGEMAP_VALUE_MUL 5		//stala - koszt przejscia do sasiedniego wierzcholka


//wybór siatki
double dxy = 0.1; 					//rozmiar komorki siatki (w mm)
const int layer_cost = 500;			//koszt przejscia miedzy warstwami
/*
double dxy = 0.025; 					//rozmiar komorki siatki (w mm)
const int layer_cost = 2000;			//koszt przejscia miedzy warstwami
*/

double max_path_width = 2.56;		//maksymalna szerokosc sciezki
	
unsigned int inf = -1;				//wartosc nieskonczonosci

//funkcja dodajaca pady do mapy odleglosci
void pads_to_range_map(vector<pad>& pads, vector<range_map>& rangemap, const string& net_name="nullptr")
{
	for (unsigned int i = 0; i < pads.size(); i++)
	{
		if (!(pads[i].net->name == net_name) || (net_name == "nullptr"))
		{

			if (pads[i].is_oval)
			{
				if (pads[i].is_smd)
					rangemap[pads[i].layer].range_map_fill_oval(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width, pads[i].ui_height);
				
				else
				{
					for (int j = 0; j<rangemap.size(); ++j)
						rangemap[j].range_map_fill_oval(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width, pads[i].ui_height);
				}
			}

			else
			{
				if (pads[i].is_smd)
					rangemap[pads[i].layer].range_map_fill_rect(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width, pads[i].ui_height);

				else
				{
					for (int j = 0; j<rangemap.size(); ++j)
						rangemap[j].range_map_fill_rect(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width, pads[i].ui_height);
				}
			}
		}
	}
}

//funkcja dodajaca pady do mapy odleglosci (mapa przeswitu)
void pads_to_range_map_clearance(vector<pad>& pads, vector<range_map>& rangemap, const string& net_name = "nullptr")
{
	for (unsigned int i = 0; i < pads.size(); i++)
	{
		if (!(pads[i].net->name == net_name) || (net_name == "nullptr"))
		{
			const unsigned int clearance = pads[i].net->ui_clearance;
			if (pads[i].is_oval)
			{
				if (pads[i].is_smd)
					rangemap[pads[i].layer].range_map_fill_oval(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width+ clearance*2, pads[i].ui_height+ clearance * 2);

				else
				{
					for (int j = 0; j<rangemap.size(); ++j)
						rangemap[j].range_map_fill_oval(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width+ clearance * 2, pads[i].ui_height+ clearance * 2);
				}
			}

			else
			{
				if (pads[i].is_smd)
					rangemap[pads[i].layer].range_map_fill_rect(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width+ clearance * 2, pads[i].ui_height+ clearance * 2);

				else
				{
					for (int j = 0; j<rangemap.size(); ++j)
						rangemap[j].range_map_fill_rect(pads[i].ui_x, pads[i].ui_y, pads[i].ui_width+ clearance * 2, pads[i].ui_height+ clearance * 2);
				}
			}
		}
	}
}


int main()
{
	ifstream myfile("plytka_nr2.kicad_pcb");			    //wczytanie pliku z projektem p³ytki (inne plytki: plytka_nr1.kicad_pcb	  plytka_nr3.kicad_pcb)
	pcb_param my_pcb(myfile, dxy);							//zmienna przechowujaca parametry plytki
	my_pcb.print();											//wyswietlenie parametrow zwiazanych z plytka

	//odczyt wymiarow p³ytki
	unsigned int ui_layers_nr = my_pcb.get_layers_nr();		//liczba warstw plytki
	vector<pad>* pads_tab = my_pcb.get_pads_tab();			//wektor padow
	double d_width = 0, d_height = 0;						//wymiary plytki
	my_pcb.get_board_size(d_width, d_height);
	const unsigned int ui_width = static_cast<unsigned int>(d_width / dxy);
	const unsigned int ui_height = static_cast<unsigned int>(d_height / dxy);

	//mozliwosc zmiany liczby warstw
	char zmiana_l_warstw;
	cout << "\nCzy chcesz zmienic liczbe warstw? (aktualna liczba warstw = " << ui_layers_nr << ")  (t/n): ";
	cin >> zmiana_l_warstw;
	if(zmiana_l_warstw=='t')
	{
		cout << "\nPodaj nowa liczbe warstw: ";
		cin >> ui_layers_nr;

		for (int i = 0; i< pads_tab->size(); ++i)
		{
			if ((*pads_tab)[i].layer != 0)
				(*pads_tab)[i].layer = ui_layers_nr-1;
		}
	}
	//ustalenie maksymalnej liczby przelotek
	unsigned int max_vias_nr;
	cout << "\nPodaj maksymalna liczbe przelotek: ";
	cin >> max_vias_nr;

	vector<range_map> rangeMap(ui_layers_nr, range_map(ui_width, ui_height, dxy));			//stworzenie mapy odleglosci od przeszkod
	vector<range_map> rangeMapClearance(ui_layers_nr, range_map(ui_width, ui_height, dxy));	//stworzenie mapy odleglosci od przeszkod (wliczajac przeswit)

	vector<bool> vias_tab(ui_width*ui_height*ui_layers_nr, false);	//mapa przelotek

	//odczyt miejsc (pady tht), w ktorych sa przelotki 
	for (int i = 0; i< pads_tab->size(); ++i)
	{
		if (!(*pads_tab)[i].is_smd)
			for (int k = 0; k<ui_layers_nr; ++k)
				vias_tab[k*ui_height*ui_width + (*pads_tab)[i].ui_y*ui_width + (*pads_tab)[i].ui_x]=true;
	}

	vector<point> points(ui_width * ui_height * ui_layers_nr);		//wektor punktow plytki
	vector<point*> came_from(ui_width * ui_height*ui_layers_nr);	//wektor poprzedników punktow
	vector<path> paths(pads_tab->size() - my_pcb.get_nets_nr()+1);	//wektor wyznaczonych sciezek
	vector<bool> closed_set(ui_layers_nr*ui_height*ui_width);		//wektor punktow przejrzanych

	//kierunki, w ktorych mozemy odnalezc sasiadow danego punktu
	const int neighbor_offset[8] = {
		-1, +1, -static_cast<int>(ui_width), +static_cast<int>(ui_width),
		-1 - static_cast<int>(ui_width), +1 - static_cast<int>(ui_width), -1 + static_cast<int>(ui_width),
		+1 + static_cast<int>(ui_width)
	};

	const int neighbor_cost[9] = { 5, 5, 5, 5, 7, 7, 7, 7, layer_cost };	//koszt przejscia do sasiada
	unsigned short path_index = 0;							//ilerator po sciezkach
	unsigned int path_index_net = 0;						//liczba sciezek ukonczonych netow
	string pad_name_prev = ((*pads_tab)[0]).net->name;		//nazwa sieci do ktorej nalezy pad poprzedni w wektorze padow

	auto start = get_time::now();

	//petla glowna programu, wybierane sa kolejne pady z wektora i sprawdzane jest czy nalezy wyznaczyc dla nich sciezke
	for (int pad_iterator = 1; pad_iterator < pads_tab->size(); ++pad_iterator) 
	{
		//odczyt wspolrzednych i nazwy sieci do ktorej nalezy pad
		const unsigned int pad_x = static_cast<unsigned int>(((*pads_tab)[pad_iterator]).x / dxy);
		const unsigned int pad_y = static_cast<unsigned int>(((*pads_tab)[pad_iterator]).y / dxy);
		const unsigned int pad_z = static_cast<unsigned int>(((*pads_tab)[pad_iterator]).layer );
		const string pad_name = (*pads_tab)[pad_iterator].net->name;

		if (pad_name == pad_name_prev)	//jesli pad poprzedni nalezy do tej samej sieci, znajdz polaczenie miedzy padami
		{
			bool path_found = false;	//flaga informujaca czy sciezka zostala znaleziona

			//wyczyszczenie zawartosci map odleglosci i dodanie do nich padow
			for(int i=0;i<ui_layers_nr;++i)		
			{
				rangeMap[i].clear_range_map();
				rangeMapClearance[i].clear_range_map();
			}
			pads_to_range_map(*pads_tab, rangeMap, ((*pads_tab)[pad_iterator]).net->name);
			pads_to_range_map_clearance(*pads_tab, rangeMapClearance, ((*pads_tab)[pad_iterator]).net->name);

			//dodaje aktualnie wyznaczone scie¿ki i przelotki do map odleglosci (za wyjatkiem tych nalezacej do tej samej sieci)
			for (unsigned int i = 0; i < path_index_net; ++i)
			{
				const unsigned int radius = paths[i].get_path_width() / 2;
				const unsigned int clearance = paths[i].get_clearance();
				
				for (auto it = (paths[i].get_path_pointer())->begin(); it != (paths[i].get_path_pointer())->
					end(); ++it)
				{
					if (vias_tab[(*it)->get_y()*ui_width + (*it)->get_x()])
					{
						rangeMap[(*it)->get_z()].range_map_fill_ball((*it)->get_x(), (*it)->get_y(), paths[i].get_via_diam() / 2);
						rangeMapClearance[(*it)->get_z()].range_map_fill_ball((*it)->get_x(), (*it)->get_y(), paths[i].get_via_diam() / 2+clearance);
					}
					
					else
					{
						rangeMap[(*it)->get_z()].range_map_fill_ball((*it)->get_x(), (*it)->get_y(), radius);
						rangeMapClearance[(*it)->get_z()].range_map_fill_ball((*it)->get_x(), (*it)->get_y(), radius+clearance);	
					}
				}
			}

			//odczyt wspolrzednych i nazwy sieci do ktorej nalezy pad poprzedni
			const unsigned int pad_x_prev = static_cast<unsigned int>(((*pads_tab)[pad_iterator - 1]).x / dxy);
			const unsigned int pad_y_prev = static_cast<unsigned int>(((*pads_tab)[pad_iterator - 1]).y / dxy);
			const unsigned int pad_z_prev = static_cast<unsigned int>(((*pads_tab)[pad_iterator - 1]).layer );

			//ustalenie punktu startowego i koncowego we wspolrzednych padow
			point* startPoint = &points[pad_z_prev*ui_width*ui_height+ui_width * pad_y_prev + pad_x_prev];
			point* endPoint = &points[pad_z*ui_width*ui_height+ui_width * pad_y + pad_x];

			//edycja wsporzednych punktu poczatkowego, jesli jest punkt blizszy w danej sieci polaczen
			double dist = startPoint->dist(endPoint);
			for (int i = path_index_net; i < static_cast<int>(path_index); ++i)
			{
				for (auto it = (paths[i].get_path_pointer())->begin(); it != (paths[i].get_path_pointer())->
					end(); ++it)
				{
					const double curr_dist = (*it)->dist(endPoint);
					if (curr_dist < dist)
					{
						startPoint = (*it);
						dist = curr_dist;
					}
				}
			}
			//wyczyszczenie zawartosci wektorow
			came_from.assign(ui_width*ui_height*ui_layers_nr, nullptr);
			closed_set.assign(ui_width*ui_height*ui_layers_nr, false);
			for (unsigned int i = 0; i <ui_width; i++)
				for (unsigned int j = 0; j < ui_height; j++)
					for (unsigned int k = 0; k < ui_layers_nr; k++)
						points[k*ui_width*ui_height + j * ui_width + i] = point(i, j, k, inf, inf);

			//przypisanie wartosci f i g punktowi poczatkowemu
			points[pad_z_prev*ui_width*ui_height + ui_width * pad_y_prev + pad_x_prev].set_f_score(RANGEMAP_VALUE_MUL * startPoint->dist(endPoint));
			points[pad_z_prev*ui_width*ui_height + ui_width * pad_y_prev + pad_x_prev].set_g_score(0);


			//stworzenie kolejki przechowujacej punkty do przejrzenia i dodanie do niej punktu startowego
			priority_queue open_set(ui_width, ui_height, ui_layers_nr);
			open_set.push(startPoint);

			//minimalny dystans od innych obiektow, konieczny do spelnienia DRC
			const unsigned short min_dist = ((*pads_tab)[pad_iterator].net->ui_trace_width / 2 + (*pads_tab)[pad_iterator].net->ui_clearance) *
				RANGEMAP_VALUE_MUL;
			const unsigned short min_via_dist = ((*pads_tab)[pad_iterator].net->ui_via_dia / 2 + (*pads_tab)[pad_iterator].net->ui_clearance) *
				RANGEMAP_VALUE_MUL;

			//petla glowna algortymu A*, wykonywana dopoki kolejka open_set nie jest pusta lub zostanie wyznaczona sciezka
			while (!open_set.empty())
			{
				point* current = open_set.pop();	//pobranie punktu o najnizszym priorytecie
				
				//jesli zostanie osiagniety punkt koncowy, do paths dodajemy wyznaczona sciezke
				if (current == endPoint)
				{
					cout << "Znaleziono sciezke nr. " << path_index << endl;
					paths[path_index] = path(current, came_from, ((*pads_tab)[pad_iterator]).net->ui_trace_width, ((*pads_tab)[pad_iterator]).net->ui_via_dia, ui_width, ui_height,
						((*pads_tab)[pad_iterator]).net->ui_clearance);
					paths[path_index].check_vias(vias_tab);
					++path_index;
					path_found = true;
					break;
				}

				closed_set[current->get_z() * ui_width * ui_height + current->get_y() * ui_width + current->get_x()] = true;	//zaznaczenie, ze punkt zostal przejrzany

				//sprawdzenie czy mozliwe jest poprowadzenie sciezki przez punkt sasiadujacy, jesli tak, dodanie go do openset
				//jesli sasiad jest w openset - aktualizacja wartosci sasiada, w przypadku gdy aktualna sciezka jest bardziej oplacalna
				for (int i = 0; i < 8; ++i)
				{
					const int index = (current->get_z() * ui_width * ui_height + current->get_y() * ui_width + current->get_x());
					const int neighbor = index + neighbor_offset[i];

					if (!(closed_set[neighbor] || rangeMap[current->get_z()].get_range_map_value(current->get_y() * ui_width + current->get_x()) <= min_dist
						|| rangeMapClearance[current->get_z()].get_range_map_value(current->get_y() * ui_width + current->get_x()) <= (((*pads_tab)[pad_iterator]).net->ui_trace_width / 2)*RANGEMAP_VALUE_MUL))
					{
						const unsigned int tentative_g_score = points[index].get_g_score() + neighbor_cost[i];
						if (!open_set.is_in_queue(&points[neighbor]))
						{
							came_from[neighbor] = current;

							points[neighbor].set_g_score(tentative_g_score);
							points[neighbor].set_f_score(tentative_g_score + points[neighbor].dist(endPoint) * RANGEMAP_VALUE_MUL);

							open_set.push(&points[neighbor]);
						}

						else if (tentative_g_score < points[neighbor].get_g_score())
						{
							came_from[neighbor] = current;
							open_set.resort(&points[neighbor], tentative_g_score + points[neighbor].dist(endPoint) * RANGEMAP_VALUE_MUL, tentative_g_score);
						}
					}
				}

		
				//sprawdzenie czy mo¿na poprowadzic œcie¿ke do warstw s¹siednich (w gore), jesli tak, dodanie go do openset
				//jesli sasiad jest w openset - aktualizacja wartosci sasiada, w przypadku gdy aktualna sciezka jest bardziej oplacalna
				const unsigned int index = current->get_z()*ui_width*ui_height + current->get_y()*ui_width + current->get_x();
				if (rangeMap[current->get_z()].get_range_map_value(current->get_y()*ui_width + current->get_x()) >= min_via_dist 
					&& rangeMapClearance[current->get_z()].get_range_map_value(current->get_y()*ui_width + current->get_x()) >= ((*pads_tab)[pad_iterator]).net->ui_via_dia /2 * RANGEMAP_VALUE_MUL)
				{
					unsigned short neighbor_layer = current->get_z();
					//unsigned short neighbor_layer_down = current->get_z()+1;
					while(neighbor_layer !=0)
					{
						--neighbor_layer;

						const int neighbor_up = neighbor_layer * ui_width*ui_height + current->get_y()*ui_width + current->get_x();

						if (!(closed_set[neighbor_up]||rangeMap[neighbor_layer].get_range_map_value(current->get_y()*ui_width + current->get_x()) <= min_via_dist||
							rangeMapClearance[neighbor_layer].get_range_map_value(current->get_y()*ui_width + current->get_x()) <= ((*pads_tab)[pad_iterator]).net->ui_via_dia / 2 * RANGEMAP_VALUE_MUL))
						{
							int neighbor_layer_cost = neighbor_cost[8];
							if (vias_tab[index])
								neighbor_layer_cost = 0;

							const unsigned int tentative_g_score = points[index].get_g_score() + neighbor_layer_cost;
							if (!open_set.is_in_queue(&points[neighbor_up]))
							{
								came_from[neighbor_up] = current;

								points[neighbor_up].set_g_score(tentative_g_score);
								points[neighbor_up].set_f_score(tentative_g_score + points[neighbor_up].dist(endPoint) * RANGEMAP_VALUE_MUL);

								open_set.push(&points[neighbor_up]);
							}

							else if (tentative_g_score < points[neighbor_up].get_g_score())
							{
								came_from[neighbor_up] = current;
								open_set.resort(&points[neighbor_up], tentative_g_score + points[neighbor_up].dist(endPoint) * RANGEMAP_VALUE_MUL, tentative_g_score);
							}
						}
						else break;
					}

					//sprawdzenie czy mo¿na poprowadzic œcie¿ke do warstw s¹siednich (w dol), jesli tak, dodanie go do openset
					//jesli sasiad jest w openset - aktualizacja wartosci sasiada, w przypadku gdy aktualna sciezka jest bardziej oplacalna
					neighbor_layer = current->get_z();

					while (neighbor_layer != ui_layers_nr-1)
					{
						++neighbor_layer;
						const int neighbor_down = neighbor_layer * ui_width*ui_height + current->get_y()*ui_width + current->get_x();

						if (!(closed_set[neighbor_down] || rangeMap[neighbor_layer].get_range_map_value(current->get_y()*ui_width + current->get_x()) <= min_via_dist ||
							rangeMapClearance[neighbor_layer].get_range_map_value(current->get_y()*ui_width + current->get_x()) <= ((*pads_tab)[pad_iterator]).net->ui_via_dia / 2 * RANGEMAP_VALUE_MUL))
						{
							int neighbor_layer_cost = neighbor_cost[8];
							if (vias_tab[index])
								neighbor_layer_cost = 0;

							const unsigned int tentative_g_score = points[index].get_g_score() + neighbor_layer_cost;
							if (!open_set.is_in_queue(&points[neighbor_down]))
							{
								came_from[neighbor_down] = current;

								points[neighbor_down].set_g_score(tentative_g_score);
								points[neighbor_down].set_f_score(tentative_g_score + points[neighbor_down].dist(endPoint) * RANGEMAP_VALUE_MUL);

								open_set.push(&points[neighbor_down]);
							}

							else if (tentative_g_score < points[neighbor_down].get_g_score())
							{
								came_from[neighbor_down] = current;

								open_set.resort(&points[neighbor_down], tentative_g_score + points[neighbor_down].dist(endPoint) * RANGEMAP_VALUE_MUL, tentative_g_score);
							}
						}
						else break;
					}
				}
			}

			//przerwanie szukania sciezek, jesli aktualna nie zostala znaleziona
			if (path_found==false)
			{
				cout << "Brak rozwiazania. Liczba wyznaczonych sciezek: "<< path_index << endl;
				break;
			}

			//przeliczenie liczby uzytych przelotek
			if(path_index== pads_tab->size() - my_pcb.get_nets_nr()+1)
			{
				unsigned int vias_nr = 0;
				for(unsigned int i=0;i<ui_width;++i)
				{
					for (unsigned int j = 0; j<ui_height; ++j)
					{
						bool via_point = false;
						for (unsigned int k = 0; k<ui_layers_nr; ++k)
						{
							if (vias_tab[k* ui_height * ui_width + j *ui_width+i])
								via_point = true;
						}
						
						if (via_point)
							++vias_nr;				
					}		
				}
				for (unsigned int i = 0; i < (*pads_tab).size(); ++i)
					if (!(*pads_tab)[i].is_smd)
					{
						--vias_nr;

						for (unsigned int k = 0; k<ui_layers_nr; ++k)
							vias_tab[k* ui_height * ui_width + (*pads_tab)[i].ui_y * ui_width + (*pads_tab)[i].ui_x] = false;
					}


				if (vias_nr > max_vias_nr)
					cout << "Brak rozwiazania. Przekroczono liczbe przelotek (liczba dodanych przelotek = " << vias_nr << " )" << endl;
				else
					cout << "Znaleziono rozwiazanie" << endl;

				break;
			}
		}
		else path_index_net = path_index;

		pad_name_prev = pad_name;
	}

	auto end = get_time::now();
	auto diff = end - start;
	std::cout << "Czas wykonywania algorytmu A*:  " << chrono::duration_cast<ns>(diff).count() << " ns " << endl;

	//dodanie do mapy odleglosci wszystkich padow oraz sciezek
	for (int i = 0; i<ui_layers_nr; ++i)
		rangeMap[i].clear_range_map();
	pads_to_range_map(*pads_tab, rangeMap);
	for (int i = 0; i < path_index; ++i)
	{
		const unsigned int radius = paths[i].get_path_width() / 2;

		for (auto it = (paths[i].get_path_pointer())->begin(); it != (paths[i].get_path_pointer())->
			end(); ++it)
		{
			if(vias_tab[(*it)->get_y()*ui_width+ (*it)->get_x()])
				rangeMap[(*it)->get_z()].range_map_fill_ball((*it)->get_x(), (*it)->get_y(), paths[i].get_via_diam() / 2);
			else
				rangeMap[(*it)->get_z()].range_map_fill_ball((*it)->get_x(), (*it)->get_y(), radius);
		}
	}

	//przepisanie rangemapy do wektora uzywanego do wygenerowania obrazu
	vector<unsigned char>image;
	for (unsigned l = 0; l<ui_layers_nr; ++l)
	{
		image.assign(ui_height*ui_width*4,0);
		for (unsigned y = 0; y < ui_height; y++)
		{
			for (unsigned x = 0; x < ui_width; x++)
			{
				if (rangeMap[l].get_range_map_value(y * ui_width + x) == 0)
				{
					image[4 * ui_width * y + 4 * x + 0] = 255;
					image[4 * ui_width * y + 4 * x + 1] = 255;
					image[4 * ui_width * y + 4 * x + 2] = 255;
					image[4 * ui_width * y + 4 * x + 3] = 255;
				}
				else
				{
					image[4 * ui_width * y + 4 * x + 0] = 10;
					image[4 * ui_width * y + 4 * x + 1] = 10;
					image[4 * ui_width * y + 4 * x + 2] = 10;
					image[4 * ui_width * y + 4 * x + 3] = 255;
				}
			}
		}
		//zakodowanie obrazow warstw plytek
		string s_file_name = "test_image" + to_string(l) + ".png";
		const char * file_name = s_file_name.c_str();
		encode_one_step(file_name, image, ui_width, ui_height);
		cout << "zakodowano obrazek: " << l << endl;
	}

	int t;
	cin >>t;
	getchar();
	return 0;
}
