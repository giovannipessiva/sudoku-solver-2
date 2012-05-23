#include <stdio.h>
#include <stdlib.h>
#include "Solver/header.h"

extern int cycleCounter;

extern int n;   //base of the sudoku
extern int n2;   //size of the sudoku (n2=n*n)
extern int mask;   //flag mask (mask=1<<n2+1)
extern int **m;   //matrix n^2 * n^2 for the sudoku
extern int chkEnabled[5];     //dont set numbers, just simplify int-of-flags



int checkSquare(int fin){
    int i,j,k,l,t,value,square;
    int I,J;
    int numbers;   //flag mask of completed numbers
    int unique;   //flag mask to find unique flags
    int unique_plus;   //flag mask to exclude not-unique flags
    int finished;   //counter of completed numbers in this square
    int *linCol=(int *) calloc(n,sizeof(int));   //to find allineated flags allineatios
    int *linRow=(int *) calloc(n,sizeof(int));   //to find allineated flags allineatios
    int *totCol=(int *) calloc(n,sizeof(int));   //to eliminate doubled flags
    int *totRow=(int *) calloc(n,sizeof(int));   //to eliminate doubled flags
    if(linCol==NULL || linRow==NULL || totCol==NULL || totRow==NULL)
    {
        fprintf(stderr,"Allocation error (3)\n");
        exit(ERR_ALLOC);
    }

    i=0;
    j=0;

    do{
        finished=0;
        square=(i/n)*n + (j/n);   //position of this square in array q and index in fin
        if((fin & (1<<square)) ==0)   //if this square is not completed
        {
            unique=0;
            unique_plus=0;
            //set the flag mask of completed numbers
            numbers=mask | (mask-1);   //numbers = 1....1 (n2+1 ones)

            I=i;   //coordinates of the 0 point of this square
            J=j;
            do{
                if((m[i][j] & mask) ==0)   //if it's a number
                    numbers=numbers & (numbers^(1<<m[i][j]));   //set to 0 its flag in numbers mask
                else{   //otherwise, if it's an int of flags
                    unique_plus=unique_plus | (unique & m[i][j]);  //detect duplicated flags
                    unique=unique^m[i][j];   //set to 0 flags that are not uniques
                }
            }while(getCoord(&i,&j)==0);

            unique=unique ^ (unique & unique_plus);
            if((unique & mask)==0)   //make sure it has flags's flag
                unique+=mask;

            i=I;   //restart from the beginning of this square
            j=J;
            do{
                if(m[i][j] & mask)   //if it's a flag
                {
                    m[i][j]= m[i][j] & numbers;   //use the mask

                    if((unique & m[i][j])>mask && chkEnabled[LV2])   //check if it has a unique flag
                    {
                        if(DEBUG1) printf("%d uniS: (%d,%d)=%d >>>>%d \n",cycleCounter,i,j,m[i][j],m[i][j] & unique);//////////////////
                        m[i][j]=unique & m[i][j];   //set the other flags to 0
                    }

                    value=checkOneValue(i,j);
                    if(value!=0)
                    {
                        if(value==-1)
                            return -1;  //error in the sudoku

                        if(DEBUG1) printf("NEW: (%d,%d)=%d >>>>%d \n",i,j,m[i][j],value);//////////////////
                        setNum(i,j,value);
                        finished++;
                        numbers=numbers | (1<<value);  //add new number to the mask
                    }
                }else{
                    finished++;
                }
            }while(getCoord(&i,&j)==0);


            if(cycleCounter>CHECK_START)   //Since it's an expensive check, it won't start at the beginning
            {
                if(chkEnabled[LV3])
                {
                    /* LV3: this part will check if there's a flags allineation
                        if a flags allineation if found for a number,
                        a function will set to 0 the number's flags in that line. */

                    //save the possible numbers of the rows and columns
                    for(k=0; k<n; k++)
                    {
                        linCol[k]=0;
                        linRow[k]=0;
                        for(l=0; l<n; l++)
                        {
                            if(m[I+l][J+k] & mask)
                                linCol[k]=linCol[k] | m[I+l][J+k];
                            if(m[I+k][J+l] & mask)
                                linRow[k]=linRow[k] | m[I+k][J+l];
                        }
                    }

                    //compare the flags to exclude common numbers
                    for(k=0; k<n; k++)
                    {
                        totCol[k]=0;
                        totRow[k]=0;
                        for(l=0; l<n; l++){
                            if(l!=k)
                            {
                                totCol[k]= totCol[k] | linCol[l];
                                totRow[k]= totRow[k] | linRow[l];
                            }
                        }
                    }


                    //check of allineations
                    for(k=0; k<n; k++)
                    {
                        linCol[k]=linCol[k] & (linCol[k] ^ totCol[k]);
                        linRow[k]=linRow[k] & (linRow[k] ^ totRow[k]);

                        if((linCol[k] & mask))   //make sure it has NOT flags's flag
                            linCol[k]-=mask;
                        if((linRow[k] & mask))   //make sure it has NOT flags's flag
                            linRow[k]-=mask;


                        if(linCol[k]>1)
                        {
                            if(DEBUG2) printf("column allineation: %d*,%d, =%d [cycler=%d]\n",I,J+k,linCol[k],cycleCounter);////////

                            warnCol(I,J,k,linCol[k]);
                            if(DEBUG2) printMatrix(stdout,0);
                        }
                        if(linRow[k]>1)
                        {
                            if(DEBUG2) printf("row allineation: %d,%d*, =%d [cycler=%d]\n",I+k,J,linRow[k],cycleCounter);////////

                            warnRow(I,J,k,linRow[k]);
                            if(DEBUG2) printMatrix(stdout,0);
                        }
                    }
                }


                 /*
                 Cicla sulle caselle di uno square
                 cerca un gruppo di celle con flag identici
                 se lo trovo ed il numero di caselle è == al numero di numeri possibili
                 semplifico quei numeri dal resto dello square
                 */
                //LV4
                if(chkEnabled[LV4])
                {
                    i=I;   //restart from the beginning of this square
                    j=J;
                    do{
                        if(m[i][j] & mask)   //if it's a flag
                        {

                            //find groups of cells with identical flags
                            k=0;
                            t=i;
                            l=j;
                            do{
                                if(m[t][l] & mask)   //if it's a flag
                                    if(areEquals(m[i][j],m[t][l]))
                                            k++;
                            }while(getCoord(&t,&l)==0);

                            if(k==countValues(i,j)){
                                //i found a group of identical flags,
                                //now i can semplify them from the cells that are not in the group

                                t=I;
                                l=J;
                                do{
                                    if(m[t][l] & mask)   //if it's a flag
                                        if(areEquals(m[i][j],m[t][l])==0){
                                            square=(i/n)*n + (l/n);   //position of this square in array q
                                            m[t][l]=( (m[t][l] ^ m[i][j]) & m[t][l] ) | mask;
                                        }
                                }while(getCoord(&t,&l)==0);
                            }
                        }
                    }while(getCoord(&i,&j)==0);  //*/
                }
            }

            if(finished==n2)
                fin=fin | (1<<square);   //set the flag for this square as completed
        }else{
            while(getCoord(&i,&j)==0);   //select next square
        }
    }while(i<n2 && j<n2);

	free(linCol);
        free(linRow);
        free(totCol);
        free(totRow);

	return fin;
}


int getCoord(int *i,int *j){
	*j=*j+1;
	if((*j)%n==0){   //end line
		(*i)=(*i)+1;
		*j=*j-n;
		if((*i)%n==0)   //end square
		{
			*i=*i-n;
			*j=*j+n;
			if(*j>=n2)   //end square line
			{
				*j=0;
				*i=*i+n;
				if(*i>=n2)   //end matrix
					return 2;
			}
			return 1;
		}
	}
	return 0;
}

void warnCol(int i,int j,int k,int val){
    int p,r;
    int square;
    int value;

    for(p=0; p<n; p++)
        if(p*n!=i)   //if it's not the allineation
        {
            square=p*n + j/n;   //position of this square in array q

            for(r=0; r<n; r++)
                if(m[p*n+r][j+k] & mask)
                {
                    if(DEBUG2) printf("subC: (%d,%d)=%d >>>>%d \n",p*n+r,j+k,m[p*n+r][j+k],m[p*n+r][j+k] & (m[p*n+r][j+k] ^ val));
                    m[p*n+r][j+k]=m[p*n+r][j+k] & (m[p*n+r][j+k] ^ val);

                    if(m[p*n+r][j+k]!=mask)
                    {
                        value=checkOneValue(p*n+r,j+k);
                        if(value>0)
                            setNum(p*n+r,j+k,value);
                    }else{
                        printInt("column error, value=",val);
                        //printf("column error: %d,%d+%d, =%d \n",i,j,k,val);
                    }
                }
        }
}

void warnRow(int i,int j,int k,int val){
    int p,r;
    int square;
    int value;

    for(p=0; p<n; p++)
        if(p*n!=j)   //if it's not the allineation
        {
            square=i + p;   //position of this square in array q

            for(r=0; r<n; r++)
                if(m[i+k][p*n+r] & mask)
                {
                    if(DEBUG2) printf("subR: (%d,%d)=%d >>>>%d \n",i+k,p*n+r,m[i+k][p*n+r],m[i+k][p*n+r] & (m[i+k][p*n+r] ^ val));
                    m[i+k][p*n+r]=m[i+k][p*n+r] & (m[i+k][p*n+r] ^ val);

                    if(m[i+k][p*n+r]!=mask)
                    {
                        value=checkOneValue(i+k,p*n+r);
                        if(value>0)
                            setNum(i+k,p*n+r,value);
                    }else{
                        printInt("row error, value=",val);
                        //printf("row error: %d+%d,%d, =%d \n",i,k,j,val);
                    }
                }
        }
}
