./carl_libmesh -mA meshes/3D/test_multi_macro.msh -mB meshes/3D/test_multi_micro_w_0_2__p_30.msh -mI meshes/3D/output/test_multi_I_w_0_2__p_30_full.msh -mR meshes/3D/output/test_multi_restriction_full.msh -mW meshes/3D/test_restriction_mask.msh --tableA meshes/equivalence_tables/equivalence_multi_carl_restrict_A_full.dat --tableB meshes/equivalence_tables/intersection_multi_carl_restrict_B_w_0_2__p_30_full.dat --tableI meshes/equivalence_tables/intersection_multi_carl_I_w_0_2__p_30_full.dat -p data/3D/test_multi_micro_w_0_2__p_30_physical.dat --weightIdx data/3D/test_multi_weight_idx.dat -oA meshes/3D/output/carl_multi_crystal_test_macro_w_0_2__p_30_full.exo -oB meshes/3D/output/carl_multi_crystal_test_micro_w_0_2__p_30_full.exo 
