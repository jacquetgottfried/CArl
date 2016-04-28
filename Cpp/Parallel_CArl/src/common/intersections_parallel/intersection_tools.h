/*
 * intersection_tools.h
 *
 *  Created on: Apr 17, 2016
 *      Author: Thiago Milanetto Schlittler
 */

#ifndef COMMON_INTERSECTIONS_PARALLEL_INTERSECTION_TOOLS_H_
#define COMMON_INTERSECTIONS_PARALLEL_INTERSECTION_TOOLS_H_

#include "common_header.h"
#include "common_header_libmesh.h"
#include "mesh_tables.h"

#include "CGAL_typedefs.h"

namespace carl
{

/*
 * 		Intersection_tools
 *
 * 			This class contains several tools that help to convert libMesh's
 * 		data structures and vice-versa. A class is used instead of a collection
 * 		of functions to reduce the amount of calls to constructors of CGAL data
 * 		exact data structures.
 *
 */

class	Intersection_Tools
{
protected:
	Nef_Polyhedron m_nef_A;
	Nef_Polyhedron m_nef_B;
	Nef_Polyhedron m_nef_I;
	Nef_Polyhedron m_nef_C;

	std::vector<ExactPoint_3> m_exact_points_A;
	std::vector<ExactPoint_3> m_exact_points_B;
	std::vector<ExactPoint_3> m_exact_points_C;

	ExactPolyhedron m_dummyPoly;

	CGAL::Convex_hull_traits_3<ExactKernel> ExactHullTraits;

	Kernel_to_ExactKernel ConvertInexactToExact;
	ExactKernel_to_Kernel ConvertExactToInexact;

	std::vector<std::vector<unsigned int> > m_tetra_tetrahedrons;
	std::vector<std::vector<unsigned int> > m_tetra_triangles;
	std::vector<std::vector<unsigned int> > m_tetra_edges;

	std::vector<std::vector<unsigned int> > m_hex_tetrahedrons;
	std::vector<std::vector<unsigned int> > m_hex_triangles;
	std::vector<std::vector<unsigned int> > m_hex_edges;

	std::vector<std::vector<unsigned int> > * m_elem_C_tetras;
	std::vector<std::vector<unsigned int> > * m_elem_C_triangles;

	ExactTetrahedron m_test_tetra;
	ExactTriangle_3 m_test_triangle;

	void p_set_element_indexes()
	{
		m_tetra_tetrahedrons.resize(1,std::vector<unsigned int>(4,0));
		m_tetra_triangles.resize(4,std::vector<unsigned int>(3,0));
		m_tetra_edges.resize(6,std::vector<unsigned int>(2,0));

		m_hex_tetrahedrons.resize(5,std::vector<unsigned int>(4,0));
		m_hex_triangles.resize(12,std::vector<unsigned int>(3,0));
		m_hex_edges.resize(12,std::vector<unsigned int>(2,0));

		// Set up tetra tetrahedrons (...)
		m_tetra_tetrahedrons[0] = {0, 1, 2, 3};

		// Set up tetra triangles
		m_tetra_triangles[0] = {0, 1, 2};
		m_tetra_triangles[1] = {1, 2, 3};
		m_tetra_triangles[2] = {0, 1, 3};
		m_tetra_triangles[3] = {0, 2, 3};

		// Set up tetra edges
		m_tetra_edges[0] = {0, 1};
		m_tetra_edges[1] = {0, 2};
		m_tetra_edges[2] = {0, 3};
		m_tetra_edges[3] = {1, 3};
		m_tetra_edges[4] = {3, 2};
		m_tetra_edges[5] = {2, 1};

		// Set up hex tetrahedrons
		m_hex_tetrahedrons[0] = {0, 1, 3, 4};
		m_hex_tetrahedrons[1] = {5, 1, 4, 6};
		m_hex_tetrahedrons[2] = {7, 3, 4, 6};
		m_hex_tetrahedrons[3] = {2, 6, 1, 3};

		m_hex_tetrahedrons[4] = {1, 4, 6, 3};

		// Set up hex triangles
		m_hex_triangles[0] = {0, 1, 2};
		m_hex_triangles[1] = {0, 3, 2};

		m_hex_triangles[2] = {0, 1, 5};
		m_hex_triangles[3] = {0, 4, 5};

		m_hex_triangles[4] = {0, 3, 7};
		m_hex_triangles[5] = {0, 4, 7};

		m_hex_triangles[6] = {6, 5, 4};
		m_hex_triangles[7] = {6, 7, 4};

		m_hex_triangles[8]  = {6, 5, 1};
		m_hex_triangles[9]  = {6, 2, 1};

		m_hex_triangles[10] = {6, 7, 3};
		m_hex_triangles[11] = {6, 2, 3};

		// Set up hex edges
		m_hex_edges[0] = {0, 1};
		m_hex_edges[1] = {1, 2};
		m_hex_edges[2] = {2, 3};
		m_hex_edges[3] = {3, 0};

		m_hex_edges[4] = {4, 5};
		m_hex_edges[5] = {5, 6};
		m_hex_edges[6] = {6, 7};
		m_hex_edges[7] = {7, 4};

		m_hex_edges[8]  = {0, 4};
		m_hex_edges[9]  = {1, 5};
		m_hex_edges[10] = {2, 6};
		m_hex_edges[11] = {3, 7};
	}

	bool p_elements_do_intersect(
			std::vector<ExactPoint_3> & elem_C_points,
			std::vector<std::vector<unsigned int> > & elem_C_tetras,
			std::vector<std::vector<unsigned int> > & elem_C_triangles,
			std::vector<ExactPoint_3> & elem_D_points,
			std::vector<std::vector<unsigned int> > & elem_D_tetras,
			std::vector<std::vector<unsigned int> > & elem_D_triangles)
	{
		bool bElemIntersect = false;

		// Test intersections between C's tetrahedrons and D's triangles
		for(unsigned int jjj = 0; jjj < elem_D_triangles.size(); ++jjj)
		{
			std::vector<unsigned int> & work_triangle = elem_D_triangles[jjj];
			m_test_triangle = ExactTriangle_3(elem_D_points[work_triangle[0]],
					elem_D_points[work_triangle[1]],
					elem_D_points[work_triangle[2]]);

			for(unsigned int iii = 0; iii < elem_C_tetras.size(); ++iii)
			{
				std::vector<unsigned int> & work_tetra = elem_C_tetras[iii];
				m_test_tetra = ExactTetrahedron(	elem_C_points[work_tetra[0]],
						elem_C_points[work_tetra[1]],
						elem_C_points[work_tetra[2]],
						elem_C_points[work_tetra[3]]);

				bElemIntersect = CGAL::do_intersect(m_test_triangle,m_test_tetra);

				if(bElemIntersect)
				{
					// Found intersection!
					break;
				}
			}

			if(bElemIntersect)
			{
				// Found intersection!
				break;
			}
		}

		if(!bElemIntersect)
		{
			// Test intersections between D's tetrahedrons and C's triangles
			for(unsigned int jjj = 0; jjj < elem_C_triangles.size(); ++jjj)
			{
				std::vector<unsigned int> & work_triangle = elem_C_triangles[jjj];
				m_test_triangle = ExactTriangle_3(elem_C_points[work_triangle[0]],
						elem_C_points[work_triangle[1]],
						elem_C_points[work_triangle[2]]);

				for(unsigned int iii = 0; iii < elem_D_tetras.size(); ++iii)
				{
					std::vector<unsigned int> & work_tetra = elem_D_tetras.at(iii);
					m_test_tetra = ExactTetrahedron(	elem_D_points[work_tetra[0]],
							elem_D_points[work_tetra[1]],
							elem_D_points[work_tetra[2]],
							elem_D_points[work_tetra[3]]);

					bElemIntersect = CGAL::do_intersect(m_test_triangle,m_test_tetra);

					if(bElemIntersect)
					{
						// Found intersection!
						break;
					}
				}

				if(bElemIntersect)
				{
					// Found intersection!
					break;
				}
			}
		}
		return bElemIntersect;
	}

public:
	Intersection_Tools(const libMesh::Elem * elem_C)
	{
		m_exact_points_A.resize(8);
		m_exact_points_B.resize(8);
		m_exact_points_C.resize(8);

		m_dummyPoly.reserve(8,18,12);

		libmesh_set_coupling_nef_polyhedron(elem_C);

		p_set_element_indexes();

		m_elem_C_tetras = NULL;
		m_elem_C_triangles = NULL;
	};

	Intersection_Tools()
	{
		m_exact_points_A.resize(8);
		m_exact_points_B.resize(8);
		m_exact_points_C.resize(8);

		m_dummyPoly.reserve(8,18,12);

		m_nef_C.clear(Nef_Polyhedron::EMPTY);

		p_set_element_indexes();

		m_elem_C_tetras = NULL;
		m_elem_C_triangles = NULL;
	};

	/*
	 * 			Find a element from the mesh intersecting the query element.
	 * 		Does so while doing a test to be sure that the query element does
	 * 		indeed intersect the tested mesh. The test can be bypassed using a
	 * 		boolean.
	 *
	 */
	const libMesh::Elem * FindFirstIntersection(	const libMesh::Elem * Query_elem,
								std::unique_ptr<libMesh::PointLocatorBase> & point_locator,
								bool				bGuaranteeQueryIsInMesh = false)
	{
		libMesh::PointLocatorBase& locator = *point_locator.get();
		if(!bGuaranteeQueryIsInMesh)
		{
			// Then we are sure that the query element is inside the mesh, only
			// one search needed
		}
		else
		{
			// Better check all the vertices ...
			unsigned int elem_nb_nodes = Query_elem->n_nodes();
			libMesh::Point dummyPoint;
			bool bInsideTheMesh = true;

			// Just to be sure, check if one of the points intersect the mesh
			for(unsigned int iii = 0; iii < elem_nb_nodes; ++iii)
			{
				dummyPoint = Query_elem->point(iii);
				const libMesh::Elem * Patch_elem = locator(Query_elem->point(iii));

				if(Patch_elem == NULL)
				{
					bInsideTheMesh = false;
					break;
				}
			}

			homemade_assert_msg(bInsideTheMesh, "Query element is not fully inside tested mesh!\n");
		}

		return locator(Query_elem->point(0));
	};

	/*
	 * 			Find all elements from the mesh intersecting the query element.
	 * 		Does so while doing a test to be sure that the query element does
	 * 		indeed intersect the tested mesh.
	 *
	 */
	void FindAllIntersection(	const libMesh::Elem * Query_elem,
								std::unique_ptr<libMesh::PointLocatorBase> & point_locator,
								std::set<unsigned int>	&	Intersecting_elems)
	{
		libMesh::PointLocatorBase& locator = *point_locator.get();

		// Clear output
		Intersecting_elems.clear();

		// Search each vertex
		unsigned int elem_nb_nodes = Query_elem->n_nodes();
		libMesh::Point dummyPoint;

		int nbOfInters = 0;

		// Just to be sure, check if one of the points intersect the mesh
		for(unsigned int iii = 0; iii < elem_nb_nodes; ++iii)
		{
			dummyPoint = Query_elem->point(iii);
			const libMesh::Elem * Patch_elem = locator(Query_elem->point(iii));

			if(Patch_elem != NULL)
			{
				Intersecting_elems.insert(Patch_elem->id());
				++nbOfInters;
			}
		}

		homemade_assert_msg(nbOfInters != 0, "Query element is not fully inside tested mesh!\n");
	};

	/*
	 * 		Test if two elements intersect
	 */
	bool libMesh_exact_do_intersect(const libMesh::Elem * elem_A,
									const libMesh::Elem * elem_B)
	{
		// The booleans
		bool bBboxIntersect = false;
		bool bElemIntersect = false;

		unsigned int n_nodes_A = elem_A->n_nodes();
		unsigned int n_nodes_B = elem_B->n_nodes();

		// First, convert both elements to CGAL exact point vectors
		convert_elem_to_exact_points(elem_A,m_exact_points_A);
		convert_elem_to_exact_points(elem_B,m_exact_points_B);

		// Fast check: bounding boxes
		std::vector<ExactPoint_3>::const_iterator exact_points_A_begin = m_exact_points_A.begin();
		std::vector<ExactPoint_3>::const_iterator exact_points_B_begin = m_exact_points_B.begin();

		Bbox_3 exact_bbox_A = CGAL::bbox_3(exact_points_A_begin,exact_points_A_begin + n_nodes_A);
		Bbox_3 exact_bbox_B = CGAL::bbox_3(exact_points_B_begin,exact_points_B_begin + n_nodes_B);

		bBboxIntersect = CGAL::do_intersect(exact_bbox_A,exact_bbox_B);

		if(bBboxIntersect)
		{
			// Bbox intersect, test intersection between tetrahedrons and triangles

			// Pointers that will depend on the element type;
			std::vector<std::vector<unsigned int> > * elem_A_tetras    = NULL;
			std::vector<std::vector<unsigned int> > * elem_A_triangles = NULL;

			std::vector<std::vector<unsigned int> > * elem_B_tetras    = NULL;
			std::vector<std::vector<unsigned int> > * elem_B_triangles = NULL;

			if(elem_A->type() == libMesh::TET4)
			{
				// Use tetrahedron geometry
				elem_A_tetras = &m_tetra_tetrahedrons;
				elem_A_triangles = &m_tetra_triangles;
			}
			else if(elem_A->type() == libMesh::HEX8)
			{
				// Use hexaedron geometry
				elem_A_tetras = &m_hex_tetrahedrons;
				elem_A_triangles = &m_hex_triangles;
			}
			else
			{
				homemade_error_msg("Unsupported element type! Must be either TET4 or HEX8");
			}

			if(elem_B->type() == libMesh::TET4)
			{
				// Use tetrahedron geometry
				elem_B_tetras = &m_tetra_tetrahedrons;
				elem_B_triangles = &m_tetra_triangles;
			}
			else if(elem_B->type() == libMesh::HEX8)
			{
				// Use hexaedron geometry
				elem_B_tetras = &m_hex_tetrahedrons;
				elem_B_triangles = &m_hex_triangles;
			}
			else
			{
				homemade_error_msg("Unsupported element type! Must be either TET4 or HEX8");
			}

			bElemIntersect = p_elements_do_intersect(m_exact_points_A, *elem_A_tetras, *elem_A_triangles,
					m_exact_points_B, *elem_B_tetras, *elem_B_triangles);
		}
		else
		{
			bElemIntersect = false;
		}

		return bElemIntersect;
	}

	/*
	 * 		Build two elements intersection
	 */
	bool libMesh_exact_intersection(const libMesh::Elem * elem_A,
									const libMesh::Elem * elem_B,
									std::set<Point_3> & points_out,
									bool bConvertPoints = true,
									bool bTestNeeded = true)
	{
		bool bElemIntersect = true;

		if(bTestNeeded)
		{
			// Test the intersection beforehand
			bElemIntersect = libMesh_exact_do_intersect(elem_A,elem_B);
		}
		else if(bConvertPoints)
		{
			// Test already made somewhere else, but we need to set up the exact
			// points.
			convert_elem_to_exact_points(elem_A,m_exact_points_A);
			convert_elem_to_exact_points(elem_B,m_exact_points_B);
		}

		if(bElemIntersect)
		{
			// Generate the Nef polyhedrons
			unsigned int n_nodes_A = elem_A->n_nodes();
			unsigned int n_nodes_B = elem_B->n_nodes();

			std::vector<ExactPoint_3>::const_iterator exact_points_A_begin = m_exact_points_A.begin();
			std::vector<ExactPoint_3>::const_iterator exact_points_B_begin = m_exact_points_B.begin();

			convert_exact_points_to_Nef(	exact_points_A_begin,
											exact_points_A_begin + n_nodes_A,
											m_nef_A);

			convert_exact_points_to_Nef(	exact_points_B_begin,
											exact_points_B_begin + n_nodes_B,
											m_nef_B);

			// Intersect them
			m_nef_I = m_nef_A*m_nef_B;
			if(!m_nef_I.is_empty())
			{
				// Intersection exists! Create output
				bElemIntersect = true;

				points_out.clear();
				for(Nef_Polyhedron::Vertex_const_iterator it_vertex = m_nef_I.vertices_begin();
						it_vertex != m_nef_I.vertices_end();
						++it_vertex)
				{
					points_out.insert(ConvertExactToInexact(it_vertex->point()));
				}
			}
			else
			{
				bElemIntersect = false;
			}
		}

		return bElemIntersect;
	}

	/*
	 * 		Test if two elements intersect inside the coupling region
	 */
	bool libMesh_exact_do_intersect_inside_coupling(const libMesh::Elem * elem_A,
													const libMesh::Elem * elem_B)
	{
		bool bElemIntersect = true;

		// Assert if C was built
		homemade_assert_msg(!m_nef_C.is_empty(), "Coupling restriction element was not set yet!\n");

		// Test the intersection beforehand. This also generates the exact
		// points needed.
		bElemIntersect = libMesh_exact_do_intersect(elem_A,elem_B);

		if(bElemIntersect)
		{
			// Generate the Nef polyhedrons
			unsigned int n_nodes_A = elem_A->n_nodes();
			unsigned int n_nodes_B = elem_B->n_nodes();

			std::vector<ExactPoint_3>::const_iterator exact_points_A_begin = m_exact_points_A.begin();
			std::vector<ExactPoint_3>::const_iterator exact_points_B_begin = m_exact_points_B.begin();

			convert_exact_points_to_Nef(	exact_points_A_begin,
											exact_points_A_begin + n_nodes_A,
											m_nef_A);

			convert_exact_points_to_Nef(	exact_points_B_begin,
											exact_points_B_begin + n_nodes_B,
											m_nef_B);

			// Intersect them
			m_nef_I = m_nef_A*m_nef_B*m_nef_C;
			if(!m_nef_I.is_empty() && m_nef_I.number_of_volumes() > 1)
			{
				// Intersection exists!
				bElemIntersect = true;
			}
			else
			{
				bElemIntersect = false;
			}
		}

		return bElemIntersect;
	}

	/*
	 * 		Build two elements intersection inside the coupling region
	 */
	bool libMesh_exact_intersection_inside_coupling(const libMesh::Elem * elem_A,
													const libMesh::Elem * elem_B,
													std::set<Point_3> & points_out,
													bool bConvertPoints = true,
													bool bTestNeeded = true)
	{
		bool bElemIntersect = true;

		// Assert if C was built
		homemade_assert_msg(!m_nef_C.is_empty(), "Coupling restriction element was not set yet!\n");

		if(bTestNeeded)
		{
			// Test the intersection beforehand
			bElemIntersect = libMesh_exact_do_intersect(elem_A,elem_B);
		}
		else if(bConvertPoints)
		{
			// Test already made somewhere else, but we need to set up the exact
			// points.
			convert_elem_to_exact_points(elem_A,m_exact_points_A);
			convert_elem_to_exact_points(elem_B,m_exact_points_B);
		}

		if(bElemIntersect)
		{
			// Generate the Nef polyhedrons
			unsigned int n_nodes_A = elem_A->n_nodes();
			unsigned int n_nodes_B = elem_B->n_nodes();

			std::vector<ExactPoint_3>::const_iterator exact_points_A_begin = m_exact_points_A.begin();
			std::vector<ExactPoint_3>::const_iterator exact_points_B_begin = m_exact_points_B.begin();

			convert_exact_points_to_Nef(	exact_points_A_begin,
											exact_points_A_begin + n_nodes_A,
											m_nef_A);

			convert_exact_points_to_Nef(	exact_points_B_begin,
											exact_points_B_begin + n_nodes_B,
											m_nef_B);

			// Intersect them
			m_nef_I = m_nef_A*m_nef_B*m_nef_C;
			if(!m_nef_I.is_empty() && m_nef_I.number_of_volumes() > 1)
			{
				// Intersection exists! Create output
				bElemIntersect = true;

				points_out.clear();
				for(Nef_Polyhedron::Vertex_const_iterator it_vertex = m_nef_I.vertices_begin();
						it_vertex != m_nef_I.vertices_end();
						++it_vertex)
				{
					points_out.insert(ConvertExactToInexact(it_vertex->point()));
				}
			}
			else
			{
				bElemIntersect = false;
			}
		}

		return bElemIntersect;
	}

	/*
	 * 		Set the coupling element Nef polyhedron
	 */
	void libmesh_set_coupling_nef_polyhedron(const libMesh::Elem * elem_C)
	{
		unsigned int n_nodes_C = elem_C->n_nodes();

		// First, convert the element to a CGAL exact point vector
		convert_elem_to_exact_points(elem_C,m_exact_points_C);

		// Second, convert to Nef
		std::vector<ExactPoint_3>::const_iterator exact_points_C_begin = m_exact_points_C.begin();
		convert_exact_points_to_Nef(	exact_points_C_begin,
										exact_points_C_begin + n_nodes_C,
										m_nef_C);

		// And, finally, associate the tetra and triangle tables to it
		if(elem_C->type() == libMesh::TET4)
		{
			// Use tetrahedron geometry
			m_elem_C_tetras = &m_tetra_tetrahedrons;
			m_elem_C_triangles = &m_tetra_triangles;
		}
		else if(elem_C->type() == libMesh::HEX8)
		{
			// Use hexaedron geometry
			m_elem_C_tetras = &m_hex_tetrahedrons;
			m_elem_C_triangles = &m_hex_triangles;
		}
		else
		{
			homemade_error_msg("Unsupported element type! Must be either TET4 or HEX8");
		}
	}

	/*
	 * 		Convert an libMesh element to list of exact points
	 */
	void convert_elem_to_exact_points(	const libMesh::Elem       *	elem_input,
										std::vector<ExactPoint_3> &	points_output)
	{
		libMesh::Point dummyPoint;
		for(unsigned int iii = 0; iii < elem_input->n_nodes(); ++iii)
		{
			dummyPoint = elem_input->point(iii);
			points_output[iii] = ExactPoint_3(	dummyPoint(0),
												dummyPoint(1),
												dummyPoint(2));
		}
	}

	/*
	 * 		Convert an exact point set to a Nef polyhedron
	 */
	void convert_exact_points_to_Nef(std::vector<ExactPoint_3>::const_iterator it_begin,
									 std::vector<ExactPoint_3>::const_iterator it_end,
									 Nef_Polyhedron & nef_out)
	{
		CGAL::convex_hull_3(it_begin,it_end,m_dummyPoly,ExactHullTraits);
		nef_out = Nef_Polyhedron(m_dummyPoly);
	}

};
}



#endif /* COMMON_INTERSECTIONS_PARALLEL_INTERSECTION_TOOLS_H_ */
