#pragma once

#include "geometry.hh"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>

namespace delaunay
{
    struct FaceInfo2
    {
        FaceInfo2(){}
        int nesting_level;
        bool in_domain(){ 
            return nesting_level%2 == 1;
        }
    };

    typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
    typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
    typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    Fbb;
    typedef CGAL::Constrained_triangulation_face_base_2<K,Fbb>        Fb;
    typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
    typedef CGAL::Exact_predicates_tag                                Itag;
    typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
    typedef CDT::Point                                                Point;
    typedef CGAL::Polygon_2<K>                                        Polygon_2;
    void 
    mark_domains(CDT& ct, 
                 CDT::Face_handle start, 
                 int index, 
                 std::list<CDT::Edge>& border )
    {
        if(start->info().nesting_level != -1){
            return;
        }
        std::list<CDT::Face_handle> queue;
        queue.push_back(start);
        while(! queue.empty()){
            CDT::Face_handle fh = queue.front();
            queue.pop_front();
            if(fh->info().nesting_level == -1){
                fh->info().nesting_level = index;
                for(int i = 0; i < 3; i++){
                    CDT::Edge e(fh,i);
                    CDT::Face_handle n = fh->neighbor(i);
                    if(n->info().nesting_level == -1){
                        if(ct.is_constrained(e)) border.push_back(e);
                        else queue.push_back(n);
                    }
                }
            }
        }
    }

    //explore set of facets connected with non constrained edges,
    //and attribute to each such set a nesting level.
    //We start from facets incident to the infinite vertex, with a nesting
    //level of 0. Then we recursively consider the non-explored facets incident 
    //to constrained edges bounding the former set and increase the nesting level by 1.
    //Facets in the domain are those with an odd nesting level.
    void
    mark_domains(CDT& cdt)
    {
        for(CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); ++it){
            it->info().nesting_level = -1;
        }
        std::list<CDT::Edge> border;
        mark_domains(cdt, cdt.infinite_face(), 0, border);
        while(! border.empty()){
            CDT::Edge e = border.front();
            border.pop_front();
            CDT::Face_handle n = e.first->neighbor(e.second);
            if(n->info().nesting_level == -1){
                mark_domains(cdt, n, e.first->info().nesting_level+1, border);
            }
        }
    }

    template<typename T>
    geometry::multipolygon<T> tessellate(const geometry::polygon<T> &polygon)
    {
        CDT cdt;
        for (auto&& ring: polygon)
        {
            Polygon_2 poly;
            for (size_t c=0; c<ring.size()-1; c++)
                poly.push_back(Point(ring[c][0], ring[c][1]));
            cdt.insert_constraint(poly.vertices_begin(), poly.vertices_end(), true);
        }

        mark_domains(cdt);

        geometry::multipolygon<T> result;
        for (CDT::Finite_faces_iterator fit=cdt.finite_faces_begin();
                                        fit!=cdt.finite_faces_end();++fit)
        {
            if (fit->info().in_domain())
            {
                geometry::polygon<T> t(1, geometry::linestring<T>(4));
                for (size_t i=0; i<4; i++)
                {
                    t[0][i] = geometry::point<T>({
                            fit->vertex(i%3)->point().x(),
                            fit->vertex(i%3)->point().y()});
                }
                result.push_back(t);
            }
        }
        return result;
    }
}



