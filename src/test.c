#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "spglib.h"

static int test_spg_get_symmetry(void);
static int test_spg_get_symmetry_with_collinear_spin(void);
static int test_spg_get_symmetry_with_site_tensors(void);
static int test_spg_get_multiplicity(void);
static int test_spg_find_primitive_BCC(void);
static int test_spg_find_primitive_corundum(void);
static int test_spg_standardize_cell_BCC(void);
static int test_spg_standardize_cell_corundum(void);
static int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                                    int types[], const int num_atom,
                                    const double symprec,
                                    const int to_primitive,
                                    const int no_idealize);
static int test_spg_get_international(void);
static int test_spg_get_schoenflies(void);
static int test_spg_get_spacegroup_type(void);
static int test_spg_get_magnetic_spacegroup_type(void);
static int test_spg_get_symmetry_from_database(void);
static int test_spg_get_magnetic_symmetry_from_database(void);
static int test_spg_refine_cell_BCC(void);
static int test_spg_get_dataset(void);
static int test_spg_get_magnetic_dataset(void);
static int test_spg_get_magnetic_dataset_type4(void);
static int test_spg_get_ir_reciprocal_mesh(void);
static int test_spg_get_stabilized_reciprocal_mesh(void);
static int test_spg_relocate_BZ_grid_address(void);
static int test_spg_relocate_dense_BZ_grid_address(void);
static int test_spg_get_error_message(void);
static int test_spg_get_hall_number_from_symmetry(void);
static int test_spg_get_spacegroup_type_from_symmetry(void);
static int show_spg_dataset(double lattice[3][3], const double origin_shift[3],
                            double position[][3], const int num_atom,
                            const int types[]);
static void show_spg_magnetic_dataset(const SpglibMagneticDataset *dataset);
static void show_spacegroup_type(const SpglibSpacegroupType spgtype);
static void show_magnetic_spacegroup_type(
    const SpglibMagneticSpacegroupType msgtype);
static void show_cell(double lattice[3][3], double position[][3],
                      const int types[], const int num_atom);

int main(void) {
    int (*funcs[])(void) = {test_spg_find_primitive_BCC,
                            test_spg_find_primitive_corundum,
                            test_spg_standardize_cell_BCC,
                            test_spg_standardize_cell_corundum,
                            test_spg_get_multiplicity,
                            test_spg_get_symmetry,
                            test_spg_get_symmetry_with_collinear_spin,
                            test_spg_get_symmetry_with_site_tensors,
                            test_spg_get_international,
                            test_spg_get_schoenflies,
                            test_spg_get_spacegroup_type,
                            test_spg_get_spacegroup_type_from_symmetry,
                            test_spg_get_magnetic_spacegroup_type,
                            test_spg_get_symmetry_from_database,
                            test_spg_get_magnetic_symmetry_from_database,
                            test_spg_refine_cell_BCC,
                            test_spg_get_dataset,
                            test_spg_get_magnetic_dataset,
                            test_spg_get_magnetic_dataset_type4,
                            test_spg_get_ir_reciprocal_mesh,
                            test_spg_get_stabilized_reciprocal_mesh,
                            test_spg_relocate_BZ_grid_address,
                            test_spg_relocate_dense_BZ_grid_address,
                            test_spg_get_error_message,
                            test_spg_get_hall_number_from_symmetry,
                            NULL};

    int i, result;

    for (i = 0; i < 30; i++) {
        if (*funcs[i] == NULL) {
            break;
        } else {
            result = (funcs[i])();
            fflush(stdout);
        }
        if (result) {
            return 1;
        }
    }

    return 0;
}

static int test_spg_get_hall_number_from_symmetry(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int num_atom = 2;
    int retval = 0;
    double symprec = 1e-5;

    int hall_number;
    SpglibSpacegroupType spgtype;
    SpglibDataset *dataset;

    dataset = NULL;

    printf("*** spg_get_hall_number_from_symmetry ***:\n");
    if ((dataset = spg_get_dataset(lattice, position, types, num_atom,
                                   symprec)) == NULL) {
        goto end;
    }
    printf("hall_number = %d is found by spg_get_dataset.\n",
           dataset->hall_number);
    hall_number = spg_get_hall_number_from_symmetry(
        dataset->rotations, dataset->translations, dataset->n_operations,
        symprec);
    printf("hall_number = %d is found by spg_get_hall_number_from_symmetry.\n",
           hall_number);
    if (hall_number == dataset->hall_number) {
        spgtype = spg_get_spacegroup_type(hall_number);
        if (spgtype.number) {
            show_spacegroup_type(spgtype);
        } else {
            retval = 1;
        }
    }
    /* Im-3m (229) */
    assert(hall_number == 529);

    if (dataset) {
        spg_free_dataset(dataset);
    }

end:
    return retval;
}

static int test_spg_find_primitive_BCC(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int num_atom = 2;
    int num_primitive_atom;
    double symprec = 1e-5;

    /* lattice, position, and types are overwritten. */
    printf("*** spg_find_primitive (BCC unitcell --> primitive) ***:\n");
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    if (num_primitive_atom == 0) {
        printf("Primitive cell was not found.\n");
        return 1;
    } else {
        show_cell(lattice, position, types, num_primitive_atom);
        return 0;
    }
}

static int test_spg_find_primitive_corundum(void) {
    double lattice[3][3] = {{4.8076344022756095, -2.4038172011378047, 0},
                            {0, 4.1635335244786962, 0},
                            {0, 0, 13.1172699198127543}};
    double position[][3] = {
        {0.0000000000000000, 0.0000000000000000, 0.3521850942289043},
        {0.6666666666666643, 0.3333333333333357, 0.6855184275622400},
        {0.3333333333333357, 0.6666666666666643, 0.0188517608955686},
        {0.0000000000000000, 0.0000000000000000, 0.6478149057711028},
        {0.6666666666666643, 0.3333333333333357, 0.9811482391044314},
        {0.3333333333333357, 0.6666666666666643, 0.3144815724377600},
        {0.0000000000000000, 0.0000000000000000, 0.1478149057710957},
        {0.6666666666666643, 0.3333333333333357, 0.4811482391044314},
        {0.3333333333333357, 0.6666666666666643, 0.8144815724377600},
        {0.0000000000000000, 0.0000000000000000, 0.8521850942288972},
        {0.6666666666666643, 0.3333333333333357, 0.1855184275622400},
        {0.3333333333333357, 0.6666666666666643, 0.5188517608955686},
        {0.3061673906454899, 0.0000000000000000, 0.2500000000000000},
        {0.9728340573121541, 0.3333333333333357, 0.5833333333333357},
        {0.6395007239788255, 0.6666666666666643, 0.9166666666666643},
        {0.6938326093545102, 0.0000000000000000, 0.7500000000000000},
        {0.3604992760211744, 0.3333333333333357, 0.0833333333333357},
        {0.0271659426878458, 0.6666666666666643, 0.4166666666666643},
        {0.0000000000000000, 0.3061673906454899, 0.2500000000000000},
        {0.6666666666666643, 0.6395007239788255, 0.5833333333333357},
        {0.3333333333333357, 0.9728340573121541, 0.9166666666666643},
        {0.0000000000000000, 0.6938326093545102, 0.7500000000000000},
        {0.6666666666666643, 0.0271659426878458, 0.0833333333333357},
        {0.3333333333333357, 0.3604992760211744, 0.4166666666666643},
        {0.6938326093545102, 0.6938326093545102, 0.2500000000000000},
        {0.3604992760211744, 0.0271659426878458, 0.5833333333333357},
        {0.0271659426878458, 0.3604992760211744, 0.9166666666666643},
        {0.3061673906454899, 0.3061673906454899, 0.7500000000000000},
        {0.9728340573121541, 0.6395007239788255, 0.0833333333333357},
        {0.6395007239788255, 0.9728340573121541, 0.4166666666666643},
    };
    int types[30];
    int i, num_primitive_atom;
    int num_atom = 30;
    double symprec = 1e-5;

    for (i = 0; i < 12; i++) {
        types[i] = 1;
    }
    for (i = 12; i < 30; i++) {
        types[i] = 2;
    }

    /* lattice, position, and types are overwritten. */
    printf("*** spg_find_primitive (Corundum) ***:\n");
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    if (num_primitive_atom == 0) {
        printf("Primitive cell was not found.\n");
        return 1;
    } else {
        show_cell(lattice, position, types, num_primitive_atom);
        return 0;
    }
}

static int test_spg_refine_cell_BCC(void) {
    double lattice[3][3] = {{0, 2, 2}, {2, 0, 2}, {2, 2, 0}};

    /* 4 times larger memory space must be prepared. */
    double position[4][3];
    int types[4];

    int num_atom_bravais;
    int num_atom = 1;
    double symprec = 1e-5;

    position[0][0] = 0;
    position[0][1] = 0;
    position[0][2] = 0;
    types[0] = 1;

    /* lattice, position, and types are overwritten. */
    printf("*** spg_refine_cell ***:\n");
    num_atom_bravais =
        spg_refine_cell(lattice, position, types, num_atom, symprec);
    if (num_atom_bravais == 0) {
        printf("Refine cell failed.\n");
        return 1;
    } else {
        show_cell(lattice, position, types, num_atom_bravais);
        return 0;
    }
}

static int test_spg_standardize_cell_BCC(void) {
    double lattice[3][3] = {{3.97, 0, 0}, {0, 4.03, 0}, {0, 0, 4.0}};
    double position[][3] = {{0.002, 0, 0}, {0.5, 0.5001, 0.5}};
    int types[] = {1, 1};
    int j, k;
    int num_atom = 2;
    double symprec = 1e-1;

    /* lattice, position, and types are overwritten. */
    printf("*** spg_standardize_cell (BCC unitcell --> primitive) ***:\n");
    printf("------------------------------------------------------\n");
    for (j = 0; j < 2; j++) {
        for (k = 0; k < 2; k++) {
            if (sub_spg_standardize_cell(lattice, position, types, num_atom,
                                         symprec, j, k)) {
                return 1;
            }
            printf("------------------------------------------------------\n");
        }
    }

    return 0;
}

static int test_spg_standardize_cell_corundum(void) {
    double lattice[3][3] = {{4.8076344022756095, -2.4038172011378047, 0},
                            {0, 4.1635335244786962, 0},
                            {0, 0, 13.1172699198127543}};
    double position[][3] = {
        {0.0000000000000000, 0.0000000000000000, 0.3521850942289043},
        {0.6666666666666643, 0.3333333333333357, 0.6855184275622400},
        {0.3333333333333357, 0.6666666666666643, 0.0188517608955686},
        {0.0000000000000000, 0.0000000000000000, 0.6478149057711028},
        {0.6666666666666643, 0.3333333333333357, 0.9811482391044314},
        {0.3333333333333357, 0.6666666666666643, 0.3144815724377600},
        {0.0000000000000000, 0.0000000000000000, 0.1478149057710957},
        {0.6666666666666643, 0.3333333333333357, 0.4811482391044314},
        {0.3333333333333357, 0.6666666666666643, 0.8144815724377600},
        {0.0000000000000000, 0.0000000000000000, 0.8521850942288972},
        {0.6666666666666643, 0.3333333333333357, 0.1855184275622400},
        {0.3333333333333357, 0.6666666666666643, 0.5188517608955686},
        {0.3061673906454899, 0.0000000000000000, 0.2500000000000000},
        {0.9728340573121541, 0.3333333333333357, 0.5833333333333357},
        {0.6395007239788255, 0.6666666666666643, 0.9166666666666643},
        {0.6938326093545102, 0.0000000000000000, 0.7500000000000000},
        {0.3604992760211744, 0.3333333333333357, 0.0833333333333357},
        {0.0271659426878458, 0.6666666666666643, 0.4166666666666643},
        {0.0000000000000000, 0.3061673906454899, 0.2500000000000000},
        {0.6666666666666643, 0.6395007239788255, 0.5833333333333357},
        {0.3333333333333357, 0.9728340573121541, 0.9166666666666643},
        {0.0000000000000000, 0.6938326093545102, 0.7500000000000000},
        {0.6666666666666643, 0.0271659426878458, 0.0833333333333357},
        {0.3333333333333357, 0.3604992760211744, 0.4166666666666643},
        {0.6938326093545102, 0.6938326093545102, 0.2500000000000000},
        {0.3604992760211744, 0.0271659426878458, 0.5833333333333357},
        {0.0271659426878458, 0.3604992760211744, 0.9166666666666643},
        {0.3061673906454899, 0.3061673906454899, 0.7500000000000000},
        {0.9728340573121541, 0.6395007239788255, 0.0833333333333357},
        {0.6395007239788255, 0.9728340573121541, 0.4166666666666643},
    };
    int types[30];
    int i, j, k;
    int num_atom = 30;
    double symprec = 1e-5;

    for (i = 0; i < 12; i++) {
        types[i] = 1;
    }
    for (i = 12; i < 30; i++) {
        types[i] = 2;
    }

    /* lattice, position, and types are overwritten. */
    printf("*** spg_standardize_cell (Corundum) ***:\n");
    printf("------------------------------------------------------\n");
    for (j = 0; j < 2; j++) {
        for (k = 0; k < 2; k++) {
            if (sub_spg_standardize_cell(lattice, position, types, num_atom,
                                         symprec, j, k)) {
                return 1;
            }
            printf("------------------------------------------------------\n");
        }
    }

    return 0;
}

static int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                                    int types[], const int num_atom,
                                    const double symprec,
                                    const int to_primitive,
                                    const int no_idealize) {
    int i, num_primitive_atom, retval;
    double lat[3][3];
    double(*pos)[3];
    int *typ;

    pos = (double(*)[3])malloc(sizeof(double[3]) * num_atom);
    typ = (int *)malloc(sizeof(int) * num_atom);

    for (i = 0; i < 3; i++) {
        lat[i][0] = lattice[i][0];
        lat[i][1] = lattice[i][1];
        lat[i][2] = lattice[i][2];
    }

    for (i = 0; i < num_atom; i++) {
        pos[i][0] = position[i][0];
        pos[i][1] = position[i][1];
        pos[i][2] = position[i][2];
        typ[i] = types[i];
    }

    /* lattice, position, and types are overwritten. */
    num_primitive_atom = spg_standardize_cell(
        lat, pos, typ, num_atom, to_primitive, no_idealize, symprec);

    if (num_primitive_atom) {
        printf("VASP POSCAR format: ");
        if (to_primitive == 0) {
            printf("to_primitive=0 and ");
        } else {
            printf("to_primitive=1 and ");
        }

        if (no_idealize == 0) {
            printf("no_idealize=0\n");
        } else {
            printf("no_idealize=1\n");
        }
        printf("1.0\n");
        for (i = 0; i < 3; i++) {
            printf("%f %f %f\n", lat[0][i], lat[1][i], lat[2][i]);
        }
        printf("%d\n", num_primitive_atom);
        printf("Direct\n");
        for (i = 0; i < num_primitive_atom; i++) {
            printf("%f %f %f\n", pos[i][0], pos[i][1], pos[i][2]);
        }

        retval = 0;
    } else {
        retval = 1;
    }

    free(typ);
    typ = NULL;
    free(pos);
    pos = NULL;

    return retval;
}

static int test_spg_get_international(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_spg;
    int num_atom = 6;
    char symbol[21];

    num_spg =
        spg_get_international(symbol, lattice, position, types, num_atom, 1e-5);
    printf("*** spg_get_international ***:\n");
    if (num_spg > 0) {
        printf("%s (%d)\n", symbol, num_spg);
        return 0;
    } else {
        printf("Space group could not be found.\n");
        return 1;
    }
}

static int test_spg_get_schoenflies(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_atom = 6;
    char symbol[7];

    if (spg_get_schoenflies(symbol, lattice, position, types, num_atom, 1e-5)) {
        printf("*** spg_get_schoenflies ***:\n");
        printf("Schoenflies: %s\n", symbol);
        return 0;
    } else {
        return 1;
    }
}

static int test_spg_get_spacegroup_type(void) {
    SpglibSpacegroupType spgtype;
    spgtype = spg_get_spacegroup_type(446);

    printf("*** spg_get_spacegroup_type ***:\n");
    if (spgtype.number) {
        show_spacegroup_type(spgtype);
        return 0;
    } else {
        return 1;
    }
}
static int test_spg_get_spacegroup_type_from_symmetry(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int num_atom = 2;
    int retval = 0;
    double symprec = 1e-5;

    int hall_number;
    SpglibSpacegroupType spgtype;
    SpglibDataset *dataset;

    dataset = NULL;

    printf("*** spg_get_spacegroup_type_from_symmetry ***:\n");
    if ((dataset = spg_get_dataset(lattice, position, types, num_atom,
                                   symprec)) == NULL) {
        goto end;
    }
    printf("hall_number = %d is found by spg_get_dataset.\n",
           dataset->hall_number);
    spgtype = spg_get_spacegroup_type_from_symmetry(
        dataset->rotations, dataset->translations, dataset->n_operations,
        lattice, symprec);
    printf("number = %d is found by spg_get_spacegroup_type_from_symmetry.\n",
           spgtype.number);
    if (spgtype.number == dataset->spacegroup_number) {
        if (spgtype.number) {
            show_spacegroup_type(spgtype);
        } else {
            retval = 1;
        }
    }
    /* Im-3m (229) */
    assert(spgtype.number == 229);

    if (dataset) {
        spg_free_dataset(dataset);
    }

end:
    return retval;
}

static int test_spg_get_magnetic_spacegroup_type(void) {
    SpglibMagneticSpacegroupType msgtype;
    msgtype = spg_get_magnetic_spacegroup_type(1279);

    printf("*** spg_get_magnetic_spacegroup_type ***:\n");
    if (msgtype.number) {
        show_magnetic_spacegroup_type(msgtype);
        return 0;
    } else {
        return 1;
    }
}

static int test_spg_get_symmetry_from_database(void) {
    int rotations[192][3][3];
    double translations[192][3];
    int i, j, size;

    size = spg_get_symmetry_from_database(rotations, translations, 460);

    if (size) {
        printf("*** spg_get_symmetry_from_database ***:\n");
        for (i = 0; i < size; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", rotations[i][j][0], rotations[i][j][1],
                       rotations[i][j][2]);
            }
            printf("%f %f %f\n", translations[i][0], translations[i][1],
                   translations[i][2]);
        }
        return 0;
    } else {
        return 1;
    }
}

static int test_spg_get_magnetic_symmetry_from_database(void) {
    int rotations[384][3][3];
    double translations[384][3];
    int time_reversals[384];
    int i, j, size;

    /* bns_number: 146.12, uni_number 1242 */
    /* hall_number: 433 -> 146:h */
    /* hall_number: 434 -> 146:r */
    size = spg_get_magnetic_symmetry_from_database(rotations, translations,
                                                   time_reversals, 1242, 434);

    if (size) {
        printf("*** spg_get_magnetic_symmetry_from_database ***:\n");
        for (i = 0; i < size; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", rotations[i][j][0], rotations[i][j][1],
                       rotations[i][j][2]);
            }
            printf("%f %f %f\n", translations[i][0], translations[i][1],
                   translations[i][2]);
            printf("%2d\n", time_reversals[i]);
        }
        return 0;
    } else {
        return 1;
    }
}
static int test_spg_get_multiplicity(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 2};
    int num_atom = 2;
    int size;

    size = spg_get_multiplicity(lattice, position, types, num_atom, 1e-5);

    if (size) {
        printf("*** spg_get_multiplicity ***:\n");
        printf("Number of symmetry operations: %d\n", size);
        return 0;
    } else {
        return 1;
    }
}

static int test_spg_get_symmetry(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},        {0.5, 0.5, 0.25}, {0.3, 0.3, 0},    {0.7, 0.7, 0},
        {0.2, 0.8, 0.25}, {0.8, 0.2, 0.25}, {0, 0, 0.5},      {0.5, 0.5, 0.75},
        {0.3, 0.3, 0.5},  {0.7, 0.7, 0.5},  {0.2, 0.8, 0.75}, {0.8, 0.2, 0.75}};
    int types[] = {1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2};
    int num_atom = 12;
    int i, j, size, retval, max_size;
    double origin_shift[3] = {0.1, 0.1, 0};

    int(*rotation)[3][3];
    double(*translation)[3];

    max_size = num_atom * 48;
    rotation = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double(*)[3])malloc(sizeof(double[3]) * max_size);

    for (i = 0; i < num_atom; i++) {
        for (j = 0; j < 3; j++) {
            position[i][j] += origin_shift[j];
        }
    }

    printf("*** spg_get_symmetry (Rutile two unit cells) ***:\n");
    size = spg_get_symmetry(rotation, translation, max_size, lattice, position,
                            types, num_atom, 1e-5);
    if (size) {
        for (i = 0; i < size; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++)
                printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1],
                       rotation[i][j][2]);
            printf("%f %f %f\n", translation[i][0], translation[i][1],
                   translation[i][2]);
        }
        retval = 0;
    } else {
        retval = 1;
    }

    free(rotation);
    rotation = NULL;
    free(translation);
    translation = NULL;

    return retval;
}

static int test_spg_get_symmetry_with_collinear_spin(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int equivalent_atoms[2];
    double spins[2];
    int num_atom = 2;
    int i, j, size, retval, max_size;

    int(*rotation)[3][3];
    double(*translation)[3];

    max_size = num_atom * 48;
    rotation = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double(*)[3])malloc(sizeof(double[3]) * max_size);

    printf("*** spg_get_symmetry_with_spin (BCC ferro) ***:\n");
    spins[0] = 0.6;
    spins[1] = 0.6;
    size = spg_get_symmetry_with_collinear_spin(
        rotation, translation, equivalent_atoms, max_size, lattice, position,
        types, spins, num_atom, 1e-5);
    if (size) {
        for (i = 0; i < size; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1],
                       rotation[i][j][2]);
            }
            printf("%f %f %f\n", translation[i][0], translation[i][1],
                   translation[i][2]);
        }
    } else {
        retval = 1;
        goto end;
    }

    printf("*** Example of spg_get_symmetry_with_spin (BCC antiferro) ***:\n");
    spins[0] = 0.6;
    spins[1] = -0.6;
    size = spg_get_symmetry_with_collinear_spin(
        rotation, translation, equivalent_atoms, max_size, lattice, position,
        types, spins, num_atom, 1e-5);
    if (size) {
        for (i = 0; i < size; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1],
                       rotation[i][j][2]);
            }
            printf("%f %f %f\n", translation[i][0], translation[i][1],
                   translation[i][2]);
        }
    } else {
        retval = 1;
        goto end;
    }

    printf("*** spg_get_symmetry_with_spin (BCC broken spin) ***:\n");
    spins[0] = 0.6;
    spins[1] = 1.2;
    size = spg_get_symmetry_with_collinear_spin(
        rotation, translation, equivalent_atoms, max_size, lattice, position,
        types, spins, num_atom, 1e-5);
    if (size) {
        for (i = 0; i < size; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1],
                       rotation[i][j][2]);
            }
            printf("%f %f %f\n", translation[i][0], translation[i][1],
                   translation[i][2]);
        }
        retval = 0;
    } else {
        retval = 1;
    }

end:
    free(rotation);
    rotation = NULL;
    free(translation);
    translation = NULL;

    return retval;
}

static int test_spg_get_symmetry_with_site_tensors() {
    /* MAGNDATA #0.1: LaMnO3 */
    /* BNS: Pn'ma' (62.448), MHall: -P 2ac' 2n' (546) */
    int max_size, size, i, j;
    double lattice[][3] = {{5.7461, 0, 0}, {0, 7.6637, 0}, {0, 0, 5.5333}};
    /* clang-format off */
    double position[][3] = {
        {0.051300, 0.250000, 0.990500}, /* La */
        {0.948700, 0.750000, 0.009500},
        {0.551300, 0.250000, 0.509500},
        {0.448700, 0.750000, 0.490500},
        {0.000000, 0.000000, 0.500000}, /* Mn */
        {0.000000, 0.500000, 0.500000},
        {0.500000, 0.500000, 0.000000},
        {0.500000, 0.000000, 0.000000},
        {0.484900, 0.250000, 0.077700}, /* O1 */
        {0.515100, 0.750000, 0.922300},
        {0.984900, 0.250000, 0.422300},
        {0.015100, 0.750000, 0.577700},
        {0.308500, 0.040800, 0.722700}, /* O2 */
        {0.691500, 0.540800, 0.277300},
        {0.691500, 0.959200, 0.277300},
        {0.308500, 0.459200, 0.722700},
        {0.808500, 0.459200, 0.777300},
        {0.191500, 0.959200, 0.222700},
        {0.191500, 0.540800, 0.222700},
        {0.808500, 0.040800, 0.777300},
    };
    int types[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    double tensors[] = {
        0, 0, 0, /* La */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        3.87, 0, 0, /* Mn */
        -3.87, 0, 0,
        -3.87, 0, 0,
        3.87, 0, 0,
        0, 0, 0, /* O1 */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0, /* O2 */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };
    /* clang-format on */
    int num_atom = 20;

    int equivalent_atoms[20];
    double primitive_lattice[3][3];
    int(*rotation)[3][3];
    double(*translation)[3];
    int *spin_flips;

    max_size = num_atom * 96;
    rotation = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double(*)[3])malloc(sizeof(double[3]) * max_size);
    spin_flips = (int *)malloc(sizeof(int *) * max_size);

    /* Find equivalent_atoms, primitive_lattice, spin_flips */
    size = spg_get_symmetry_with_site_tensors(
        rotation, translation, equivalent_atoms, primitive_lattice, spin_flips,
        max_size, lattice, position, types, tensors, 1, num_atom,
        1 /* with_time_reversal */, 1 /* is_axial */, 1e-5);
    assert(size == 8);

    // Test spg_get_magnetic_spacegroup_type_from_symmetry
    int *time_reversals;
    time_reversals = (int *)malloc(sizeof(int *) * size);
    for (i = 0; i < size; i++) {
        time_reversals[i] = (1 - spin_flips[i]) / 2;
    }
    SpglibMagneticSpacegroupType msgtype =
        spg_get_magnetic_spacegroup_type_from_symmetry(
            rotation, translation, time_reversals, size, lattice, 1e-5);
    assert(msgtype.uni_number == 546);

    printf("*** spg_get_symmetry_with_site_tensors (type-III) ***:\n");
    for (i = 0; i < size; i++) {
        printf("--- %d ---\n", i + 1);
        for (j = 0; j < 3; j++) {
            printf("%2d %2d %2d\n", rotation[i][j][0], rotation[i][j][1],
                   rotation[i][j][2]);
        }
        printf("%f %f %f\n", translation[i][0], translation[i][1],
               translation[i][2]);
        printf("%d\n", spin_flips[i]);
    }

    free(rotation);
    rotation = NULL;
    free(translation);
    translation = NULL;
    free(spin_flips);
    spin_flips = NULL;
    free(time_reversals);
    time_reversals = NULL;

    return 0;
}

static int test_spg_get_dataset(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double origin_shift[3] = {0.1, 0.1, 0};
    double position[][3] = {
        {0, 0, 0},        {0.5, 0.5, 0.25}, {0.3, 0.3, 0},    {0.7, 0.7, 0},
        {0.2, 0.8, 0.25}, {0.8, 0.2, 0.25}, {0, 0, 0.5},      {0.5, 0.5, 0.75},
        {0.3, 0.3, 0.5},  {0.7, 0.7, 0.5},  {0.2, 0.8, 0.75}, {0.8, 0.2, 0.75}};
    int types[] = {1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2};
    int num_atom = 12;

    double lattice_2[3][3] = {{3.7332982433264039, -1.8666491216632011, 0},
                              {0, 3.2331311186244847, 0},
                              {0, 0, 6.0979971306362799}};
    double origin_shift_2[3] = {0.1, 0.1, 0};
    double position_2[][3] = {
        {0, 0, 0},
        {1.0 / 3, 2.0 / 3, 0.4126},
        {1.0 / 3, 2.0 / 3, 0.776},
        {2.0 / 3, 1.0 / 3, 0.2542},
    };
    int types_2[] = {1, 2, 3, 3};
    int num_atom_2 = 4;

    printf("*** spg_get_dataset (Rutile two unit cells) ***:\n");

    if (show_spg_dataset(lattice, origin_shift, position, num_atom, types) ==
        1) {
        return 1;
    }

    if (show_spg_dataset(lattice_2, origin_shift_2, position_2, num_atom_2,
                         types_2)) {
        return 1;
    }

    return 0;
}

static int test_spg_get_magnetic_dataset(void) {
    /* Rutile structure (P4_2/mnm) */
    /* Generators: -y+1/2,x+1/2,z+1/2; -x+1/2,y+1/2,-z+1/2; -x,-y,-z */
    double lattice[3][3] = {{5, 0, 0}, {0, 5, 0}, {0, 0, 3}};
    double position[][3] = {
        /* Ti (2a) */
        {0, 0, 0},
        {0.5, 0.5, 0.5},
        /* O (4f) */
        {0.3, 0.3, 0},
        {0.7, 0.7, 0},
        {0.2, 0.8, 0.5},
        {0.8, 0.2, 0.5},
    };
    int types[] = {1, 1, 2, 2, 2, 2};
    double spins[6];
    int num_atom = 6;
    SpglibMagneticDataset *dataset;

    /* Type-I, 136.495: -P 4n 2n */
    {
        printf("*** spg_get_magnetic_dataset (type-I, ferro) ***:\n");
        spins[0] = 0.3;
        spins[1] = 0.3;
        spins[2] = 0;
        spins[3] = 0;
        spins[4] = 0;
        spins[5] = 0;
        dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                           0 /* tensor_rank */, num_atom,
                                           0 /* is_axial */, 1e-5);
        assert(dataset->msg_type == 1);
        assert(dataset->uni_number == 1155);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }

    /* Type-II, "136.496": -P 4n 2n 1' */
    {
        printf("*** spg_get_magnetic_dataset (type-II, gray) ***:\n");
        spins[0] = 0;
        spins[1] = 0;
        spins[2] = 0;
        spins[3] = 0;
        spins[4] = 0;
        spins[5] = 0;
        dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                           0 /* tensor_rank */, num_atom,
                                           0 /* is_axial */, 1e-5);
        assert(dataset->msg_type == 2);
        assert(dataset->uni_number == 1156);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }

    /* Type-III, "136.498": -P 4n' 2n' */
    {
        printf("*** spg_get_magnetic_dataset (type-III, antiferro) ***:\n");
        spins[0] = 0.7;
        spins[1] = -0.7;
        spins[2] = 0;
        spins[3] = 0;
        spins[4] = 0;
        spins[5] = 0;
        dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                           0 /* tensor_rank */, num_atom,
                                           0 /* is_axial */, 1e-5);
        assert(dataset->msg_type == 3);
        assert(dataset->uni_number == 1158);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }
    return 0;
}

static int test_spg_get_magnetic_dataset_type4(void) {
    /* double Rutile structure (P4_2/mnm) */
    double lattice[3][3] = {{5, 0, 0}, {0, 5, 0}, {0, 0, 6}};
    double position[][3] = {
        /* Ti (2a) */
        {0, 0, 0},
        {0.500001, 0.5, 0.25}, /* Test with small displacement */
        /* O (4f) */
        {0.3, 0.3, 0},
        {0.7, 0.7, 0},
        {0.2, 0.8, 0.25},
        {0.8, 0.2, 0.25},
        /* Ti (2a) */
        {0, 0, 0.5},
        {0.5, 0.5, 0.75},
        /* O (4f) */
        {0.3, 0.3, 0.5},
        {0.7, 0.7, 0.5},
        {0.2, 0.8, 0.75},
        {0.8, 0.2, 0.75},
    };
    int types[] = {1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2};
    double spins[] = {0.300001, 0.299999, 0, 0, 0, 0, -0.3, -0.3, 0, 0, 0, 0};
    int num_atom = 12;
    SpglibMagneticDataset *dataset;

    printf("*** spg_get_magnetic_dataset_type4 ***:\n");

    /* "136.504": -P 4n 2n 1c' */
    dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                       0 /* tensor_rank */, num_atom,
                                       0 /* is_axial */, 1e-5);
    assert(dataset->msg_type == 4);
    assert(dataset->uni_number == 932);
    show_spg_magnetic_dataset(dataset);

    spg_free_magnetic_dataset(dataset);
    return 0;
}

static int test_spg_get_ir_reciprocal_mesh(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int num_ir, retval;
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_atom = 6;
    int m = 40;
    int mesh[3];
    int is_shift[] = {1, 1, 1};
    int(*grid_address)[3];
    int *grid_mapping_table;

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;
    grid_address = (int(*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (int *)malloc(sizeof(int) * m * m * m);

    printf("*** spg_get_ir_reciprocal_mesh of Rutile structure ***:\n");

    num_ir = spg_get_ir_reciprocal_mesh(grid_address, grid_mapping_table, mesh,
                                        is_shift, 1, lattice, position, types,
                                        num_atom, 1e-5);

    if (num_ir) {
        printf("Number of irreducible k-points of Rutile with\n");
        printf("40x40x40 Monkhorst-Pack mesh is %d (4200).\n", num_ir);
        retval = 0;
    } else {
        retval = 1;
    }

    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;
    return retval;
}

static int test_spg_get_stabilized_reciprocal_mesh(void) {
    int retval = 0;
    SpglibDataset *dataset;
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int num_ir;
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_atom = 6;
    int m = 40;
    int mesh[3];
    int is_shift[] = {1, 1, 1};
    int(*grid_address)[3];
    int *grid_mapping_table;
    double q[] = {0, 0.5, 0.5};

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;

    /* Memory spaces have to be allocated to pointers */
    /* to avoid Invalid read/write error by valgrind. */
    grid_address = (int(*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (int *)malloc(sizeof(int) * m * m * m);

    dataset = spg_get_dataset(lattice, position, types, num_atom, 1e-5);

    if (dataset == NULL) {
        retval = 1;
        goto end;
    }

    printf("*** spg_get_stabilized_reciprocal_mesh of Rutile structure ***:\n");

    num_ir = spg_get_stabilized_reciprocal_mesh(
        grid_address, grid_mapping_table, mesh, is_shift, 1,
        dataset->n_operations, dataset->rotations, 1, (double(*)[3])q);

    spg_free_dataset(dataset);
    dataset = NULL;

    if (num_ir) {
        printf(
            "Number of irreducible k-points stabilized by q=(0, 1/2, 1/2) of "
            "Rutile with\n");
        printf("40x40x40 Monkhorst-Pack mesh is %d (8000).\n", num_ir);
    } else {
        retval = 1;
    }

end:
    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;
    return retval;
}

static int test_spg_relocate_BZ_grid_address(void) {
    double rec_lattice[3][3] = {{-0.17573761, 0.17573761, 0.17573761},
                                {0.17573761, -0.17573761, 0.17573761},
                                {0.17573761, 0.17573761, -0.17573761}};
    int rotations[][3][3] = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

    int retval = 0;
    int(*bz_grid_address)[3], (*grid_address)[3];
    int *grid_mapping_table, *bz_map;

    int num_ir, num_q;
    int m = 40;
    int mesh[3];
    int is_shift[] = {0, 0, 0};
    double q[] = {0, 0, 0};

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;

    /* Memory spaces have to be allocated to pointers */
    /* to avoid Invalid read/write error by valgrind. */
    bz_grid_address =
        (int(*)[3])malloc(sizeof(int[3]) * (m + 1) * (m + 1) * (m + 1));
    bz_map = (int *)malloc(sizeof(int) * m * m * m * 8);
    grid_address = (int(*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (int *)malloc(sizeof(int) * m * m * m);

    num_ir = spg_get_stabilized_reciprocal_mesh(
        grid_address, grid_mapping_table, mesh, is_shift, 1, 1, rotations, 1,
        (double(*)[3])q);
    if (num_ir) {
        printf("*** spg_relocate_BZ_grid_address of NaCl structure ***:\n");

        num_q = spg_relocate_BZ_grid_address(
            bz_grid_address, bz_map, grid_address, mesh, rec_lattice, is_shift);

        printf("Number of k-points of NaCl Brillouin zone\n");
        printf(
            "with Gamma-centered 40x40x40 Monkhorst-Pack mesh is %d (65861).\n",
            num_q);
    } else {
        retval = 1;
    }

    free(bz_grid_address);
    bz_grid_address = NULL;
    free(bz_map);
    bz_map = NULL;
    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;

    return retval;
}

static int test_spg_relocate_dense_BZ_grid_address(void) {
    double rec_lattice[3][3] = {{-0.17573761, 0.17573761, 0.17573761},
                                {0.17573761, -0.17573761, 0.17573761},
                                {0.17573761, 0.17573761, -0.17573761}};
    int rotations[][3][3] = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

    int retval = 0;
    int(*bz_grid_address)[3], (*grid_address)[3];
    size_t *grid_mapping_table, *bz_map;

    size_t num_ir, num_q;
    int m = 40;
    int mesh[3];
    int is_shift[] = {0, 0, 0};
    double q[] = {0, 0, 0};

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;

    /* Memory spaces have to be allocated to pointers */
    /* to avoid Invalid read/write error by valgrind. */
    bz_grid_address =
        (int(*)[3])malloc(sizeof(int[3]) * (m + 1) * (m + 1) * (m + 1));
    bz_map = (size_t *)malloc(sizeof(size_t) * m * m * m * 8);
    grid_address = (int(*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (size_t *)malloc(sizeof(size_t) * m * m * m);

    num_ir = spg_get_dense_stabilized_reciprocal_mesh(
        grid_address, grid_mapping_table, mesh, is_shift, 1, 1, rotations, 1,
        (double(*)[3])q);
    if (num_ir) {
        printf(
            "*** spg_relocate_dense_BZ_grid_address of NaCl structure ***:\n");

        num_q = spg_relocate_dense_BZ_grid_address(
            bz_grid_address, bz_map, grid_address, mesh, rec_lattice, is_shift);

        printf("Number of k-points of NaCl Brillouin zone\n");
        printf(
            "with Gamma-centered 40x40x40 Monkhorst-Pack mesh is %lu "
            "(65861).\n",
            num_q);
    } else {
        retval = 1;
    }

    free(bz_grid_address);
    bz_grid_address = NULL;
    free(bz_map);
    bz_map = NULL;
    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;

    return retval;
}

static int test_spg_get_error_message(void) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1, 1};
    int num_atom = 3;
    int num_primitive_atom;
    double symprec = 1e-5;
    SpglibError error;

    /* lattice, position, and types are overwritten. */
    printf("*** Example of spg_get_error_message ***:\n");
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    if (num_primitive_atom == 0) {
        printf("Primitive cell was not found.\n");
        error = spg_get_error_code();
        printf("%s\n", spg_get_error_message(error));
        if (error == SPGLIB_SUCCESS) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 1;
    }
}

static int show_spg_dataset(double lattice[3][3], const double origin_shift[3],
                            double position[][3], const int num_atom,
                            const int types[]) {
    SpglibDataset *dataset;
    char ptsymbol[6];
    int pt_trans_mat[3][3];
    int i, j;
    int retval = 0;
    const char *wl = "abcdefghijklmnopqrstuvwxyz";

    for (i = 0; i < num_atom; i++) {
        for (j = 0; j < 3; j++) {
            position[i][j] += origin_shift[j];
        }
    }

    dataset = spg_get_dataset(lattice, position, types, num_atom, 1e-5);

    if (dataset == NULL) {
        retval = 1;
        goto end;
    }

    printf("International: %s (%d)\n", dataset->international_symbol,
           dataset->spacegroup_number);
    printf("Hall symbol:   %s\n", dataset->hall_symbol);
    if (spg_get_pointgroup(ptsymbol, pt_trans_mat, dataset->rotations,
                           dataset->n_operations)) {
        printf("Point group:   %s\n", ptsymbol);
        printf("Transformation matrix:\n");
        for (i = 0; i < 3; i++) {
            printf("%f %f %f\n", dataset->transformation_matrix[i][0],
                   dataset->transformation_matrix[i][1],
                   dataset->transformation_matrix[i][2]);
        }
        printf("Wyckoff letters:\n");
        for (i = 0; i < dataset->n_atoms; i++) {
            printf("%c ", wl[dataset->wyckoffs[i]]);
        }
        printf("\n");
        printf("Equivalent atoms:\n");
        for (i = 0; i < dataset->n_atoms; i++) {
            printf("%d ", dataset->equivalent_atoms[i]);
        }
        printf("\n");

        for (i = 0; i < dataset->n_operations; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", dataset->rotations[i][j][0],
                       dataset->rotations[i][j][1],
                       dataset->rotations[i][j][2]);
            }
            printf("%f %f %f\n", dataset->translations[i][0],
                   dataset->translations[i][1], dataset->translations[i][2]);
        }
    } else {
        retval = 1;
    }

    if (dataset) {
        spg_free_dataset(dataset);
    }

end:
    return retval;
}

static void show_spg_magnetic_dataset(const SpglibMagneticDataset *dataset) {
    int i, p, s;
    printf("UNI number: %d\n", dataset->uni_number);
    printf("Type: %d\n", dataset->msg_type);
    printf("Hall number: %d\n", dataset->hall_number);

    printf("\nSymmetry operations\n");
    for (p = 0; p < dataset->n_operations; p++) {
        printf("--- %d ---\n", p + 1);
        for (s = 0; s < 3; s++) {
            printf("%2d %2d %2d\n", dataset->rotations[p][s][0],
                   dataset->rotations[p][s][1], dataset->rotations[p][s][2]);
        }
        printf("%f %f %f\n", dataset->translations[p][0],
               dataset->translations[p][1], dataset->translations[p][2]);
        printf("%d\n", dataset->time_reversals[p]);
    }

    printf("\nEquivalent atoms:\n");
    for (i = 0; i < dataset->n_atoms; i++) {
        printf(" %d", dataset->equivalent_atoms[i]);
    }
    printf("\n");

    printf("\nTransformation matrix:\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->transformation_matrix[s][0],
               dataset->transformation_matrix[s][1],
               dataset->transformation_matrix[s][2]);
    }
    printf("Origin shift:\n");
    printf("%f %f %f\n", dataset->origin_shift[0], dataset->origin_shift[1],
           dataset->origin_shift[2]);

    printf("\nStandardization\n");
    printf("Rigid rotation\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->std_rotation_matrix[s][0],
               dataset->std_rotation_matrix[s][1],
               dataset->std_rotation_matrix[s][2]);
    }
    printf("Lattice\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->std_lattice[s][0],
               dataset->std_lattice[s][1], dataset->std_lattice[s][2]);
    }
    printf("Positions, types, site tensors \n");
    for (i = 0; i < dataset->n_std_atoms; i++) {
        printf("[%f %f %f], types=%d, ", dataset->std_positions[i][0],
               dataset->std_positions[i][1], dataset->std_positions[i][2],
               dataset->std_types[i]);
        if (dataset->tensor_rank == 0) {
            printf("%f\n", dataset->std_tensors[i]);
        } else if (dataset->tensor_rank == 1) {
            printf("%f %f %f \n", dataset->std_tensors[i * 3],
                   dataset->std_tensors[i * 3 + 1],
                   dataset->std_tensors[i * 3 + 2]);
        }
    }
}

static void show_spacegroup_type(const SpglibSpacegroupType spgtype) {
    printf("Number:            %d\n", spgtype.number);
    printf("International:     %s\n", spgtype.international_short);
    printf("International:     %s\n", spgtype.international_full);
    printf("International:     %s\n", spgtype.international);
    printf("Schoenflies:       %s\n", spgtype.schoenflies);
    printf("Hall symbol:       %s\n", spgtype.hall_symbol);
    printf("Point group intl:  %s\n", spgtype.pointgroup_international);
    printf("Point group Schoe: %s\n", spgtype.pointgroup_schoenflies);
    printf("Arithmetic cc num. %d\n", spgtype.arithmetic_crystal_class_number);
    printf("Arithmetic cc sym. %s\n", spgtype.arithmetic_crystal_class_symbol);
}

static void show_magnetic_spacegroup_type(
    const SpglibMagneticSpacegroupType msgtype) {
    printf("UNI Number:    %d\n", msgtype.uni_number);
    printf("Litvin Number: %d\n", msgtype.litvin_number);
    printf("BNS Number:    %s\n", msgtype.bns_number);
    printf("OG Number:     %s\n", msgtype.og_number);
    printf("Number:        %d\n", msgtype.number);
    printf("Type:          %d\n", msgtype.type);
}

static void show_cell(double lattice[3][3], double position[][3],
                      const int types[], const int num_atom) {
    int i;

    printf("Lattice parameter:\n");
    for (i = 0; i < 3; i++) {
        printf("%f %f %f\n", lattice[0][i], lattice[1][i], lattice[2][i]);
    }
    printf("Atomic positions:\n");
    for (i = 0; i < num_atom; i++) {
        printf("%d: %f %f %f\n", types[i], position[i][0], position[i][1],
               position[i][2]);
    }
}
