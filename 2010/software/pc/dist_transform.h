#ifndef DIST_TRSFRM_H
#define DIST_TRSFRM_H

#include <stdint.h>
#include <queue>
#include <list>
#include <utility>

#include "types.h"

#define DT_DIST_RESOL  0.005    // Résolution métrique pour la carte des distances
#define DT_ANGLE_RESOL 8        // Résolution angulaire pour la carte des distances

using namespace std;

//------------------------------------------------------------------------------

typedef float dt_dist;

//------------------------------------------------------------------------------

class dt_map;

class dt_zone
{
  friend class dt_map;
  
  private:
  void allocate();
  
  protected:
  int height;
  int *left;
  int *right;
  int cy;
  
  public:
  dt_zone(int h);
  dt_zone(int h, int c);  
  ~dt_zone();
  
  dt_zone* operator * (const dt_zone &A);
};

//------------------------------------------------------------------------------

class dt_zone_disc : public dt_zone
{
  public:
  dt_zone_disc(int radius);
};

class dt_zone_hbox : public dt_zone
{
  public:
  dt_zone_hbox(int w, int h);
};

class dt_zone_orientbox : public dt_zone
{
  private:
  void line(int *array, int x1, int y1, int x2, int y2);
  inline void swap_int(int* x, int* y);
  
  public:
  dt_zone_orientbox(int width, int depth, double angle);
};

//------------------------------------------------------------------------------

typedef list<pair<dt_dist, dt_dist> > dt_path;

//------------------------------------------------------------------------------

class dt_map
{
  private:
  static const dt_dist pix_cost;
  static const double RAD_ANGLE_RESOL;
  static const double RAD_ANGLE_RESOL_2;  
  
  int width, height;
  dt_dist** pix;
  
  dt_zone_orientbox **robot;
  
  typedef struct dt_pix
  {
    int x,y;
    dt_dist dist;
    dt_dist score;
    
    dt_pix() {};
    dt_pix(int _x, int _y, dt_dist _dist, dt_dist _score) : x(_x), y(_y), dist(_dist), score(_score) {}
    bool operator < (const dt_pix &b) const {return score>b.score;}
  } dt_pix;
  
  // 1-dimensional distance transform
  void distance_transform_1D(dt_dist *src_pix, dt_dist *dest_pix, bool vertical);
  
  // Marque l'intérieur de la zone comme infranchissable
  void fillZone(dt_zone *zone, int iAngle, int x, int y);
  
  // Transforme une distance en score entre 0 et 1
  static inline dt_dist score(double distance);

  // Renvoie l'énergie associé au pixel
  dt_dist get_pix_energy(int x, int y, double a);
  
  // Estime l'énergie associée au chemin de (x1,y1) à (x2,y2)
  dt_dist estimate_path_energy(int x1, int y1, int x2, int y2);
  
  // Vérifie que la coordonnée est dans le terrain
  static inline int clip(int c, int max);
  
  // Ajoute un pixel à la frontière
  void add_to_queue(dt_dist *dist, bool *processed, priority_queue<dt_pix> &boundary, priority_queue<dt_pix> &boundary_dont_cross, bool allow_crossing, dt_dist d, int x, int y, int xt, int yt);
  
  // Deduit le chemin à partir de la carte des distances
  dt_path dist2path(dt_dist *dist, int x, int y);
   
  public:    
  // Constructeur / Destructeur
  dt_map(double terrainWidth, double terrainHeight, double robotWidth, double robotDepth);
  ~dt_map();
      
  // Produit un tableau pouvant être envoyé à save_buff_to_bitmap (see common/bitmap.h)
  uint16_t* to_bitmap(int iAngle, int &w, int &h);
  
  // Calcule la distance en chaque point de la map
  void compute_distance_transform();
  
  // Remplis les cartes avec les zones indiqués en prenant en compte la taille du robot
  void fillBox(double x, double y, double w, double h);
  void fillDisc(double cx, double cy, double radius);
    
  // Renvoie un chemin 
  dt_path find_path(const position_t &from, const position_t &to);
};

#endif
