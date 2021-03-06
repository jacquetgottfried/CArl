#include "main.h"

/*
 * 		Sketch of the second version of the program ----------------------------
 *
 * 		-> Read the meshes A, B and the coupling region file.
 *
 * 		-> Read the intersection mesh I.
 *
 * 		-> Read the mediator mesh, M.
 *
 * 		-> Read the mesh restrictions, R_A and R_B, and the equivalence tables
 * 		   between them and the original meshes, t_R_A->A and t_R_B->B.
 *
 * 		-> A, B, M and I are partitioned over all processors, while each one
 * 		   will have a copy of R_A and R_B.
 *
 * 		TODO : For now, each processor will read the whole mesh R_A and R_B!
 * 		       This is not practical for large systems!
 *
 * 		-> Run the coupling assemble program using the intersection mesh as the
 * 		   main loop index.
 *
 *		-> The reduced meshes will be used to calculate the values, while the
 *		   equivalence tables will be used to convert them to the full meshes.
 *
 *		-> Solve the system using the LATIN method. Since the matrices are
 *		   associated to A, B and M (which were properly partitioned), no
 *		   further steps are needed to paralellize the solver.
 */

int main(int argc, char** argv) {

	// - Start libmesh ---------------------------------------------------------
	const bool MASTER_bPerfLog_carl_libmesh = true;
	libMesh::LibMeshInit init(argc, argv);

	libMesh::PerfLog perf_log("Main program", MASTER_bPerfLog_carl_libmesh);

	// - Displacement conditions -----------------------------------------------
	boundary_displacement y_max_BIG(0,1.0,0);
	boundary_displacement y_min_BIG(-0.25,0,0);
	boundary_id_cube boundary_ids;

	// - Set up inputs
	GetPot command_line(argc, argv);
	GetPot field_parser;
	std::string input_filename;

	if (command_line.search(1, "--inputfile")) {
		input_filename = command_line.next(input_filename);
		field_parser.parse_input_file(input_filename, "#", "\n", " \t\n");
	} else {
		field_parser = command_line;
	}

	carl::coupling_generation_input_params input_params;
	carl::get_input_params(field_parser, input_params);

	const unsigned int dim = 3;

	libmesh_example_requires(dim == LIBMESH_DIM, "3D support");

	// Set up the communicator and the rank variables
	libMesh::Parallel::Communicator& WorldComm = init.comm();
	libMesh::Parallel::Communicator LocalComm;

	int rank = WorldComm.rank();
	int nodes = WorldComm.size();

	WorldComm.split(rank,rank,LocalComm);

	// - Read the meshes -------------------------------------------------------

	// - Parallelized meshes: A, B, mediator and weight

	perf_log.push("Meshes - Parallel","Read files:");
	libMesh::Mesh mesh_BIG(WorldComm, dim);
	mesh_BIG.read(input_params.mesh_BIG_file);
	mesh_BIG.prepare_for_use();

	libMesh::Mesh mesh_micro(WorldComm, dim);
	mesh_micro.read(input_params.mesh_micro_file);
	mesh_micro.prepare_for_use();

	libMesh::Mesh mesh_mediator(WorldComm, dim);
	mesh_mediator.allow_renumbering(false);
	mesh_mediator.read(input_params.mesh_mediator_file);
	mesh_mediator.prepare_for_use();

	libMesh::Mesh mesh_weight(WorldComm, dim);
	mesh_weight.allow_renumbering(false);
	mesh_weight.read(input_params.mesh_weight_file);
	mesh_weight.prepare_for_use();

//	// DEBUG - Test: print info per proc
//	{
//		std::ofstream mesh_info_ofstream;
//		mesh_info_ofstream.open("meshes/parallel_test/output/mesh_A_" + std::to_string(rank) + "_info.txt");
//		mesh_BIG.print_info(mesh_info_ofstream);
//		mesh_info_ofstream.close();
//
//		WorldComm.barrier();
//
//		mesh_info_ofstream.open("meshes/parallel_test/output/mesh_B_" + std::to_string(rank) + "_info.txt");
//		mesh_micro.print_info(mesh_info_ofstream);
//		mesh_info_ofstream.close();
//
//		WorldComm.barrier();
//
//		mesh_info_ofstream.open("meshes/parallel_test/output/mesh_inter_" + std::to_string(rank) + "_info.txt");
//		mesh_inter.print_info(mesh_info_ofstream);
//		mesh_info_ofstream.close();
//
//		WorldComm.barrier();
//	}

	perf_log.pop("Meshes - Parallel","Read files:");

	// - Local meshes: restrict A and restrict B

	// -> libMesh's default mesh IS the SerialMesh, which creates a copy of
	//    itself on each processor, but partitions the iterators over each
	//    processor to allow the parallelization of the code. The usage of
	//    ParallelMesh is still a bit risky, and, performance-wise, is worse
	//    than SerialMesh for less than a few thousand processors.

	perf_log.push("Meshes - Serial","Read files:");
	libMesh::ReplicatedMesh mesh_R_BIG(WorldComm, dim);
	mesh_R_BIG.allow_renumbering(false);
	mesh_R_BIG.read(input_params.mesh_restrict_BIG_file);
	mesh_R_BIG.prepare_for_use();

	libMesh::ReplicatedMesh mesh_R_micro(WorldComm, dim);
	mesh_R_micro.allow_renumbering(false);
	mesh_R_micro.read(input_params.mesh_restrict_micro_file);
	mesh_R_micro.prepare_for_use();

//	// DEBUG - Test: print info per proc
//	{
//		std::ofstream mesh_info_ofstream;
//		mesh_info_ofstream.open("meshes/parallel_test/output/mesh_RA_" + std::to_string(rank) + "_info.txt");
//		mesh_R_BIG.print_info(mesh_info_ofstream);
//		mesh_info_ofstream.close();
//
//		WorldComm.barrier();
//
//		mesh_info_ofstream.open("meshes/parallel_test/output/mesh_RB_" + std::to_string(rank) + "_info.txt");
//		mesh_R_micro.print_info(mesh_info_ofstream);
//		mesh_info_ofstream.close();
//
//		WorldComm.barrier();
//
//		mesh_info_ofstream.open("meshes/parallel_test/output/mesh_mediator_" + std::to_string(rank) + "_info.txt");
//		mesh_mediator.print_info(mesh_info_ofstream);
//		mesh_info_ofstream.close();
//
//		std::ofstream mesh_data;
//		mesh_data.open("meshes/parallel_test/output/mesh_A_data_" + std::to_string(rank)  + ".dat");
//		libMesh::MeshBase::const_element_iterator       el     = mesh_BIG.active_local_elements_begin();
//		const libMesh::MeshBase::const_element_iterator end_el = mesh_BIG.active_local_elements_end();
//
//		for ( ; el != end_el; ++el)
//		{
//			const libMesh::Elem* elem = *el;
//			mesh_data << elem->id() << " " << elem->point(0) << " " << elem->point(1) << " " << elem->point(2)<< " " << elem->point(3)<< std::endl;
//		}
//		mesh_data.close();
//	}

	// - Local mesh: intersection mesh
	libMesh::Mesh mesh_inter(LocalComm, dim);
	mesh_inter.allow_renumbering(false);
	std::string local_inter_mesh_filename = input_params.mesh_inter_file + "_r_"
										+ std::to_string(rank) + "_n_" + std::to_string(nodes) + ".e";
	std::string local_inter_table_filename = input_params.intersection_table_full + "_r_"
										+ std::to_string(rank) + "_n_" + std::to_string(nodes) + "_inter_table.dat";
	std::string global_inter_table_filename = input_params.intersection_table_full + "_global_inter_pairs.dat";

	mesh_inter.read(local_inter_mesh_filename);
	mesh_inter.prepare_for_use();

	perf_log.pop("Meshes - Serial","Read files:");

	/*
	 * 		To do on the first proc
	 * 		- Read and build the equivalence tables between R_X and X, e_X - DONE
	 * 		- Read and build the intersection pairs table between A and B, p_AB - DONE
	 *		- Read and build the intersection indexes table, I_F - DONE
	 *
	 * 		Broadcast e_X, p_AB, I_F - DONE
	 *
	 * 		Build on each proc
	 * 		- The restricted intersection pairs table, p_R,AB- DONE
	 * 		- A local intersection indexes table, I_L - DONE
	 *
	 * 		Convert the pairs table to the libMesh indexing - DONE
	 */

	perf_log.push("Equivalence / intersection tables","Read files:");
	std::unordered_map<int,std::pair<int,int> > local_intersection_pairs_map;
	std::unordered_map<int,std::pair<int,int> > local_intersection_restricted_pairs_map;
	std::unordered_map<int,int> local_intersection_meshI_to_inter_map;

	std::unordered_map<int,int> equivalence_table_BIG_to_R_BIG;
	std::unordered_map<int,int> equivalence_table_micro_to_R_micro;
	std::unordered_map<int,int> equivalence_table_R_BIG_to_BIG;
	std::unordered_map<int,int> equivalence_table_R_micro_to_micro;

	//	Start by reading and broadcasting the equivalence tables
	carl::set_equivalence_tables(
			WorldComm,
			input_params.equivalence_table_restrict_BIG_file,
			input_params.equivalence_table_restrict_micro_file,

			equivalence_table_BIG_to_R_BIG,
			equivalence_table_micro_to_R_micro,
			equivalence_table_R_BIG_to_BIG,
			equivalence_table_R_micro_to_micro);

	if(input_params.b_UseMesh_BIG_AsMediator)
	{
		carl::set_local_intersection_tables(
				WorldComm,
				mesh_inter,
				local_inter_table_filename,
				input_params.equivalence_table_restrict_BIG_file,
				input_params.equivalence_table_restrict_micro_file,

				equivalence_table_BIG_to_R_BIG,
				equivalence_table_micro_to_R_micro,

				local_intersection_pairs_map,
				local_intersection_restricted_pairs_map,
				local_intersection_meshI_to_inter_map);
	}
	else if(input_params.b_UseMesh_micro_AsMediator)
	{
		carl::set_local_intersection_tables(
				WorldComm,
				mesh_inter,
				local_inter_table_filename,
				input_params.equivalence_table_restrict_micro_file,
				input_params.equivalence_table_restrict_BIG_file,

				equivalence_table_micro_to_R_micro,
				equivalence_table_BIG_to_R_BIG,

				local_intersection_pairs_map,
				local_intersection_restricted_pairs_map,
				local_intersection_meshI_to_inter_map);
	}

	std::unordered_multimap<int,int> inter_mediator_BIG;
	std::unordered_multimap<int,int> inter_mediator_micro;

	if(input_params.b_Repartition_micro)
		carl::repartition_system_meshes(WorldComm,mesh_micro,mesh_BIG,local_intersection_pairs_map);

	carl::set_global_mediator_system_intersection_lists(
			WorldComm,
			global_inter_table_filename,
			equivalence_table_BIG_to_R_BIG,
			equivalence_table_R_BIG_to_BIG,
			inter_mediator_BIG,
			inter_mediator_micro);

	perf_log.pop("Equivalence / intersection tables","Read files:");

	perf_log.push("Weight function domain","Read files:");

	// Set weight functions
	int domain_Idx_BIG = -1;
	int nb_of_domain_Idx = 1;
	std::vector<int> domain_Idx_micro;
	std::vector<int> domain_Idx_coupling;

	carl::set_weight_function_domain_idx(	input_params.weight_domain_idx_file,
											domain_Idx_BIG, nb_of_domain_Idx,
											domain_Idx_micro, domain_Idx_coupling
											);

	WorldComm.barrier();

	perf_log.pop("Weight function domain","Read files:");

	// - Generate the equation systems -----------------------------------------
	perf_log.push("Initialization","System initialization:");
	carl::coupled_system CoupledTest(WorldComm,input_params.solver_type);

	libMesh::EquationSystems& equation_systems_inter =
					CoupledTest.add_inter_EquationSystem("InterSys", mesh_inter);

	// Add the weight function mesh
	CoupledTest.add_alpha_mask("MicroSys",mesh_weight);
	CoupledTest.set_alpha_mask_parameters("MicroSys",domain_Idx_BIG,domain_Idx_micro[0],domain_Idx_coupling[0]);

	perf_log.pop("Initialization","System initialization:");

	// - Build the BIG system --------------------------------------------------

	perf_log.push("Macro system","System initialization:");
	libMesh::EquationSystems& equation_systems_BIG =
					CoupledTest.set_BIG_EquationSystem("BigSys", mesh_BIG);

	// [MACRO] Set up the physical properties
	libMesh::LinearImplicitSystem& elasticity_system_BIG
										= add_elasticity(equation_systems_BIG);

	// [MACRO] Defining the boundaries with Dirichlet conditions
	set_displaced_border_translation(elasticity_system_BIG, y_max_BIG,boundary_ids.MAX_Y);
	set_clamped_border(elasticity_system_BIG, boundary_ids.MIN_Y);

	// [MACRO] Build stress system
	libMesh::ExplicitSystem& stress_system_BIG
										= add_stress(equation_systems_BIG);

	equation_systems_BIG.init();

	perf_log.pop("Macro system","System initialization:");

	// - Build the micro system ------------------------------------------------

	perf_log.push("Micro system","System initialization:");

	libMesh::EquationSystems& equation_systems_micro =
					CoupledTest.add_micro_EquationSystem<libMesh::PetscMatrix<libMesh::Number> >("MicroSys", mesh_micro);

	// [MICRO] Set up the physical properties
	libMesh::LinearImplicitSystem& elasticity_system_micro
										= add_elasticity(equation_systems_micro);

	// [MICRO] Build stress system
	libMesh::ExplicitSystem& stress_system_micro
										= add_stress(equation_systems_micro);

	equation_systems_micro.init();
	perf_log.pop("Micro system","System initialization:");

	// - Build the RESTRICTED BIG system ---------------------------------------

	perf_log.push("RESTRICTED macro system","System initialization:");
	libMesh::EquationSystems& equation_systems_R_BIG =
					CoupledTest.set_Restricted_BIG_EquationSystem("BigSys", mesh_R_BIG);

	// [R. MACRO] Set up the physical properties
	libMesh::ExplicitSystem& elasticity_system_R_BIG
										= add_explicit_elasticity(equation_systems_R_BIG);

	carl::reduced_system_init(elasticity_system_R_BIG);

	perf_log.pop("RESTRICTED macro system","System initialization:");

	// - Build the RESTRICTED micro system ------------------------------------------------

	perf_log.push("RESTRICTED micro system","System initialization:");

	libMesh::EquationSystems& equation_systems_R_micro =
					CoupledTest.add_Restricted_micro_EquationSystem("MicroSys", mesh_R_micro);

	// [R. MICRO] Set up the physical properties
	libMesh::ExplicitSystem& elasticity_system_R_micro
										= add_explicit_elasticity(equation_systems_R_micro);

	carl::reduced_system_init(elasticity_system_R_micro);
	
	perf_log.pop("RESTRICTED micro system","System initialization:");

	// - Build the mediator system ---------------------------------------------

	perf_log.push("Mediator system","System initialization:");

	libMesh::EquationSystems& equation_systems_mediator =
					CoupledTest.add_mediator_EquationSystem("MediatorSys", mesh_mediator);

	libMesh::LinearImplicitSystem& elasticity_system_mediator
										= add_elasticity(equation_systems_mediator);

	equation_systems_mediator.init();

	WorldComm.barrier();

	perf_log.pop("Mediator system","System initialization:");

	// - Build the dummy inter system ------------------------------------------

	perf_log.push("Intersection system","System initialization:");

	libMesh::ExplicitSystem& elasticity_system_inter
										= add_explicit_elasticity(equation_systems_inter);

	carl::reduced_system_init(elasticity_system_inter);

	WorldComm.barrier();

	perf_log.pop("Intersection system","System initialization:");

	perf_log.push("Physical properties","System initialization:");
	double BIG_E = 0;
	double BIG_Mu = 0;

	double coupling_const = -1;
	std::ifstream phys_params_file(input_params.physical_params_file);
	phys_params_file >> BIG_E >> BIG_Mu;
	phys_params_file.close();

	set_constant_physical_properties(equation_systems_micro,BIG_E,BIG_Mu);
	set_constant_physical_properties(equation_systems_BIG,BIG_E,BIG_Mu);
	
	perf_log.pop("Physical properties","System initialization:");

	// - Set the coupling matrix -----------------------------------------------
	perf_log.push("Set coupling matrices");
	coupling_const = BIG_E;
	CoupledTest.set_coupling_parameters("MicroSys",coupling_const,input_params.mean_distance);

	CoupledTest.use_H1_coupling("MicroSys");
	CoupledTest.assemble_coupling_elasticity_3D_parallel("BigSys","MicroSys",
			"InterSys","MediatorSys",
			mesh_R_BIG, mesh_R_micro,
			local_intersection_pairs_map,
			local_intersection_restricted_pairs_map,
			local_intersection_meshI_to_inter_map,
			inter_mediator_BIG,
			inter_mediator_micro);

	std::cout << std::endl;
	std::cout << "| ---> Constants " << std::endl;
	std::cout << "| Macro :" << std::endl;
	std::cout << "|    E            : " << BIG_E << std::endl;
	std::cout << "|    Mu (lamba_2) : " << BIG_Mu << std::endl;
	std::cout << "|    lambda_1     : " << eval_lambda_1(BIG_E,BIG_Mu) << std::endl;
	std::cout << "| Coupling :" << std::endl;
	std::cout << "|    kappa        : " << coupling_const << std::endl;
	std::cout << "|    e            : " << input_params.mean_distance << std::endl;

	switch(input_params.solver_type)
	{
		case carl::LATIN_MODIFIED_STIFFNESS:
		case carl::LATIN_ORIGINAL_STIFFNESS:
		{
			std::cout << "| LATIN :" << std::endl;
			std::cout << "|    k_dA, k_dB   : " << input_params.k_dA << " " << input_params.k_dB << std::endl;
			std::cout << "|    k_cA, k_cB   : " << input_params.k_cA << " " << input_params.k_cB << std::endl;
			break;
		}
		case carl::CG:
		{
			break;
		}
	}
	perf_log.pop("Set coupling matrices");
	
	std::cout << "| restart file    : " << input_params.coupled_restart_file_base << "*" << std::endl;

	std::cout << std::endl << "| --> Testing the solver " << std::endl << std::endl;
	perf_log.push("Set up","Coupled Solver:");
	if(input_params.b_PrintRestartFiles || input_params.b_UseRestartFiles)
	{
		CoupledTest.set_restart(	input_params.b_UseRestartFiles,
				input_params.b_PrintRestartFiles,
				input_params.coupled_restart_file_base);
	}
	std::cout << std::endl << "| --> Setting the solver " << std::endl << std::endl;

	switch(input_params.solver_type)
	{
		case carl::LATIN_MODIFIED_STIFFNESS:
		case carl::LATIN_ORIGINAL_STIFFNESS:
		{
			CoupledTest.set_LATIN_solver(	"MicroSys","Elasticity",
									assemble_elasticity_with_weight,
									assemble_elasticity_with_weight,
									input_params.k_dA, input_params.k_dB, input_params.k_cA, input_params.k_cB,
									input_params.LATIN_eps, input_params.LATIN_conv_max, input_params.LATIN_relax);
			break;
		}
		case carl::CG:
		{
			CoupledTest.use_null_space_micro("MicroSys",true);
			CoupledTest.set_cg_preconditioner_type(input_params.CG_precond_type);
			CoupledTest.set_CG_solver(	"MicroSys","Elasticity",
											assemble_elasticity_with_weight,
											assemble_elasticity_with_weight,
											input_params.CG_coupled_conv_abs,input_params.CG_coupled_conv_rel,
											input_params.CG_coupled_conv_max,input_params.CG_coupled_div,
											input_params.CG_coupled_conv_corr);
			break;
		}
	}

	perf_log.pop("Set up","Coupled Solver:");


	// Solve !
	perf_log.push("Solve","Coupled Solver:");
	CoupledTest.solve("MicroSys","Elasticity",input_params.coupled_convergence_output);
	perf_log.pop("Solve","Coupled Solver:");

	// Calculate stress
	perf_log.push("Compute stress - micro","Output:");
	compute_stresses(equation_systems_micro);
	perf_log.pop("Compute stress - micro","Output:");

	perf_log.push("Compute stress - macro","Output:");
	compute_stresses(equation_systems_BIG);
	perf_log.pop("Compute stress - macro","Output:");

	// Export solution
#ifdef LIBMESH_HAVE_EXODUS_API
	if(input_params.b_PrintOutput)
	{
		perf_log.push("Save output","Output:");
		libMesh::ExodusII_IO exo_io_micro(mesh_micro, /*single_precision=*/true);

		std::set<std::string> system_names_micro;
		system_names_micro.insert("Elasticity");
		exo_io_micro.write_equation_systems(input_params.output_file_micro,equation_systems_micro,&system_names_micro);

		exo_io_micro.write_element_data(equation_systems_micro);

		libMesh::ExodusII_IO exo_io_BIG(mesh_BIG, /*single_precision=*/true);

		std::set<std::string> system_names_BIG;
		system_names_BIG.insert("Elasticity");
		exo_io_BIG.write_equation_systems(input_params.output_file_BIG,equation_systems_BIG,&system_names_BIG);

		exo_io_BIG.write_element_data(equation_systems_BIG);
		perf_log.pop("Save output","Output:");
	}
#endif

	if(input_params.b_ExportScalingData)
	{
		CoupledTest.print_perf_log(input_params.scaling_data_file);
	}

	return 0;
}
