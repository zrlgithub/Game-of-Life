#define _CRT_SECURE_NO_WARNINGS

#define ON 1 //celula vie
#define OFF 0 //celula moarta

#define for_i for(int i=0;i<nr_lin;i++)  //folosite doar pentru compararea configuratiilor
#define for_j for(int j=0;j<nr_col;j++)
                                   
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <Windows.h>

                                        // // G A M E  OF  L I F E \\ \\

/*
!
!Nu s-au folosit variabile globale.
!Programul gestionează memoria într-o manieră dinamică. 
!Pentru modul SUPER DINAMIC am folosit doua functii pentru a numara liniile si coloanele
!Pentru modul COMPRIMAT am folosit un nou fisier pentru o verificare mai rapida a programului si am afisat rezultatul
in acelasi fisier de iesire ca in modul normal.
!
*/

void configuratie_initiala(FILE *FISIER, int steps);  //folosim pentru configuratia de inceput 
int **configuratie_finala(int **configuratie, int &nr_lin, int &nr_col); //pentru generatiile urmatoare
int  **creare_configuratie(int numar_linii, int numar_coloane); //alocarea dinamica a matricei
void  distrugere_configuratie(int **configuratie, int linii);  //dezalocare matrice
void afisare_configuratie(int **configuratie, int nr_lin, int nr_col); //afisarea in fisier a ultimei configuratii
int numare_vecini(int **configuratie, int linie, int coloana, int linii, int coloane); //numarul de vecini al fiecarei celule
void vizualizare_configuratie(int **configuratie, int nr_lin, int nr_col); //pentru vizualizarea generatiilor urmatoare de celule
int numarare_linii(FILE *fisier);//pentru super-dinamic  -Nu se da nr de linii si de coloane
int numarare_coloane(FILE *fisier);//super-dinamic
void functie_super_dinamic(int steps);
void configuratie_initiala_dinamic(FILE *file_in, int steps, int numar_linii, int numar_coloane);
void functie_comprimat();
int comparare_configuratii(int **conf1, int **conf2,int nr_lin, int nr_col);//pentru determinarea configuratiei stabile


void main(int argc, char *argv[])
{
	FILE *file_in, *file_out, *text;
	int argument, steps=NULL;
	char *in=NULL, *out=NULL;

	int super_dinamic = OFF, comprimat = OFF, stabil = OFF;
	for (int argument = 0; argument < argc; argument++)    //tratarea parametrilor in orice ordine
	{
		if (strcmp(argv[argument], "-in") == 0 and strstr(argv[argument + 1], ".txt"))
			in = _strdup(argv[argument + 1]);
		if (strcmp(argv[argument], "-out") == 0 and strstr(argv[argument + 1], ".txt"))
			out = _strdup(argv[argument + 1]);

		if (strcmp(argv[argument], "-steps") == 0)
			steps = atoi(argv[argument + 1]);

		if (strcmp(argv[argument], "-super_dinamic")==0)
			super_dinamic = ON;
		if (strcmp(argv[argument], "-comprimat")==0)
			comprimat = ON;
	}
	 
	if (super_dinamic == ON)
		functie_super_dinamic(steps);
	else
		if (comprimat == ON)
		{
			if (in == NULL )
			{
				file_out = fopen(out, "w");
				fprintf(file_out, "Fisierul nu exista");
				fclose(file_out);
			}
			functie_comprimat();
		}
		else {
			file_in = fopen(in, "r");
			if (in == NULL or file_in == NULL)
			{
				file_out = fopen("file_out.txt", "w");
				fprintf(file_out, "Fisierul nu exista");
				fclose(file_out);
			}

			configuratie_initiala(file_in, steps);



			fclose(file_in);
		}
	
}
void configuratie_initiala(FILE *file_in,int steps)
{
	int **configuratie,**configuratie_stabila;
	int numar_linii, numar_coloane, contor, linie, coloana, vietate;
	int *aux, pasul = 0,ok=0,stabil=0,c=0; //pentru configuratie stabila
	aux = (int*)malloc(steps * sizeof(int));

	fscanf(file_in, "%d", &numar_linii);
	fscanf(file_in, "%d", &numar_coloane);
	assert(numar_linii >= 3 and numar_coloane >= 3); //verificarea numarului de linii si coloane

	configuratie = creare_configuratie(numar_linii, numar_coloane);
	configuratie_stabila = creare_configuratie(numar_linii, numar_coloane);

	//initializam cu configuratia initiala pentru a verifica momentul cand se realizeaza configuratia stabila
	for (linie = 0; linie < numar_linii; linie++)
		for (coloana = 0; coloana < numar_coloane; coloana++) {
			fscanf(file_in, "%d", &configuratie[linie][coloana]);
			configuratie_stabila[linie][coloana] = configuratie[linie][coloana];
		}
	afisare_configuratie(configuratie, numar_linii, numar_coloane);
	while (steps)
	{
		int **noua_configuratie = configuratie_finala(configuratie, numar_linii, numar_coloane);
		distrugere_configuratie(configuratie, numar_linii);
		configuratie = noua_configuratie;
		pasul++;
		stabil = comparare_configuratii(noua_configuratie, configuratie_stabila, numar_linii, numar_coloane);
		if (stabil == 1){ //daca se gaseste aceeasi configuratie cu cea initiala	
			ok = 1;
			*(aux+c)=pasul; //salvam pasul
			c++; 
		}
		afisare_configuratie(configuratie, numar_linii, numar_coloane);
		vizualizare_configuratie(configuratie, numar_linii, numar_coloane);
		steps--;
		Sleep(800);
	}
	//afisarea in fisierul de iesire al pasului de configuratie stabila
	if (ok == 1)
	{
		FILE *fisier = fopen("file_out.txt", "w"); //prima configuratie stabila gasita
		fprintf(fisier, "Configuratie stabila la pasul: %d si s-a repetat de %d ori.", *(aux),c);
		fclose(fisier);
	}
	else {
		FILE *fisier = fopen("file_out.txt", "w");
		fprintf(fisier, "Nu s-a gasit configuratie stabila");
		fclose(fisier);}
	free(aux);
	distrugere_configuratie(configuratie, numar_linii);
}
int **configuratie_finala(int **configuratie, int &nr_lin, int &nr_col)
{
	int** noua_configuratie = (int**)malloc(nr_lin * sizeof(int*));     //alocare pentru noua configuratie
	for (int i = 0; i < nr_lin; i++)
		noua_configuratie[i] = (int*)malloc(nr_col * sizeof(int));

	for (int linie = 0; linie < nr_lin; linie++)    //initializam configuratia cu 0
	{
		for (int coloana = 0; coloana < nr_col; coloana++)
		{
			noua_configuratie[linie][coloana] = 0;
		}
	}
	for (int linie = 0; linie < nr_lin; linie++)
	{
		for (int coloana = 0; coloana < nr_col; coloana++)
		{
			int viata = numare_vecini(configuratie, linie, coloana, nr_lin, nr_col); //reprezinta numarul de vecini
		 
		//folosim regulile pentru a verifica daca celula urmeaza sa fie ON sau OFF in generatia urmatoare
			if (viata < 2)
				noua_configuratie[linie][coloana] = 0;

			else if ((viata == 2 or viata == 3) and configuratie[linie][coloana] == ON)
				noua_configuratie[linie][coloana] = ON;

			else if (viata > 3 and configuratie[linie][coloana] == ON)
				noua_configuratie[linie][coloana] = OFF;

			else if (viata == 3 and configuratie[linie][coloana] == OFF)
				noua_configuratie[linie][coloana] = ON;
		}
	}
	return noua_configuratie;
}
int numare_vecini(int **configuratie, int x, int y, int linii, int coloane)
{
	int nr_vecini = 0;
	//Alocam o matrice mai mare in care copiem vecinii elementelor aflate in primul chenar
	int **configuratie_marita;
	configuratie_marita = creare_configuratie(linii + 2, coloane + 2);
	int n1 = linii, m1 = coloane;
	linii += 2;
	coloane += 2;
	for (int i = 1; i < linii - 1; i++)
		for (int j = 1; j <coloane- 1; j++)
			configuratie_marita[i][j] = configuratie[i - 1][j - 1];

	for (int i = 0; i < n1; i++)
		configuratie_marita[i + 1][0] = configuratie[i][m1 - 1];

	for (int i = 0; i < m1; i++)
		configuratie_marita[0][i + 1] = configuratie[n1 - 1][i];

	for (int i = 0; i < m1; i++)
		configuratie_marita[linii - 1][i + 1] = configuratie[0][i];
	for (int i = 0; i < n1; i++) 
		configuratie_marita[i + 1][coloane - 1] = configuratie[i][0];

	configuratie_marita[0][0] = configuratie[n1 - 1][m1 - 1];
	configuratie_marita[0][coloane- 1] = configuratie[n1 - 1][0];
	configuratie_marita[linii - 1][0] = configuratie[0][m1 - 1];
	configuratie_marita[linii - 1][coloane- 1] = configuratie[0][0];
	//Luam vecinii folosind coordonatele celulei
	for (int i = x - 1; i <= (x + 1); i++)
	{
		for (int j = y - 1; j <= (y + 1); j++)
		{
			if (configuratie_marita[i+1][j+1])
				nr_vecini++;
		}
	
         }
    if (configuratie_marita[x+1][y+1]) //Pentru ca verificam inclusiv celula in sine.Daca este 1 se scade nr de vecini
       nr_vecini--;

	return nr_vecini;
	//Dezalocam matricea folosita pentru numarare
	distrugere_configuratie(configuratie_marita, linii + 2);
} 
int **creare_configuratie(int numar_linii, int numar_coloane)
{   //Subprogram alocare dinamica configuratie
	int **configuratie;
	configuratie = (int**)malloc(numar_linii * sizeof(int*));
	if (configuratie == NULL)
		printf("Nu s-a putut configura!");
	for (int contor = 0; contor < numar_linii; contor++)
	{
		configuratie[contor] = (int*)malloc(numar_coloane * sizeof(int));
		if (configuratie[contor] == NULL)
			printf("Nu s-a format configuratia");
	}
	return configuratie;
}
void distrugere_configuratie(int **configuratie, int linii)
{
	//Subprogram dezalocare dinamica configuratie
	for (int linie = 0; linie < linii; linie++)
		free(configuratie[linie]);
	free(configuratie);
}
void afisare_configuratie(int **configuratie, int nr_lin, int nr_col)
{
	FILE *file_out;
	//afisarea ultimei configuratii in fisierul text de iesire
	file_out = fopen("file_out.txt", "wt");
	
	for (int contor = 0; contor < nr_lin; contor++)
	{
		for (int contor1 = 0; contor1 < nr_col; contor1++)
			fprintf(file_out,"%d ", configuratie[contor][contor1]);
		fprintf(file_out,"\n");
	}
   
	fclose(file_out);
}
void vizualizare_configuratie(int **configuratie, int nr_lin, int nr_col)
{
	//afisarea pe consola a generatiilor
	system("cls");
	for (int contor = 0; contor < nr_lin; contor++)
	{
		for (int contor1 = 0; contor1 < nr_col; contor1++)
			if(configuratie[contor][contor1]==1)
				printf("*");
			else printf(" ");
		printf("\n");
	}

}
int numarare_linii(FILE *fisier)
{
	//functie pentru numararea liniilor dintr-un fisier
	char *linie =(char*) malloc(sizeof(char) * 1024);
	char *p;
	int numar_linii = 0, numar_coloane = 0;
	while (fgets(linie, 1024, fisier) != NULL)
	{
		 numar_linii++;
		p = linie;
		while (*p)
		{
			while (*p and isspace(*p)) //pentru spatiile dintre cifre 
				p++;

			if (*p == '\0')
				break;
			numar_coloane++;

			while (*p and !isspace(*p))
				p++;
		}

	}
	return numar_linii;
	free(linie);
}
int numarare_coloane(FILE *fisier)
{
	//functie pentru numararea coloanelor dintr-un fisier
	char *linie = (char*)malloc(sizeof(char) * 1024);
	char *p;
	int numar_linii = 0, numar_coloane = 0;
	fgets(linie, 1024, fisier);
	
		numar_linii++;
		p = linie;
		while (*p)
		{
			while (*p and isspace(*p))
				p++;

			if (*p == '\0')
				break;
			numar_coloane++;

			while (*p and !isspace(*p))
				p++;
		}

	
	return numar_coloane;
}
void functie_super_dinamic(int steps)
{  //Se foloseste alt fisier
	FILE *fisier = fopen("Text.txt", "r");
	int numar_linii,numar_coloane;
	numar_linii = numarare_linii(fisier);
	fclose(fisier);
	fisier = fopen("Text.txt", "r");
	numar_coloane = numarare_coloane(fisier);
	fclose(fisier);
	fisier = fopen("Text.txt", "r");
	configuratie_initiala_dinamic(fisier, steps,numar_linii,numar_coloane);
}
void configuratie_initiala_dinamic(FILE *file_in, int steps,int numar_linii,int numar_coloane)
{
	//Am folosit acelasi subprogram pentru formarea si afisarea generatiilor, modificand doar parametrii
	// Functia SUPER DINAMIC activa

	int **configuratie;
	int contor, linie, coloana, vietate;
	int *aux, pasul = 0, ok = 0, stabil = 0,c=0; //pentru configuratie stabila in modul SUPER DINAMIC
	aux = (int*)malloc(steps * sizeof(int));
	configuratie = creare_configuratie(numar_linii, numar_coloane);

	for (linie = 0; linie < numar_linii; linie++)
		for (coloana = 0; coloana < numar_coloane; coloana++)
			fscanf(file_in, "%d", &configuratie[linie][coloana]);


	afisare_configuratie(configuratie, numar_linii, numar_coloane);

	while (steps)
	{
		int **noua_configuratie = configuratie_finala(configuratie, numar_linii, numar_coloane);
		distrugere_configuratie(configuratie, numar_linii);
		configuratie = noua_configuratie;
		pasul++;
		if (stabil == 1)    //daca se gaseste aceeasi configuratie cu cea initiala
		{
			ok = 1;
			*(aux+c) = pasul; //salvam pasul
			c++;
		}
		afisare_configuratie(configuratie, numar_linii, numar_coloane);
		vizualizare_configuratie(configuratie, numar_linii, numar_coloane);
		steps--;
		Sleep(800);
	}
	if (ok == 1)
	{
		//afisarea in fisierul de iesire al pasului pentru configuratia stabila
		FILE *fisier = fopen("file_out.txt", "w");
		//prima configuratie stabila gasita
		fprintf(fisier, "Configuratie stabila la pasul: %d si s-a repetat de %d ori.", *aux,c); 
		fclose(fisier);
	}
	else {
		FILE *fisier = fopen("file_out.txt", "w");
		fprintf(fisier, "Nu s-a gasit configuratie stabila");
		fclose(fisier);
	}
	distrugere_configuratie(configuratie, numar_linii);
}
void functie_comprimat()
{
	//am folosit alt text de intrare pentru a verifica mai usor executia modului COMPRIMAT
	int **configuratie;
	FILE *fisier,*fisieriesire;
	fisier = fopen("Fisier_comprimat.txt", "r" );
	fisieriesire = fopen("file_out.txt", "w");

	if (fisier == NULL){
		printf("Fisierul nu exista");
		fclose(fisieriesire);
	}

	int nr_linii, nr_coloane;
	int lin, col;
	fscanf(fisier, "%d %d", &nr_linii, &nr_coloane);
	configuratie = (int**)malloc(nr_linii * sizeof(int*));     //alocare pentru configuratie
	for (int i = 0; i < nr_linii; i++)
		configuratie[i] = (int*)malloc(nr_coloane * sizeof(int));

	for (int linie = 0; linie < nr_linii; linie++) {   //initializam configuratia cu 0
		for (int coloana = 0; coloana < nr_coloane; coloana++){
			configuratie[linie][coloana] = 0;
		}
	}
	//citim linia si coloana pentru vietate pana cand se termina fisierul
	while(!feof(fisier))
	{
		fscanf(fisier, "%d %d", &lin, &col);
		configuratie[lin][col] = ON;
	}

    afisare_configuratie(configuratie, nr_linii, nr_coloane);

}
int comparare_configuratii(int **conf1, int** conf2,int nr_lin,int nr_col)
{
	//functie pentru compararea configuratiilor in vederea stabilirii configuratiei stabile
	for_i
		for_j
		if (conf1[i][j] != conf2[i][j])
		{
			return 0;
			break;
		}

	return 1;
}