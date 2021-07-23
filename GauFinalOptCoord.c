/*
 **********************************************************************************
 * This program reads the last geometry from a Gaussian optimization output file. *
 **********************************************************************************
 * Usage: GauFinalOptCoord.exe [INPUT] [OUTPUT] [--std]                           *
 **********************************************************************************
 * If INPUT is omitted, it will be asked interactively.                           *
 * If OUTPUT is omitted, it will be "output.xyz".                                 *
 * If OUTPUT is "-", will write to stdout.                                        *
 * If "--std" or "-s" is provided, standard orientation,                          *
 * instead of input orientation, will be used.                                    *
 **********************************************************************************
*/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>

char const *elements_list[] = {"", \
 "H" , "He", "Li", "Be", "B" , "C" , "N" , "O" , \
 "F" , "Ne", "Na", "Mg", "Al", "Si", "P" , "S" , \
 "Cl", "Ar", "K" , "Ca", "Sc", "Ti", "V" , "Cr", \
 "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", \
 "As", "Se", "Br", "Kr", "Rb", "Sr", "Y" , "Zr", \
 "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", \
 "In", "Sn", "Sb", "Te", "I" , "Xe", "Cs", "Ba", \
 "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", \
 "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", "Hf", \
 "Ta", "W" , "Re", "Os", "Ir", "Pt", "Au", "Hg", \
 "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", \
 "Ac", "Th", "Pa", "U" , "Np", "Pu", "Am", "Cm", \
 "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr", "Rf", \
 "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn", \
 "Nh", "Fl", "Mc", "Lv", "Ts", "Og"};

unsigned int const num_element_overflow = sizeof(elements_list) / sizeof(char *);

int main(int argc, char const *argv[])
{
    # define useless_lines 5u
    # define max_marker_len 21
    unsigned int iarg = 0u;
    char coordinate_determine_mark[max_marker_len + 1] = "";
    char const splitter[] = " \n";
    char in_f_name_buf[BUFSIZ + 1] = "";
    char out_f_name[BUFSIZ + 1] = "";
    char *in_f_name = in_f_name_buf;
    char c = '\0';
    FILE *in_f = NULL, *out_f = NULL;
    long pos = 0, final_pos = 0;
    char line[BUFSIZ + 1] = "";
    unsigned int num_atoms = 0;
    unsigned int i = 0;
    unsigned int atom_index = 0;
    unsigned int index_coord = 0u; /* x y z */
    double coor = 0.0;
    char const coordinate_determine_mark_temp[2][max_marker_len + 1] = {"Input orientation:", "Standard orientation:"};
    unsigned int index_marker = 0u;

    memset(coordinate_determine_mark, 0, (max_marker_len + 1) * sizeof(char));
    memset(in_f_name_buf, 0, (BUFSIZ + 1) * sizeof(char));
    memset(out_f_name, 0, (BUFSIZ + 1) * sizeof(char));
    memset(line, 0, (BUFSIZ + 1) * sizeof(char));
    strcpy(coordinate_determine_mark, coordinate_determine_mark_temp[index_marker]);

    for (iarg = 1u; iarg < argc; ++ iarg)
    {
        if (! strcmp(argv[iarg], "--help") || ! strcmp(argv[iarg], "-h") \
            || ! strcmp(argv[iarg], "/?"))
        {
            printf("Usage: %s [INPUT] [OUTPUT] [--std]\n", argv[0]);
            puts("");
            puts("If INPUT is omitted, it will be asked interactively.");
            puts("If OUTPUT is omitted, it will be the input name with a \".xyz\" suffix instead.");
            puts("If OUTPUT is \"-\", will write to stdout.");
            puts("If \"--std\" or \"-s\" is provided, standard orientation,");
            puts("instead of input orientation, will be used.");
            puts("");
            puts("Exiting normally.");
            exit(EXIT_SUCCESS);
        }
    }
    for (iarg = 1u; iarg < argc; ++ iarg)
    {
        if (! strcmp(argv[iarg], "--std") || ! strcmp(argv[iarg], "-s"))
        {
            ++ index_marker;
            strcpy(coordinate_determine_mark, coordinate_determine_mark_temp[index_marker]);
        }
        else if (! strcmp(in_f_name_buf, ""))
            strncpy(in_f_name_buf, argv[iarg], BUFSIZ);
        else if (! strcmp(out_f_name, ""))
            strncpy(out_f_name, argv[iarg], BUFSIZ);
        else
        {
            fprintf(stderr, "Illegal command argument: \"%s\".\n", argv[iarg]);
            exit(EXIT_FAILURE);
        }
    }

    if (! strcmp(in_f_name_buf, ""))
    {
        puts("Input file name: ");
        if (! fgets(in_f_name_buf, BUFSIZ, stdin))
            exit(EXIT_FAILURE);
        in_f_name_buf[strlen(in_f_name_buf) - 1] = '\0';
        if (* in_f_name_buf == '\"')
        {
            in_f_name_buf[strlen(in_f_name_buf) - 1] = '\0';
            ++ in_f_name;
        }
    }
    if (strlen(in_f_name) < strlen(".out") || \
        strcmp(in_f_name + strlen(in_f_name) - strlen(".out"), ".out") && \
        strcmp(in_f_name + strlen(in_f_name) - strlen(".log"), ".log"))
    {
        fprintf(stderr, "Error! The suffix of the input file must be either \".out\" or \".log\".\n");
        exit(EXIT_FAILURE);
    }
    in_f = fopen(in_f_name, "rt");
    if (! in_f)
    {
        fprintf(stderr, "Error! File \"%s\" not found.\n", in_f_name);
        exit(EXIT_FAILURE);
    }
    if (! strcmp(out_f_name, "-"))
        out_f = stdout;
    else
    {
        if (strcmp(out_f_name, ""))
        {
            if (strlen(out_f_name) < strlen(".xyz") || \
                strcmp(out_f_name + strlen(out_f_name) - strlen(".xyz"), ".xyz"))
            {
                fprintf(stderr, "Error! The suffix of the output file must be \".xyz\".\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            strcpy(out_f_name, in_f_name);
            strcpy(out_f_name + strlen(out_f_name) - strlen(".xyz"), ".xyz");
        }
        out_f = fopen(out_f_name, "wt");
        if (! out_f)
        {
            fprintf(stderr, "Error! Cannot open file \"%s\" for writing.\n", out_f_name);
            exit(EXIT_FAILURE);
        }
    }

    for (;;)
    {
        if (! fgets(line, BUFSIZ, in_f))
            exit(EXIT_FAILURE);
        if (! strncmp(line, " NAtoms=", strlen(" NAtoms=")))
        {
            sscanf(line + strlen(" NAtoms="), "%u", & num_atoms);
            break;
        }
    }

    fseek(in_f, 0, SEEK_SET);
    final_pos = -1;
    for (;;)
    {
        pos = ftell(in_f);
        if (! fgets(line, BUFSIZ, in_f))
            break;
        if (strstr(line, coordinate_determine_mark))
            final_pos = pos; /* update final position, may not really be 'final'. */
    }
    if (final_pos < 0)
    {
        fprintf(stderr, "Warning! Cannot find \"%s\" in Gaussian output file.\n", \
            coordinate_determine_mark_temp[index_marker]);
        index_marker = index_marker + 1 & 1u;
        fprintf(stderr, "Using \"%s\" instead.\n", \
            coordinate_determine_mark_temp[index_marker]);
        fseek(in_f, 0, SEEK_SET);
        strcpy(coordinate_determine_mark, coordinate_determine_mark_temp[index_marker]);
        for (;;)
        {
            pos = ftell(in_f);
            if (! fgets(line, BUFSIZ, in_f))
                break;
            if (strstr(line, coordinate_determine_mark))
                final_pos = pos; /* update final position, may not really be 'final'. */
        }
        if (final_pos < 0)
        {
            fprintf(stderr, "Error! Also cannot find \"%s\" in Gaussian output file.\n", \
                coordinate_determine_mark_temp[index_marker]);
            exit(EXIT_FAILURE);
        }
    }


    fseek(in_f, final_pos, SEEK_SET);
    for (i = 0; i < useless_lines; i ++)
        fgets(line, BUFSIZ, in_f); /* useless */
    fprintf(out_f, "%u\n", num_atoms);
    fprintf(out_f, "Generated from %s\n", in_f_name);

    for (i = 1; i <= num_atoms; i ++)
    {
        fgets(line, BUFSIZ, in_f);
        strtok(line, splitter); /* center number */
        atom_index = 0u;
        sscanf(strtok(NULL, splitter), "%u", & atom_index);
        if (atom_index >= num_element_overflow)
            fprintf(stderr, "Atom Index %u overflown, using \"Bq\" as element symbol.", atom_index);
        fprintf(out_f, "%1s%-2s%13s", "", atom_index < num_element_overflow ? elements_list[atom_index] : "Bq", "");
        strtok(NULL, splitter); /* atomic type */
        for (index_coord = 0u; index_coord <= 2; ++ index_coord) /* x y z */
        {
            sscanf(strtok(NULL, splitter), "%lf", & coor);
            fprintf(out_f, "%2s%12.8lf", "", coor);
        }
        fprintf(out_f, "\n");
    }

    fclose(in_f);
    in_f = NULL;
    if (strcmp(out_f_name, "-"))
        fclose(out_f);
    out_f = NULL;

    if (argc == 1)
    {
        printf("File has been saved to \"%s\"\n.", out_f_name);
        puts("Press <Enter> to exit ...");
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }

    return 0;
}

