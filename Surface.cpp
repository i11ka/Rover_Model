#include "Surface.h"

size_t _max_rand_hummocks;
size_t _max_rand_logs;
size_t _max_rand_stones;

Surface::Surface(const double & surf_len, const double & surf_wid, const double & surf_uneven){
    length = surf_len;
    width = surf_wid;
    unevenness_degree = surf_uneven;
    pixels = new Point*[int(length/NET_STEP)+1];
    for (int i = 0; i <= int(length/NET_STEP); i++)
    {
        pixels[i] = new Point[int(width/NET_STEP)+1];
    }
    double random_x, random_y, random_z, random_sig_y, random_sig_x, random_radius, random_rotation, random_x2, random_y2;
    srand(time(NULL));
    for (size_t i = 0; i < _max_rand_hummocks; i++)
    {
        random_x = get_random(length/10, length*9/10);
        random_y = get_random(width/10, width*9/10);
        random_z = get_random(-MAX_HUMMOCK_HEIGHT/2, MAX_HUMMOCK_HEIGHT);
        random_sig_x = get_random(surf_uneven/5, surf_uneven);
        random_sig_y = get_random(surf_uneven/5, surf_uneven);
        random_rotation = get_random(0, 2*3.1415);

        hummocks.push_back(Hummock(random_x, random_y, random_z, random_sig_x, random_sig_y, random_rotation));
    }
    for (size_t i = 0; i < _max_rand_stones; i++)
    {
        random_x = get_random(length/10, length*9/10);
        random_y = get_random(width/10, width*9/10);
        random_z = get_random(-MAX_HUMMOCK_HEIGHT/2, MAX_HUMMOCK_HEIGHT);
        random_radius = get_random(0, MAX_STONE_HEIGHT);

        stones.push_back(Stone(random_x, random_y, random_radius));
    }
    for (size_t i = 0; i < _max_rand_logs; i++)
    {
        random_x = get_random(length/10, length*9/10);
        random_x2 = get_random(length/10, length*9/10);
        random_y = get_random(width/10, width*9/10);
        random_y2 = get_random(width/10, width*9/10);
        random_radius = get_random(0, MAX_LOGS_HEIGHT);
        
        logs.push_back(Log(random_x, random_y, random_x2, random_y2, random_radius));
    }
}

Surface:: ~Surface(){
    for (int i = 0; i < int(length/NET_STEP); i++) {
        delete[] pixels[i];
    }
    delete[] pixels;
}

bool Surface::add_hummock(const double & x, const double & y, const double & z, const double & sig_x, const double & sig_y, const double & ax_rotat_angle){
    if(x > -EPS && y > -EPS && x < length && y < width){
        hummocks.push_back(Hummock(x, y, z, sig_x, sig_y, ax_rotat_angle));
        return true;
    }
    else{
        return false;
    }
}

bool Surface::add_log(const double & x1, const double & y1, const double & x2, const double & y2, const double & radius)
{
    if(x1 > -EPS && y1 > -EPS && x1 < length && y1 < width && x2 > -EPS && y2 > -EPS && x2 < length && y2 < width){
        logs.push_back(Log(x1, y1, x2, y2, radius));
        return true;
    }
    else{ 
        return false;
    }
}

bool Surface::add_stone(const double &x, const double &y, const double &radius)
{
    if(x > 0 && y > 0 && x < length && y < width){
        stones.push_back(Stone(x, y, radius));
        return true;
    }
    else{
        return false;
    }
}

void Surface::Print_in_file(ofstream& file){
    for (int i = 0; i <= int(length/NET_STEP); i++)
    {
        for (int j = 0; j <= int(width/NET_STEP); j++)
        {
            file << i*NET_STEP << " " << j*NET_STEP << " " << pixels[i][j].z_cord << "\n";
        }
    }
}

void Surface::count_surface(){
    for (int i = 0; i <= int(length/NET_STEP); i++)
    {
        for (int j = 0; j <= int(width/NET_STEP); j++)
        {
            pixels[i][j] = Point(i*NET_STEP, j*NET_STEP, get_surface_height(i*NET_STEP, j*NET_STEP));
        }
    }
    
}

double Surface::get_surface_height(const double & x, const double & y){
    double z = 0;
    for (size_t i = 0; i < hummocks.size(); i++)
    {
        z += hummocks[i].get_hummock_height(x, y);
    }
    for (size_t i = 0; i < stones.size(); i++)
    {
        z += stones[i].get_stone_height(x, y);
    }
    for (size_t i = 0; i < logs.size(); i++)
    {
        z += logs[i].get_log_height(x, y);
    }
    return z;
}