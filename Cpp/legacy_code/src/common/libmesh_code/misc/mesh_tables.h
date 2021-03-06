/*
 * mesh_tables.h
 *
 *  Created on: Jan 26, 2016
 *      Author: Thiago Milanetto Schlittler
 *
 *      File containing the tools used to read and generate the mesh tables
 */

#ifndef COMMON_LIBMESH_CODE_MESH_TABLES_H_
#define COMMON_LIBMESH_CODE_MESH_TABLES_H_

#include "carl_headers.h"
#include "mpi_carl_tools.h"

namespace carl
{

void set_weight_function_domain_idx(	std::string &filename,
										int& domain_Idx_BIG,
										int& nb_of_domain_Idx_micro,
										std::vector<int>& domain_Idx_micro,
										std::vector<int>& domain_Idx_coupling
										);

//void set_mesh_Gmsh(
//		libMesh::Mesh& mesh,
//		const std::string& mesh_file,
//		std::unordered_map<int,int>& mesh_NodeMap,
//		std::unordered_map<int,int>& mesh_ElemMap
//		);
//
//void set_mesh_Gmsh(
//		libMesh::Mesh& mesh,
//		const std::string& mesh_file,
//		std::unordered_map<int,int> &node_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &node_libmesh_to_gmsh_map,
//		std::unordered_map<int,int> &element_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &element_libmesh_to_gmsh_map
//		);
//template<typename libMeshMesh>
//void set_mesh_Gmsh(
//		libMeshMesh& mesh,
//		const std::string& mesh_file
//		);

//void create_mesh_map(
//		const std::string &filename,
//		std::unordered_map<int,int> &node_map,
//		std::unordered_map<int,int> &element_map
//		);
//
//void create_mesh_map(
//		const std::string &filename,
//		std::unordered_map<int,int> &node_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &node_libmesh_to_gmsh_map,
//		std::unordered_map<int,int> &element_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &element_libmesh_to_gmsh_map
//		);
//
//void create_mesh_map(
//		const std::string &filename,
//		std::unordered_map<int,int> &node_map,
//		std::unordered_map<int,int> &element_map,
//		const libMesh::Parallel::Communicator& WorldComm
//		);
//
//void create_mesh_map(
//		const std::string &filename,
//		std::unordered_map<int,int> &node_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &node_libmesh_to_gmsh_map,
//		std::unordered_map<int,int> &element_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &element_libmesh_to_gmsh_map,
//		const libMesh::Parallel::Communicator& WorldComm
//		);
//
//void build_mesh_map_Gmsh(
//		const std::string &filename,
//		std::unordered_map<int,int> &node_map,
//		std::unordered_map<int,int> &element_map
//		);
//
//void build_mesh_map_Gmsh(
//		const std::string &filename,
//		std::unordered_map<int,int> &node_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &node_libmesh_to_gmsh_map,
//		std::unordered_map<int,int> &element_gmsh_to_libmesh_map,
//		std::unordered_map<int,int> &element_libmesh_to_gmsh_map
//		);

void build_intersection_and_restriction_tables(
		const libMesh::Parallel::Communicator& WorldComm,
		const std::string& intersection_full_table_Filename,
		const std::string& equivalence_table_A_Filename,
		const std::string& equivalence_table_B_Filename,
		std::vector<carl::IntersectionData>& intersection_full_table,
		std::unordered_map<int,int>& equivalence_table_A_to_R_A,
		std::unordered_map<int,int>& equivalence_table_B_to_R_B,
		std::unordered_map<int,int>& equivalence_table_R_A_to_A,
		std::unordered_map<int,int>& equivalence_table_R_B_to_B
		);

void generate_intersection_tables_partial(	std::string& intersection_table_restrict_B_Filename,
		std::string& intersection_table_I_Filename,
		std::unordered_map<int,int>& mesh_restrict_ElemMap,
		std::unordered_map<int,int>& mesh_micro_ElemMap,
		std::unordered_map<int,int>& mesh_inter_ElemMap,
		std::vector<std::pair<int,int> >& intersection_table_restrict_B,
		std::unordered_multimap<int,int>& intersection_table_I
		);

void generate_intersection_tables_full(		std::string& equivalence_table_restrict_A_Filename,
		std::string& intersection_table_restrict_B_Filename,
		std::string& intersection_table_I_Filename,
		std::unordered_map<int,int>& mesh_restrict_ElemMap,
		std::unordered_map<int,int>& mesh_micro_ElemMap,
		std::unordered_map<int,int>& mesh_BIG_ElemMap,
		std::unordered_map<int,int>& mesh_inter_ElemMap,
		std::unordered_map<int,int>& equivalence_table_restrict_A,
		std::vector<std::pair<int,int> >& intersection_table_restrict_B,
		std::unordered_multimap<int,int>& intersection_table_I
		);

void set_equivalence_tables(
		const libMesh::Parallel::Communicator& WorldComm,
		const std::string& equivalence_table_A_Filename,
		const std::string& equivalence_table_B_Filename,

		std::unordered_map<int,int>& equivalence_table_A_to_R_A,
		std::unordered_map<int,int>& equivalence_table_B_to_R_B,
		std::unordered_map<int,int>& equivalence_table_R_A_to_A,
		std::unordered_map<int,int>& equivalence_table_R_B_to_B );

void set_restricted_intersection_pairs_table(
		const std::unordered_map<int,std::pair<int,int> >&  full_intersection_pairs_map,
		const std::unordered_map<int,int>& equivalence_table_A_to_R_A,
		const std::unordered_map<int,int>& equivalence_table_B_to_R_B,
		std::unordered_map<int,std::pair<int,int> >& full_intersection_restricted_pairs_map);

void set_full_intersection_tables(
		const libMesh::Parallel::Communicator& WorldComm,
		const std::string& intersection_full_table_Filename,

		std::unordered_map<int,std::pair<int,int> >& full_intersection_pairs_map,
		std::unordered_map<int,int>& full_intersection_meshI_to_inter_map);

void set_intersection_tables(
		const libMesh::Parallel::Communicator& WorldComm,
		const libMesh::Mesh& mesh_intersection,
		const std::string& intersection_full_table_Filename,
		const std::string& equivalence_table_A_Filename,
		const std::string& equivalence_table_B_Filename,

		const std::unordered_map<int,int>& equivalence_table_A_to_R_A,
		const std::unordered_map<int,int>& equivalence_table_B_to_R_B,

		std::unordered_map<int,std::pair<int,int> >& full_intersection_pairs_map,
		std::unordered_map<int,std::pair<int,int> >& full_intersection_restricted_pairs_map,
		std::unordered_map<int,int>& local_intersection_meshI_to_inter_map
		);

void read_local_intersection_tables(
		const libMesh::Parallel::Communicator& WorldComm,
		const std::string& intersection_local_table_Filename,

		std::unordered_map<int,std::pair<int,int> >& local_intersection_pairs_map,
		std::unordered_map<int,int>& local_intersection_meshI_to_inter_map);

void set_local_intersection_tables(
		const libMesh::Parallel::Communicator& WorldComm,
		const libMesh::Mesh& mesh_intersection,
		const std::string& intersection_local_table_Filename,
		const std::string& equivalence_table_A_Filename,
		const std::string& equivalence_table_B_Filename,

		const std::unordered_map<int,int>& equivalence_table_A_to_R_A,
		const std::unordered_map<int,int>& equivalence_table_B_to_R_B,

		std::unordered_map<int,std::pair<int,int> >& local_intersection_pairs_map,
		std::unordered_map<int,std::pair<int,int> >& local_intersection_restricted_pairs_map,
		std::unordered_map<int,int>& local_intersection_meshI_to_inter_map
		);

void set_global_mediator_system_intersection_lists(
		const libMesh::Parallel::Communicator& WorldComm,
		const std::string& intersection_global_table_Filename,
		const std::unordered_map<int,int>& equivalence_table_system_to_mediator,
		const std::unordered_map<int,int>& equivalence_table_mediator_to_system,

		std::unordered_multimap<int,int>& inter_mediator_A,
		std::unordered_multimap<int,int>& inter_mediator_B);

void repartition_system_meshes(
		const libMesh::Parallel::Communicator& WorldComm,
		libMesh::Mesh& mesh_input,
		libMesh::Mesh& mesh_intersect,
		std::unordered_map<int,std::pair<int,int> >& local_intersection_pairs_map,
		bool bUseSecond = true);
};







#endif /* COMMON_LIBMESH_CODE_MESH_TABLES_H_ */
