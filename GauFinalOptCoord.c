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
# include <stdbool.h>

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

int const num_element_overflow = sizeof(elements_list) / sizeof(char *);

int main(int argc, char const *argv[])
{
    # define useless_lines 5
    # define max_marker_len 21
    # define ene_type_str_len 10
    int iarg = 0;
    char coordinate_determine_mark[max_marker_len + 1] = "";
    char const splitter[] = " \n";
    char in_f_name_buf[BUFSIZ + 1] = "";
    char out_f_name[BUFSIZ + 1] = "";
    char *in_f_name = in_f_name_buf;
    char c = '\0';
    FILE *in_f = NULL, *out_f = NULL;
    size_t pos = 0ll, final_pos = 0ll, last_pos = 0ll; /* last_pos is not final_pos, the final coordinates will be printed twice. */
    char line[BUFSIZ + 1] = "";
    int num_atoms = 0;
    int i = 0;
    int atom_index = 0;
    int index_coord = 0; /* x y z */
    double coor = 0.0;
    char const coordinate_determine_mark_temp[][max_marker_len + 1] = {"Input orientation:", "Standard orientation:"};
    int index_marker = 0;
    char ene_type_str[ene_type_str_len + 1] = "";
    double ene = 0.0;
    char *tok = NULL;

    memset(coordinate_determine_mark, 0, (max_marker_len + 1) * sizeof(char));
    memset(in_f_name_buf, 0, (BUFSIZ + 1) * sizeof(char));
    memset(out_f_name, 0, (BUFSIZ + 1) * sizeof(char));
    memset(line, 0, (BUFSIZ + 1) * sizeof(char));
    memset(ene_type_str, 0, (ene_type_str_len + 1) * sizeof(char));
    strcpy(coordinate_determine_mark, coordinate_determine_mark_temp[index_marker]);

    for (iarg = 1; iarg < argc; ++ iarg)
    {
        if (! strcmp(argv[iarg], "--help") || ! strcmp(argv[iarg], "-h") \
            || ! strcmp(argv[iarg], "/?"))
        {
            printf("Usage: %s [INPUT] [OUTPUT] [--std]\n", argv[0]);
            printf("\n");
            printf("If INPUT is omitted, it will be asked interactively.\n");
            printf("If OUTPUT is omitted, it will be the input name with a \".xyz\" suffix instead.\n");
            printf("If OUTPUT is \"-\", will write to stdout.\n");
            printf("If \"--std\" or \"-s\" is provided, standard orientation, \n");
            printf("instead of input orientation, will be used.\n");
            printf("\n");
            printf("Exiting normally.\n");
            exit(EXIT_SUCCESS);
        }
    }
    for (iarg = 1; iarg < argc; ++ iarg)
    {
        if (! strcmp(argv[iarg], "--std") || ! strcmp(argv[iarg], "-s"))
        {
            ++ index_marker;
            strcpy(coordinate_determine_mark, coordinate_determine_mark_temp[index_marker]);
        }
        else if (! strcmp(in_f_name_buf, ""))
        {
            strncpy(in_f_name_buf, argv[iarg], BUFSIZ + 1);
        }
        else if (! strcmp(out_f_name, ""))
        {
            strncpy(out_f_name, argv[iarg], BUFSIZ + 1);
        }
        else
        {
            fprintf(stderr, "Illegal command argument: \"%s\".\n", argv[iarg]);
            exit(EXIT_FAILURE);
        }
    }

    if (! strcmp(in_f_name_buf, ""))
    {
        printf("Input file name: \n");
        while (! fgets(in_f_name_buf, BUFSIZ, stdin))
        {
            ;
        }
        in_f_name_buf[strlen(in_f_name_buf) - 1] = '\0';
        if (* in_f_name_buf == '\"')
        {
            in_f_name_buf[strlen(in_f_name_buf) - 1] = '\0';
            ++ in_f_name;
        }
    }
    // if (strlen(in_f_name) < strlen(".out") || \
    //     strcmp(in_f_name + strlen(in_f_name) - strlen(".out"), ".out") && \
    //     strcmp(in_f_name + strlen(in_f_name) - strlen(".log"), ".log"))
    if (! strrchr(in_f_name, '.') || strcmp(strrchr(in_f_name, '.'), ".out") && strcmp(strrchr(in_f_name, '.'), ".log"))
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
            // if (strlen(out_f_name) < strlen(".xyz") || \
            //     strcmp(out_f_name + strlen(out_f_name) - strlen(".xyz"), ".xyz"))
            if (! strrchr(out_f_name, '.') || strcmp(strrchr(out_f_name, '.'), ".xyz"))
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
        out_f = fopen(out_f_name, "rt");
        if (out_f)
        {
            fclose(out_f);
            out_f = NULL;
            fprintf(stderr, "Error! File \"%s\" already exists, overwrites it? y/N\n", out_f_name);
            while (true)
            {
                if (fgets(line, BUFSIZ, stdin))
                {
                    if (strlen(line) >= 1 && line[strlen(line) - 1] == '\n')
                    {
                        line[strlen(line) - 1] = '\0';
                    }
                    if (! (strcmp(line, "Y") && strcmp(line, "y")))
                    {
                        break;
                    }
                }
                fprintf(stderr, "Aborting.\n");
                fclose(in_f);
                in_f = NULL;
                exit(EXIT_FAILURE);
                break;
            }
        }
        out_f = fopen(out_f_name, "wt");
        if (! out_f)
        {
            fprintf(stderr, "Error! Cannot open file \"%s\" for writing.\n", out_f_name);
            fclose(in_f);
            in_f = NULL;
            exit(EXIT_FAILURE);
        }
    }

    rewind(in_f);
    while (true)
    {
        if (! fgets(line, BUFSIZ, in_f))
        {
            fprintf(stderr, "Error! Cannot determine energy type.\n");
            fclose(in_f);
            in_f = NULL;
            fclose(out_f);
            out_f = NULL;
            exit(EXIT_FAILURE);
        }
        if (strstr(line, "Energy="))
        {
            strcpy(ene_type_str, "MM");
            break;
        }
        if (strstr(line, "SCF Done"))
        {
            break;
        }
    }
    if (strcmp(ene_type_str, "MM")) /* found "SCF Done" */
    {
        while (true)
        {
            if (! fgets(line, BUFSIZ, in_f) || strstr(line, "Population analysis"))
            {
                strcpy(ene_type_str, "SCF");
                break;
            }
            if (strstr(line, "EUMP2 ="))
            {
                strcpy(ene_type_str, "MP2");
                break;
            }
            if (! strncmp(line, " E2(", strlen(" E2(")))
            {
                strcpy(ene_type_str, "DFTPT2");
                break;
            }
            if (strstr(line, "E(CIS/TDA)"))
            {
                strcpy(ene_type_str, "CIS/TDA");
                break;
            }
            if (strstr(line, "E(TD-HF/TD-DFT)"))
            {
                strcpy(ene_type_str, "TD");
                break;
            }
        }
    }

    rewind(in_f);
    while (true)
    {
        if (! fgets(line, BUFSIZ, in_f))
        {
            fprintf(stderr, "Error! File \"%s\" may not be a Gaussian output file.\n", in_f_name);
            exit(EXIT_FAILURE);
        }
        if (strstr(line, "Gaussian"))
        {
            break;
        }
    }

    rewind(in_f);
    final_pos = 0ll;
    last_pos = 0ll;
    while (true)
    {
        pos = ftell(in_f);
        if (! fgets(line, BUFSIZ, in_f))
        {
            break;
        }
        if (strstr(line, coordinate_determine_mark))
        {
            last_pos = final_pos;
            final_pos = pos; /* update final position, may not really be "final". */
        }
    }
    printf("last_pos = %zd, final_pos = %zd\n", last_pos, final_pos);

    if (! final_pos)
    {
        fprintf(stderr, "Warning! Cannot find \"%s\" in Gaussian output file.\n", \
            coordinate_determine_mark_temp[index_marker]);
        index_marker = ! index_marker; /* 0 to 1, 1 to 0 */
        fprintf(stderr, "Using \"%s\" instead.\n", \
            coordinate_determine_mark_temp[index_marker]);
        rewind(in_f);
        strcpy(coordinate_determine_mark, coordinate_determine_mark_temp[index_marker]);
        while (true)
        {
            pos = ftell(in_f);
            if (! fgets(line, BUFSIZ, in_f))
            {
                break;
            }
            if (strstr(line, coordinate_determine_mark))
            {
                last_pos = final_pos;
                final_pos = pos; /* update final position, may not really be 'final'. */
            }
        }
        if (! final_pos)
        {
            fprintf(stderr, "Error! Also cannot find \"%s\" in Gaussian output file.\n", \
                coordinate_determine_mark_temp[index_marker]);
            exit(EXIT_FAILURE);
        }
    }

    rewind(in_f);
    while (true)
    {
        if (! fgets(line, BUFSIZ, in_f))
        {
            fprintf(stderr, "Error! Cannot get the amount of atoms after \"NAtoms=\".\n");
            exit(EXIT_FAILURE);
        }
        if (strstr(line, "NAtoms="))
        {
            sscanf(strstr(line, "NAtoms=") + strlen("NAtoms="), "%d", & num_atoms);
            break;
        }
    }

    /* get energy */
    fseek(in_f, last_pos, SEEK_SET);
    if (strstr(ene_type_str, "MM"))
    {
        while (fgets(line, BUFSIZ, in_f))
        {
            if (tok = strstr(line, "Energy="))
            {
                break;
            }
        }
        tok += strlen("Energy=");
        sscanf(tok, "%lg", & ene);
    }
    else if (strstr(ene_type_str, "SCF"))
    {
        while (fgets(line, BUFSIZ, in_f))
        {
            if (strstr(line, "SCF Done"))
            {
                break;
            }
        }
        tok = strchr(line, '=') + strlen("=");
        sscanf(tok, "%lg", & ene);
    }
    else if (strstr(ene_type_str, "MP2"))
    {
        while (fgets(line, BUFSIZ, in_f))
        {
            if (tok = strstr(line, "EUMP2"))
            {
                break;
            }
        }
        tok = strchr(tok, ' ') + strlen("=");
        * strchr(tok, 'D') = 'E';
        sscanf(tok, "%lg", & ene);
    }
    else if (strstr(ene_type_str, "DFTPT2"))
    {
        while (fgets(line, BUFSIZ, in_f))
        {
            if (! strncmp(line, " E2(", strlen(" E2(")))
            {
                break;
            }
        }
        tok = strstr(line, "E(");
        tok = strchr(tok, '=') + strlen("=");
        * strchr(tok, 'D') = 'E';
        sscanf(tok, "%lg", & ene);
    }
    else if (strstr(ene_type_str, "CIS/TDA"))
    {
        while (fgets(line, BUFSIZ, in_f))
        {
            if (tok = strstr(line, "E(CIS/TDA)"))
            {
                break;
            }
        }
        tok = strchr(line, '=') + strlen("=");
        sscanf(tok, "%lg", & ene);
    }
    else if (strstr(ene_type_str, "TD"))
    {
        while (fgets(line, BUFSIZ, in_f))
        {
            if (tok = strstr(line, "E(TD-HF/TD-DFT)"))
            {
                break;
            }
        }
        tok = strchr(line, '=') + strlen("=");
        sscanf(tok, "%lg", & ene);
    }
    else
    {
        /* should never happen */
        ;
    }
    tok = NULL;

    /* get coordinates */
    fseek(in_f, last_pos, SEEK_SET);
    for (i = 0; i != useless_lines; i ++)
    {
        if (! fgets(line, BUFSIZ, in_f))
        {
            fprintf(stderr, "Error! Cannot find coordinates after \"%s\".\n", coordinate_determine_mark);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(out_f, "%d\n", num_atoms);
    fprintf(out_f, "Generated from \"%s\". Energy type is \"%s\". Energy (in unit Hartree) = %17.10lf\n", \
        in_f_name, ene_type_str, ene);

    for (i = 1; i <= num_atoms; i ++)
    {
        if (! fgets(line, BUFSIZ, in_f))
        {
            fprintf(stderr, "Error! Cannot find coordinates of atom %d.\n", i);
            exit(EXIT_FAILURE);
        }
        strtok(line, splitter); /* center number */
        atom_index = 0;
        sscanf(strtok(NULL, splitter), "%d", & atom_index);
        if (atom_index >= num_element_overflow)
        {
            fprintf(stderr, "Atom Index %d overflown, using \"Bq\" as element symbol.", atom_index);
        }
        fprintf(out_f, "%1s%-2s%13s", "", atom_index < num_element_overflow ? elements_list[atom_index] : "Bq", "");
        strtok(NULL, splitter); /* atomic type */
        for (index_coord = 0; index_coord <= 2; ++ index_coord) /* x y z */
        {
            sscanf(strtok(NULL, splitter), "%lf", & coor);
            fprintf(out_f, "%2s%12.8lf", "", coor);
        }
        fprintf(out_f, "\n");
    }

    fclose(in_f);
    in_f = NULL;
    if (strcmp(out_f_name, "-"))
    {
        fclose(out_f);
    }
    out_f = NULL;

    if (argc == 1)
    {
        printf("File has been saved to \"%s\"\n.", out_f_name);
        printf("Press <Enter> to exit ...\n");
        while ((c = getchar()) != '\n' && c != EOF)
        {
            ;
        }
    }

    return 0;
}

