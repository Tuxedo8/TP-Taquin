#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int nbr_grille=0; // variable globale pour avoir sa valeur dans la fonction d'import et pouvoir l'utiliser dans le menu


char directions[] = {'h','b','g','d'};  // tableau de char contenant les directions de mouvements possibles dans le jeu

struct grille {

	int **tab;
	int taille;
	int i_libre;
	int j_libre ;

};
typedef struct grille grille ;


int menu();
void play_user_melange();
void play(grille*);
int verif(grille*);



//parametre : G pointeur sur une grille
//affiche la grille de taquin (le contenu de g->tab)
//renvoi : rien
void displayGrille(grille* G){
	for(int i=0;i<G->taille;i++ ){
		for(int j=0;j<G->taille;j++ ){
			if(G->tab[i][j]==0){
				printf("|X|"); // on represente la case vide
			}else{
				printf("|%d|",G->tab[i][j]);
			}
		}
		printf("\n");
	}
	printf("\n");
}


//parametre : g pointeur sur une grille
//libere la grille de taquin et son contenu
//renvoi : rien
void libereGrille(grille* g){

	for (int i=0; i<g->taille ;i++){
		free(g->tab[i]);	// on libere tab
	}
	free(g->tab);
	free(g);
}
//parametres : -g: tableau de grilles 
//		-taille : la taille du tableau de grilles
//libere le tableau de grilles de taquin, son contenu, et le contenu de chaque grille
//renvoi : rien
void libereTabGrille(grille ** g, int taille){
	for (int i=0; i<taille ;i++){
		libereGrille(g[i]);	//on libere chaque grille
	}
	free(g);
	
}
//parametre :taille : la taille du tableau 2D voulu
//alloue un tableau 2D d'entiers  de taille "taillextaille" rempli de zeros
//renvoi : table2D un pointeur sur le tableau 2D d'entiers de taille "taillextaille"
// et NULL si l allocation echoue
int **newTab(int taille){

	int ** table2D = (int**)malloc(taille*sizeof(int*));
	if (table2D == NULL) {
		return NULL;	// si l allocation en memoire echoue on renvoie NULL
	}	
	for(int i=0; i<taille ; i++){
		table2D[i]=(int *)malloc(taille*sizeof(int));
		
		if (table2D[i]==NULL){ // si l allocation en memoire echoue libere tout ce qui a deja ete alloue
			
			for(int j=i ; j>-1;j--){
				free(table2D[j]);
			}
			free(table2D);
			return NULL; // si l allocation en memoire echoue on renvoie NULL
		}
	
	}
	
	return table2D;

}
//parametre : n : la taille de la grille de taquin voulue
//alloue une grille de taquin de taille "nxn" de type Tid comme defini dans le sujet
// rappel : Tid est la grille resolue
//renvoi : new_grille un pointeur sur la grille de taquin de taille "nxn" de type Tid
// et NULL si on donne une taille n inferieure a 2
grille *newGrilleID(int n){
	if (n<2){
		printf("Une grille de taille inferieure a 2 n'a pas de sens\n");
		return NULL;
	}	
	grille *new_grille = (grille *)malloc(sizeof(grille));
	new_grille->taille=n;
	new_grille->i_libre=n-1;
	new_grille->j_libre=n-1;
	new_grille->tab=newTab(n);
	
	int init = 0;
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			new_grille->tab[i][j] = init+1; // on initialise les valeurs de tab
			init++;	
			
		}
	}
	new_grille->tab[new_grille->i_libre][new_grille->j_libre]=0; // on initialiste la case vide
	
	
	return new_grille;

}


//parametre : g : un pointeur sur la grille de taquin dont on veut connaitre les mouvements possibles
// La fonction donne les mouvements legaux sur la grille passee en parametre
// le tableau d entier moves est defini tel que les cases 0, 1, 2, 3 correspondent respectivement aux mouvements h,b,g,d. Si la case corespondante est a 0 le mouvement est illegal si la case est a 1 le mouvemente est possible 
//renvoi : tableau d entier moves modelisant les mouvements legaux pour l etat de la grille
 
int * possible_moves(grille *g){
	int * moves=malloc(4*sizeof(int));
	moves[0]=0;
	moves[1]=0;
	moves[2]=0;
	moves[3]=0;
	if(g->i_libre!=0){ // si la case libre n est pas tout en haut on peut monter
		moves[0]=1;
	}
	if(g->i_libre!=g->taille-1){ // si la case libre n est pas tout en bas on peut descendre
		moves[1]=1;
	}
	if(g->j_libre!=0){ // si la case libre n est pas tout a gauche on peut aller a gauche
		moves[2]=1;
	}
	if(g->j_libre!=g->taille-1){ // si la case libre n est pas tout a droite on peut aller a droite
		moves[3]=1;
	}

	return moves;
}

//parametres : -g : le pointeur sur la grille dans laquelle le mouvement doit etre effectue
//-direction : le caractere represantant la direction du mouvement a effectue
// rappel : les mouvements possibles sont listes dans la string directions declaree au debut
// la grille passee en parametre est modifie pour modeliser le mouvement effectue
// dans cette fonction on echange la case vide avec la case adjacente voulue sans verifier la legalite du mouvement. Cela sera fait en amont de l appel de la fonction
//renvoi : rien
void moving(grille *g, char direction){
	int nbr_switched = 0;
	
	switch(direction){
		case 'h':
			
			nbr_switched = g->tab[g->i_libre-1][g->j_libre];
			g->tab[g->i_libre-1][g->j_libre]=g->tab[g->i_libre][g->j_libre];
			g->tab[g->i_libre][g->j_libre] = nbr_switched;
			g->i_libre=g->i_libre-1;
			
			break;
		case 'b':
			nbr_switched = g->tab[g->i_libre+1][g->j_libre];
			g->tab[g->i_libre+1][g->j_libre]=g->tab[g->i_libre][g->j_libre];
			g->tab[g->i_libre][g->j_libre] = nbr_switched;
			g->i_libre=g->i_libre+1;
			break;
		
		case 'g':
			nbr_switched = g->tab[g->i_libre][g->j_libre-1];
			g->tab[g->i_libre][g->j_libre-1]=g->tab[g->i_libre][g->j_libre];
			g->tab[g->i_libre][g->j_libre] = nbr_switched;
			g->j_libre=g->j_libre-1;
			break;

		case 'd':
			nbr_switched = g->tab[g->i_libre][g->j_libre+1];
			g->tab[g->i_libre][g->j_libre+1]=g->tab[g->i_libre][g->j_libre];
			g->tab[g->i_libre][g->j_libre] = nbr_switched;
			g->j_libre=g->j_libre+1;
			break;

	}
	
	
}
//parametre : g : un pointeur sur la grille de taquin qu'on veut melanger
// la grille passee en parametre est melangee par un nombre aléatoire de mouvements legaux
//renvoi : rien
void melange(grille *g){
	
	
	int min_moves = (g->taille)*5 + rand()%100; // on definit le nombre de mouvements
	
	int * legal=possible_moves(g); // on initialise les mouvements legaux
	
	while( min_moves >= 0 ){
		int direct = rand() % 4; // on choisit un mouvement parmi les 4 au hasard
		
		
		while(!legal[direct]){ // tant que le mouvement n est pas legal on en prend un autre toujours au hasard
			direct = rand() % 4;
		}
		
		moving(g,directions[direct]); // on effectue le mouvement legal	
		min_moves=min_moves-1;
		free(legal); 
		legal=possible_moves(g); // on met a jour les mouvements legaux apres le mouvement

		
	}
	
	free(legal); 

	
}

//parametre : user_G : un pointeur sur la grille de taquin dont on veut connaitre l etat de resolution
// on compare la grille passee en parametre avec une grille ID (resolue)
//renvoi : si on trouve une difference on retourne 0 sinon la grille est resolue et on retourne 1
int verif(grille* user_G){
	grille *ID_G=newGrilleID(user_G->taille);
	
	//Si le joueur a trouver la matrice ID, verif = 1, 0 sinon;
	for(int i=0; i<user_G->taille; i++){
		for(int j=0; j<user_G->taille; j++){
			if(ID_G->tab[i][j]!=user_G->tab[i][j]){ // on compare les deux grilles
				libereGrille(ID_G); // on libere la grille ID une fois la comparaison finie
				return 0;
			}
		}
	}
	// on libere la grille ID une fois la comparaison finie
	libereGrille(ID_G);

	return 1;
}
//parametre : nom_fichier : chaine de caracteres contenant le nom du fichier .txt formate qui contient les grilles a importer
//FORMATAGE DU FICHIER
// 1er caractère = nbr de grilles à charger puis un '.'
// ensuite pour chaque grille
// taille de la  grille puis ";" puis ',' apres chaque element de la grille. y compris le dernier
// par exemple le fichier "1.2;1,2,3,0" contient la grille ID de taille 2  
//la fonction parcours le fichier et initialise les grilles comme definie par le formatage dans le tableau de grilles
//renvoi : grilles :le tableau de grilles de taille "nbr_grille" qui contient les "nbr_grille" grilles du fichier
grille **import_grille(char* nom_fichier){
	FILE* read = fopen(nom_fichier,"r"); // on ouvre le flux en lecture seulement
	if (read ==NULL){
		perror("Erreur lors de l'ouverture du fichier");
		return NULL;
	}
	char parcours=fgetc(read); // caractere utilise pour parcourir le fichier
	int compteur_nbr_grille =0;
	while((parcours !=EOF) && (parcours !='.')){	//on regarde combien de chiffre fait le nombre de grille
		compteur_nbr_grille = compteur_nbr_grille+1;
		parcours=fgetc(read);
	
	}
	
	if(parcours == EOF){
		fclose(read);
		return NULL;
	}
	fseek(read,0,SEEK_SET); // on a maintenant la taille du nombre, on revient au debut pour pouvoir le lire ET le stocker
	char * temp=(char*)malloc(compteur_nbr_grille*sizeof(char));
	for(int i=0; i<compteur_nbr_grille;i++){		// on recupere le nombre de grilles
		temp[i]=fgetc(read);

	}
	nbr_grille=atoi(temp); // conversion du nombre en entier
	free(temp);
	
	grille ** grilles = (grille**)malloc(nbr_grille*sizeof(grille *)); // on alloue la liste de grilles dont on connait maintenant la taille
	if (grilles==NULL){
		fclose(read);
		return grilles;
	}

	for (int i=0;i<nbr_grille;i++){	// on alloue les grilles dans la liste de grilles
		grilles[i]=(grille*)malloc(sizeof(grille));
		
		if (grilles[i]==NULL){ // si une allocation echoue on libere tout et on retourne NULL
			for (int j=0;j<i;j++){
				free(grilles[j]);
			}
			free(grilles);
			fclose(read);
			return NULL;
		}
	}
	
	
	fseek(read,1,SEEK_CUR); // on replace le curseur pour continuer le parcours
	
	for (int i=0;i<nbr_grille;i++){ //on traite le fichier en remplissant les "nbr_grille" grilles
		int compteur_taille_grille =0 ;
		while((parcours !=EOF) && (parcours !=';')){	
			compteur_taille_grille = compteur_taille_grille+1; // on recupere le nombre de chiffres de la taille de la grille
			parcours=fgetc(read);
		
			
		}
	
		if(parcours == EOF){
			printf("erreur dans la lecture du fichier\n");
			libereTabGrille(grilles,nbr_grille) ;
			fclose(read);
			return grilles ;
		}
		fseek(read,0-compteur_taille_grille,SEEK_CUR);// on revient au debut du nombre pour pouvoir le stocker maintenant que l'on a sa taille
		
		
		char * temp=(char*)malloc((compteur_taille_grille-1)*sizeof(char));
		for(int i=0; i<(compteur_taille_grille-1);i++){		// on recupere la taille de la grille
			temp[i]=fgetc(read);
			
		}
		int taille_grille=atoi(temp);
		
		free(temp);
		grilles[i]->tab=newTab(taille_grille); // on initialise le tab et la taille de la grille
		grilles[i]->taille=taille_grille;
		int ligne=0;
		int colonne=0;
		int compteur=0;
		
		parcours=fgetc(read); // Pour passer le point-virgule
		int valeur;
		while (ligne <taille_grille){		//une grille est remplie une fois qu'on a rempli les "nbr_grille" lignes de tab
				
				
				
				while((parcours !=EOF) &&(parcours !=',')){ //on recupere le nombre de chiffre de l element 
					compteur = compteur+1;
					parcours=fgetc(read);
					
					
				}
				
				
				if((parcours == EOF)&&(ligne!=taille_grille-1)&&(colonne!=taille_grille-1) ){
					printf("erreur dans la lecture du fichier\n");
					libereTabGrille(grilles,nbr_grille);
					fclose(read);
					return grilles;
				}
				fseek(read,-compteur,SEEK_CUR); // on retourne au debut du nombre pour le stocker
				
				char * temp=(char*)malloc((compteur-1)*sizeof(char));
				
				for(int k=0; k<(compteur-1);k++){		// on recupere la valeur
					temp[k]=fgetc(read);
				
				}
				parcours=fgetc(read);//on replace la tete pour lui faire passer la virgule
				
				valeur=atoi(temp);
			
				free(temp);
				if (valeur==0){ 			//Quand on trouve 0 on initialise les champs relatifs à la case vide
					grilles[i]->i_libre=ligne;
					grilles[i]->j_libre=colonne;
				}
				grilles[i]->tab[ligne][colonne]=valeur; // on affecte la valeur a sa place dans tab
				

				colonne=colonne+1;
				if (colonne==taille_grille){	//on verifie si on a fini la ligne
					colonne=0;
					ligne=ligne+1;
				}
				 
				parcours=fgetc(read); // on fait en sorte que parcours ne contient plus le separateur
				fseek(read,-1,SEEK_CUR); //on replace le curseur en consequent
				compteur=0;
		}
		
	
		
		
	}
	fclose(read);
	return grilles;
}

//parametres : - tab : un tableau 2D d entiers de taille "taillextaille"
// - val : la valeur entiere dont on veut la position dans le tableau
//- taille :la taille entiere du tableau 2D tab
// on parcours tab pour trouver val et renvoyer sa position dans tab
//renvoi : pos : pointeur NULL si on a pas trouve val dans tab et un tableau de 2 entier contenant respectivement la ligne et la colonne sinon
int* getPos(int  ** tab, int val, int taille){
	int * pos = (int*)malloc(2*sizeof(int));
	
	for (int i =0;i<taille ; i++){
		for (int j=0;j<taille;j++){ // on parcourt tab
			if (val==tab[i][j]){	// on verifie si on a trouve val
				pos[0]=i;
				pos[1]=j;
				return pos;
			}
		}
	
	}
	free(pos);
	return NULL;
}

//parametre : g : un pointeur sur une grille
// permet de savoir si on peut resoudre la grille en utilisant le principe de parite des transmutations 
//renvoi : 0 si on ne peut pas resoudre la grille 1 sinon
int resolvable (grille * g){
	int possible = 0;
	
	int transpos=0;
	int transpos_vide =(g->taille -1)*2 -(g->i_libre + g->j_libre); // formule donnant le nombre de mouvements a effectuer pour mettre la case vide en bas a droite
	int valeur=1;
	int swap =0;
	int ** copie = newTab(g->taille);	// on cree une copie de tab sur laquelle effectuer les operations
	
	for (int i=0;i<g->taille;i++){
		
		for(int j=0;j<g->taille;j++){
			copie[i][j]=g->tab[i][j];
			
		}
	}
	
	for (int i=0;i<g->taille;i++){	//on calcule le nombre de permutations pour mettre les elements non vide a leur place (on ne bouge pas les elements avec des mouvements legaux)
		for (int j=0;j<g->taille;j++){
			if (j==g->taille-1 && i==j){
			//on ne repositionne pas la case vide
			}else{
			if (copie[i][j]!=valeur){
				int * pos =getPos(copie,valeur,g->taille);
				swap=copie[i][j];
				
				copie[i][j]=copie[pos[0]][pos[1]];
				copie[pos[0]][pos[1]]=swap;
				
				free(pos);
				transpos=transpos +1;
				
			}}
		valeur=valeur+1;
		}
	
	}

	//printf("transpos %d\n",transpos);
	//printf("transpos vide %d\n",transpos_vide);
	if (transpos %2 == transpos_vide %2 ){ // si la parite est la meme on peut resoudre ce taquin
		possible=1;
	}
	
	for (int i=0; i<g->taille ;i++){
		free(copie[i]);	// on libere tab
	}
	free(copie);
	return possible;
}

//parametre : new_G : un pointeur sur la grille de taquin qui va servir a jouer
// on joue au jeu de taquin jusqu a resolution ou volonte de l utilisateur de quitter
//renvoi : rien
void play(grille *new_G){
	
		
	char direct;

	while(direct!='q' && verif(new_G)!=1){	// tant que l utilisateur ne veut pas quitter et que la grille n est pas resolue on continue
		
		system("clear");	
		printf("\n");		
		displayGrille(new_G);
		printf("\n'h'(haut), 'b'(bas), 'g'(gauche), 'd'(droite), 'r'(resolvable?) ,'q'(quitter): ");	// on demande une commande a l utilisateur
		scanf(" %c",&direct);	
		if(direct=='r'){
			if(resolvable (new_G)){
				
				printf("Ce taquin est resolvable !");
				sleep(1);
			}else{
				printf("Ce taquin n'est pas resolvable !");
				sleep(1);
			}
		}else {
			int *legal=possible_moves(new_G);
			
			int user_move=1;
			for(int i=0;i<4;i++){ 				// on verifier que le mouvement est legal
				if(directions[i]==direct){
					user_move = legal[i]; 
				}	
			}	
			if(user_move==1){				// si le mouvement est legal on l effectue
				moving(new_G, direct);
				
			}
			
			
			free(legal);	
		}
	}
	printf("\n");
	displayGrille(new_G);


}
//parametre : aucun
// on joue au jeu de taquin avec un grille de taille demandee a l utilisateur generee aleatoire (melangee)
//renvoi : rien
void play_user_melange(){	
		
	system("clear");	
	
	int N;	
	grille *new_G=NULL;
	while (new_G==NULL){ // on demande une taille de grille
		printf("\nLa dimension NxN du jeu de taquin : ");
		scanf("%d",&N);
		new_G=newGrilleID(N);	
	}
	
	melange(new_G); // on melange la grille
	

	play(new_G);	// on effectue une partie
	

	printf("\nGAME OVER\n");
	sleep(1);
	char ask_replay;
	while(ask_replay!='o' && ask_replay!='n'){ // on demande si l utilisateur veut rejouer
		printf("\n	Rejouer ? -> 'o' (OUI), 'n' (NON) : ");
		scanf(" %c",&ask_replay);
	}
	if(ask_replay=='o'){
		play_user_melange();
	}



}



//parametre : rien
// menu qui gere l affichage et l utilisation des fonctions implementees ci dessus pour le jeu de taquin
//renvoi 
int menu(){
	int tag_import=0;
	grille ** g_imp=NULL;
	int reponse=0;
	char nom_fichier[100] ;
	while(reponse!=3)  // tant que l utilisateur ne veut pas quitter
	{
		system("clear");
		printf("\nJEU DE TAQUIN\n\n	1 - Jeu de Taquin \n	2 - Importer une grille\n	3 - Quitter\n\nVotre Choix : ");		
		scanf("%d", &reponse);
	
	switch(reponse){
		case 1: 
			printf("\n	Jeu de Taquin \n");
			sleep(2);
			system("clear");
			if (tag_import){ // si on a importer une grille on propose de jouer avec 
				printf("%d grilles lues dans un fichier\n",nbr_grille);
				printf("	1 - Grille generee\n	2 - Grille importee\n");
				scanf("%d", &reponse);
				while(reponse!=1 && reponse!=2)
				{
					system("clear");
					printf("	1 - Grille generee\n	2 - Grille importee\n");		
					scanf("%d", &reponse);
				}
				if(reponse == 1){
					play_user_melange();
				}else{
					printf("Saisir le numero de la grille (commencant par 0) parmi les %d grilles lues dans le fichier\n",nbr_grille);
					scanf("%d", &reponse);
					while(reponse<0 && reponse>nbr_grille )
					{
						system("clear");
						printf("saisir le numero de la grille (commencant par 0) parmi les %d grilles lues dans le fichier\n",nbr_grille);
						scanf("%d", &reponse);
					}
					play(g_imp[reponse]);
					printf("\nGAME OVER\n");
					sleep(1);
				}
			}else{
			
				play_user_melange();
			}	
			break;
			
		case 2:
			printf("\n	Importer une grille\n");
			sleep(2);
			system("clear");
			printf("\nNom du fichier ?\n");
			scanf("%s",nom_fichier);	
			printf("NOM FICHIER %s\n",nom_fichier);
			if(tag_import){ // si on importe un autre fichier on libere le contenu du precedent, on ne peut pas importer plusieurs fichiers a la fois
			libereTabGrille(g_imp,nbr_grille);
			}	
			g_imp=import_grille(nom_fichier);
			tag_import=1;
			break;
		case 3:
			printf("\n	A BIENTOT ! \n\n");
			sleep(1);
			
			break;

		default:
			printf("\nSaisie invalide\n");
			reponse=3;
			
			break;
				

	}
	}
	if(tag_import){
	libereTabGrille(g_imp,nbr_grille);
	}
	return 0;
}

int main(){
	srand(time(NULL));


	menu();	

	return 0;

}
