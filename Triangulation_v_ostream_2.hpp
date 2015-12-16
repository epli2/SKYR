#include <CGAL/Triangulation_2.h>
#include <iostream>
#include <sstream>

namespace CGAL {

  template < class GT, class TDS >
  std::string
  my_show_triangulation_faces(const Triangulation_2<GT,TDS> &T)
  {
    std::ostringstream os;
    // Finite vertices coordinates.
    std::map<typename Triangulation_2<GT, TDS>::Vertex_handle, int> V;
    int inum = 0;
    for( typename Triangulation_2<GT, TDS>::Vertex_iterator
      vit = T.vertices_begin(); vit != T.vertices_end(); ++vit) {
        V[vit] = inum++;
        // std::cout << vit->point() << std::endl;
    }

    // Finite faces indices.
    for( typename Triangulation_2<GT, TDS>::Face_iterator
      fit = T.faces_begin(); fit != T.faces_end(); ++fit) {
        for (int i=0; i<3; i++)
          os << V[fit->vertex(i)] << ",";
        os << "-1,\n";
    }
    return os.str();
  }
} //namespace CGAL
