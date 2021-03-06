#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include "grid.h"
#include "misc.h"

using namespace std;

// Sample constructor
template <int dim>
Sample<dim>::Sample(const unsigned int n_var,
                    const unsigned int n_cell,
                    const unsigned int n_moment,
                    const unsigned int counter)
   :
   x (dim),
   n_var  (n_var),
   n_cell (n_cell),
   idx    (counter)
{
   status = NEW;
   load   = false;
   
   J.resize (n_moment);
   
   // Set directory for sample: S000 to S999
   if(counter <= 9)
      sprintf(directory, "S00%d", counter);
   else if(counter <= 99)
      sprintf(directory, "S0%d", counter);
   else if(counter <= 999)
      sprintf(directory, "S%d", counter);
   else
   {
      cout << "Sample: exceeding 1000 !!!" << endl;
      abort ();
   }
}

// Read primal/adjoint solution from file
// TBD Assuming only one adjoint solution
template <int dim>
void Sample<dim>::read ()
{
   // Make sure solution is not already loaded into memory
   assert (load == false);
   
   primal  = new double [n_var * n_cell];
   adjoint = new double [n_var * n_cell];
   
   // Read from file
   char filename[128];   
   ifstream ff;
   unsigned int c;
   
   // Read primal solution
   sprintf(filename, "RESULT/%s/primal.dat", directory);
   ff.open (filename);
   assert (ff.is_open());
   c = 0;
   for(unsigned int i=0; i<n_cell; ++i)
      for(unsigned int j=0; j<n_var; ++j)
         ff >> primal[c++];
   ff.close ();
   
   // Read adjoint solution
   sprintf(filename, "RESULT/%s/adjoint.dat", directory);
   ff.open (filename);
   assert (ff.is_open());
   c = 0;
   for(unsigned int i=0; i<n_cell; ++i)
      for(unsigned int j=0; j<n_var; ++j)
         ff >> adjoint[c++];
   ff.close ();
   
   load = true;
}

// Free memory for sample
template <int dim>
void Sample<dim>::clear ()
{
   // Make sure solution is in memory before clearing it
   assert (load == true);
   
   delete [] primal;
   delete [] adjoint;
   
   load = false;
}

// Element constructor
template <int dim>
Element<dim>::Element (const unsigned int order,
                       const unsigned int n_moment,
                       const unsigned int n_cell,
                       const unsigned int counter)
   :
   order    (order),
   n_moment (n_moment),
   n_cell   (n_cell)
{
   assert (order == 1 || order == 2);
   assert (n_moment > 0);
   
   status      = NEW;
   active      = true;
   refine_flag = false;
   moment.resize  (n_moment);
   adj_cor.resize (n_moment);
   RE.resize      (n_moment);

   if(dim == 1)
   {
      if(order==1) n_dof = 2;
      if(order==2) n_dof = 3;
   }
   else if(dim == 2)
   {
      if(order==1) n_dof = 3;
      if(order==2) n_dof = 6;
   }
   else 
   {
      cout << "Element::init : dim = " << dim
           << " not implemented" << endl;
      abort ();
   }
   
   idof.resize(n_dof);
   dof.resize (n_dof);
   
   // Set directory for element: E000 to E999
   // Used only if physical grid is refined
   if(counter <= 9)
      sprintf(directory, "E00%d", counter);
   else if(counter <= 99)
      sprintf(directory, "E0%d", counter);
   else if(counter <= 999)
      sprintf(directory, "E%d", counter);
   else
   {
      cout << "Element: exceeding 1000 !!!" << endl;
      abort ();
   }
}

// Read primal/adjoint solution for all samples of element into memory
template <int dim>
void Element<dim>::read_dof ()
{
   for(unsigned int d=0; d<n_dof; ++d)
      dof[d]->read();
}

// Clear primal/adjoint solution for all samples of element from memory
template <int dim>
void Element<dim>::clear_dof ()
{
   for(unsigned int d=0; d<n_dof; ++d)
      dof[d]->clear();
}

// Save mesh_error into files
template <int dim>
void Element<dim>::save_mesh_error ()
{
   unsigned int c = 0;
   
   for(unsigned int i=0; i<n_moment; ++i)
   {
      // Create directory
      char command[128];
      sprintf(command, "mkdir -p RESULT/%s", directory);
      system(command);
      
      // Write mesh error indicator
      char filename[64];
      sprintf(filename, "RESULT/%s/error%d.dat", directory, i);
      ofstream fo;
      fo.open (filename);
      fo.precision (15);
      fo.setf (ios::scientific);
      
      for(unsigned int j=0; j<n_cell; ++j)
         fo << mesh_error[c++] << endl;
      
      fo.close ();
   }
   
   mesh_error.resize (0);
}

// Read element mesh_error from files
template <int dim>
void Element<dim>::load_mesh_error ()
{
   mesh_error.resize (n_moment * n_cell);
   
   unsigned int c = 0;
   
   for(unsigned int i=0; i<n_moment; ++i)
   {
      char filename[64];
      sprintf(filename, "RESULT/%s/error%d.dat", directory, i);
      ifstream fi;
      fi.open (filename);
      assert (fi.is_open());
      
      for(unsigned int j=0; j<n_cell; ++j)
         fi >> mesh_error[c++];
      
      fi.close ();
   }
}

// Find largest edge of a triangular stochastic element
template <>
vector<unsigned int> Element<2>::largest_face (const valarray<double>& x0,
                                               const valarray<double>& x1,
                                               const valarray<double>& x2)
{
   // Find length of three edges
   valarray<double> dx0 = x1 - x0;
   double ds0 = norm (dx0);
   
   valarray<double> dx1 = x2 - x1;
   double ds1 = norm (dx1);
   
   valarray<double> dx2 = x0 - x2;
   double ds2 = norm (dx2);

   vector<unsigned int> edge = idof;
   
   if (ds1 > ds0)
   {
      edge[0] = idof[1];
      edge[1] = idof[2];
      edge[2] = idof[0];
      if(order == 2)
      {
         edge[3] = idof[4];
         edge[4] = idof[5];
         edge[5] = idof[3];
      }
   }
   
   if (ds2 > ds1 && ds2 > ds0)
   {
      edge[0] = idof[2];
      edge[1] = idof[0];
      edge[2] = idof[1];
      if(order == 2)
      {
         edge[3] = idof[5];
         edge[4] = idof[3];
         edge[5] = idof[4];
      }
   }
   
   return edge;
}

// Set pointer from element.dof to sample
// This must be done every time sample is modified by push_back,
// usually after grid refinement
template <int dim>
void Grid<dim>::reinit_dof (vector<typename Sample<dim>::Sample>& sample)
{
   for(unsigned int i=0; i<element.size(); ++i)
      for(unsigned int j=0; j<element[i].n_dof; ++j)
      {
         unsigned int idx = element[i].idof[j];
         element[i].dof[j] = &sample[idx];
      }
}

// Count number of active elements
template <int dim>
int Grid<dim>::n_active_elements () const
{
   int n = 0;
   for(unsigned int i=0; i<element.size(); ++i)
      if(element[i].active)
         ++n;
   return n;
}

// To avoid linker errors
template class Sample<1>;
template class Element<1>;
template class Grid<1>;
template class Sample<2>;
template class Element<2>;
template class Grid<2>;