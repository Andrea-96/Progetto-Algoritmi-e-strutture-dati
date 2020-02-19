//ultimo main...

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#define HASH_SIZE 8//potenza di due
#define ENTITY_NAME_SIZE 34
#define RELATIONSHIP_TYPE_SIZE 34
#define DELIM " "

///////////////////////////////////////////////////
/////////////////dichiarazione di tipo

//nodo tabella di hash delle entità

typedef struct ENTITY_HASH_NODE{

    char entityName[ENTITY_NAME_SIZE];
    struct ENTITY_HASH_NODE *next;
    struct ENTITY_HASH_NODE *prev;
}entityHashNode;

//putatore a nodo tabella di hash delle entità

typedef entityHashNode * entityNodeRef;

///////////&parte nuova

//nodo lista dei mittenti costenuta nelle celle di hash delle relazioni; ogni ricevente di relazioni ha la propria lista di mittenti.
typedef struct senderEntity{

    char entitySenderName[ENTITY_NAME_SIZE];
    struct senderEntity *next;
    struct senderEntity *prev;
}senderNode;

typedef senderNode *senderNodeRef;

//nodo tabella di hash delle relazioni, ogni ricevente di quel tipo di relazione
//ha un nodo privato in cui cè una lista di tutti i mittenti.

typedef struct NODE_RELATIONSHIP_HASH{

    char entityReceiverName[ENTITY_NAME_SIZE];
    struct NODE_RELATIONSHIP_HASH *next;
    struct NODE_RELATIONSHIP_HASH *prev;
    int numberOfRelationship;
    senderNodeRef headOfSendersList;

}receiverNode;

typedef receiverNode * receivingNodeRef;

typedef receivingNodeRef receivingHashTable[HASH_SIZE];

////////////////////////fine parte nuova

//nodo per salvare un entità massima nella liste dei massimi

typedef struct ENTITY_FOR_MAX{

    char entityName[ENTITY_NAME_SIZE];
    struct ENTITY_FOR_MAX *next;
    struct ENTITY_FOR_MAX *prev;
}maxEntity;

//puntatore a nodo di entità per i massimi

typedef maxEntity * maxEntityRef;

//nodo relazione

typedef struct TYPE_RELATIONSHIP_LIST_NODE{

    char relationshipName[RELATIONSHIP_TYPE_SIZE];
    struct TYPE_RELATIONSHIP_LIST_NODE *next;
    struct TYPE_RELATIONSHIP_LIST_NODE *prev;
    int maxRelationshipOfThisType;
    int numberOfMax;
    maxEntityRef headOfMaxEntityList;
    receivingHashTable hashTableOfRelationship;


}relationshipListNode;

//puntatore a nodo relazione

typedef relationshipListNode * relationshipNodeRef;

/////////////////////////////////////////////
//////////////////////variabili globali

char commandLine[100];  //tutta la riga di comando di input
char *commandType; //tipo di comando
char *entity1; //entità 1
char *entity2; //entità 2
char *nameOfRelationship; //nome della relazione
entityNodeRef hashTableEntity[HASH_SIZE]; //tabella di hash delle entità(puntatore alla prima cella)
relationshipNodeRef headOfRelationshipTypeList=NULL; //testa della lista per il salvataggio delle relazioni.

//////////////////////////////////////////////
/////////////////////////prototipi funzioni


int hashFunctionForEntity(char[]);
entityNodeRef searchInHashTableEntity(char []);
void insertInHashTableEntity(char []);
void deleteInHashTableEntity(char []);
void addent(char []);
relationshipNodeRef searchRelationship(char []);
void insertRelationship(char []);
void addrel(char [],char[],char[]);
receivingNodeRef * searchHashTableOfRelationship(char *nameRelationship);
void report();
void delrel(char [],char[],char[]);
void delent(char []);
//funzioni di stampa
void printEntityHash();
void printCollisionListEntityHash(int t);
void printRelationshipList();
void printHashTableOfRelationship(char []);


//////////////////////////////////////////////////
/////////////////////////////main

int main() {

    while (1) {
        gets(commandLine);
        commandType = strtok(commandLine, DELIM);
        if (strcmp(commandType, "end") == 0) {
            return 0;
        }
        entity1 = strtok(NULL, DELIM);
        entity2 = strtok(NULL, DELIM);
        nameOfRelationship = strtok(NULL, DELIM);
        if (strcmp(commandType, "addent") == 0) {

            addent(entity1);
            continue;
        }
        if (strcmp(commandType, "report") == 0) {

            report();
            continue;
        }
        if (strcmp(commandType, "addrel") == 0) {

            addrel(entity1, entity2, nameOfRelationship);
            continue;
        }
        if (strcmp(commandType, "delrel")==0){

            delrel(entity1,entity2,nameOfRelationship);
            continue;
        }
        if (strcmp(commandType,"delent")==0){

            delent(entity1);
            continue;
        }
    }


}



//////////////////////////////////////////////
////////////////////////definizione funzioni



//computes hash for the given string

int hashFunctionForEntity(char s[]){

    unsigned index;

    for (index = 0; *s != '\0'; s++)
        index = *s + 31*index;
    return index % HASH_SIZE;
}



//return the pointer to the node of entity
//null if there is not the element

entityNodeRef searchInHashTableEntity(char key[]){

    entityNodeRef hashNodeRef=hashTableEntity[hashFunctionForEntity(key)];
    while(hashNodeRef!=NULL&&(strcmp(hashNodeRef->entityName,key)!=0)){

        hashNodeRef=hashNodeRef->next;
    }

    return hashNodeRef;
}

//insert a new node in hash table of entity
//if there is a collision insert the node in the list in order
//it the element is already present do nothing.

void insertInHashTableEntity(char key[]){

    int index=hashFunctionForEntity(key);
    //lista collisioni vuota
    if(hashTableEntity[index]==NULL) {

        hashTableEntity[index]=malloc(sizeof(entityHashNode));
        strcpy(hashTableEntity[index]->entityName,key);
        hashTableEntity[index]->prev=NULL;
        hashTableEntity[index]->next=NULL;
        return;
    }else {

        entityNodeRef newNode=NULL;
        entityNodeRef nodeRef=hashTableEntity[index];
        while(nodeRef!=NULL&&strcmp(nodeRef->entityName,key)<0){

            newNode=nodeRef;
            nodeRef=nodeRef->next;
        }
        //elemento già presente
        if(nodeRef!=NULL&&strcmp(nodeRef->entityName,key)==0){

            return;
        }
        //elemento non già presente ed inserimento in ultima posizione
        if(nodeRef==NULL){

            nodeRef=malloc(sizeof(entityHashNode));
            strcpy(nodeRef->entityName,key);
            newNode->next=nodeRef;
            nodeRef->prev=newNode;
            nodeRef->next=NULL;
            return;
        }else {
            newNode=malloc(sizeof(entityHashNode));
            strcpy(newNode->entityName,key);
            //elemento non già presente inserimento in testa
            if(nodeRef->prev==NULL){

                hashTableEntity[index]=newNode;
            }else { //inserimento centrale

                nodeRef->prev->next=newNode;
            }
            newNode->prev=nodeRef->prev;
            newNode->next=nodeRef;
            nodeRef->prev = newNode;
            return;
        }
    }
}

//delete element in hash table entity

void deleteInHashTableEntity(char key[]){

    entityNodeRef nodeToDeleteRef=searchInHashTableEntity(key);
    //se elemento è presente eliminalo altrimenti non fai nulla
    if(nodeToDeleteRef!=NULL){

        if(nodeToDeleteRef->prev!=NULL){

            nodeToDeleteRef->prev->next=nodeToDeleteRef->next;
        }
        else{

            hashTableEntity[hashFunctionForEntity(key)]=nodeToDeleteRef->next;
        }
        if(nodeToDeleteRef->next!=NULL){

            nodeToDeleteRef->next->prev=nodeToDeleteRef->prev;
        }
        free(nodeToDeleteRef);
    }
}

//add a new entity in the system.
void addent(char key[]){

    insertInHashTableEntity(key);
}

//return the pointer to the node of relationship
//null if is not present

relationshipNodeRef searchRelationship(char relationshipName[]){

    relationshipNodeRef nodeRef=headOfRelationshipTypeList;
    while(nodeRef!=NULL&&(strcmp(nodeRef->relationshipName,relationshipName)!=0)){

        nodeRef=nodeRef->next;
    }

    return nodeRef;
}

//insert a new relationship type in the system
//if is already present do nothing

void insertRelationship(char relationshipName []){

    //inserimento in testa(la lista prima dell'inserimento è vuota)
    if(headOfRelationshipTypeList==NULL) {

        headOfRelationshipTypeList = calloc(1, sizeof(relationshipListNode));
        strcpy(headOfRelationshipTypeList->relationshipName, relationshipName);
        headOfRelationshipTypeList->next = NULL;
        headOfRelationshipTypeList->prev = NULL;
        headOfRelationshipTypeList->maxRelationshipOfThisType = 0;
        return;

    }else{

        //lista non vuota
        relationshipNodeRef newNode = NULL;
        relationshipNodeRef nodeRef;
        nodeRef=headOfRelationshipTypeList;
        //cerco la posizione in cui voglio inserire
        while(nodeRef!=NULL&&(strcmp(nodeRef->relationshipName,relationshipName)<0)){

            newNode=nodeRef;
            nodeRef=nodeRef->next;
        }
        //elemento già presente non faccio nulla
        if(nodeRef!=NULL&&strcmp(nodeRef->relationshipName,relationshipName)==0){

            return;
        }
        if(nodeRef==NULL) {
            //elemento non già presente ed inserimento in ultima posizione.
            nodeRef = calloc(1, sizeof(relationshipListNode));
            strcpy(nodeRef->relationshipName, relationshipName);
            nodeRef->maxRelationshipOfThisType = 0;
            newNode->next = nodeRef;
            nodeRef->prev = newNode;
            nodeRef->next = NULL;
            return;

        }else {

            //elemento non già presente, inserimento in testa o in mezzo alla lista
            newNode = calloc(1, sizeof(relationshipListNode));
            strcpy(newNode->relationshipName, relationshipName);
            newNode->maxRelationshipOfThisType = 0;
            if (nodeRef->prev == NULL) {

                headOfRelationshipTypeList = newNode;
            } else {

                nodeRef->prev->next = newNode;
            }
            newNode->prev = nodeRef->prev;
            newNode->next = nodeRef;
            nodeRef->prev = newNode;
            return;

        }
    }
}

void addrel(char keyOrig[],char keyDest[],char nameRelationship[]) {

    if(searchInHashTableEntity(keyDest)!=NULL&&searchInHashTableEntity(keyOrig)!=NULL){

        int index=hashFunctionForEntity(keyDest);
        //inserimento relazione nella lista delle relazioni se non è gia presente
        insertRelationship(nameRelationship);
        relationshipNodeRef relationshipNode=searchRelationship(nameRelationship);
        receivingNodeRef * hashTableOfRelationship=relationshipNode->hashTableOfRelationship;
        //i-esima cella tabella di hash vuota, non esiste nessun ricevente che ha nodo in questa cella.
        if(hashTableOfRelationship[index]==NULL){

            hashTableOfRelationship[index] = malloc(sizeof(receiverNode));
            strcpy(hashTableOfRelationship[index]->entityReceiverName,keyDest);
            hashTableOfRelationship[index]->next=NULL;
            hashTableOfRelationship[index]->prev=NULL;
            hashTableOfRelationship[index]->headOfSendersList=NULL;
            hashTableOfRelationship[index]->numberOfRelationship=0;
            //per adesso inseirmento in testa per la lista dei mittenti di un certo ricevente, potrebbe essere comodo metterlo ordinato.
            //inserimento in testa nella lista dei mittenti all interno del nodo del ricevente.
            if(hashTableOfRelationship[index]->headOfSendersList==NULL){

                hashTableOfRelationship[index]->headOfSendersList=malloc(sizeof(senderNode));
                strcpy(hashTableOfRelationship[index]->headOfSendersList->entitySenderName,keyOrig);
                hashTableOfRelationship[index]->headOfSendersList->prev=NULL;
                hashTableOfRelationship[index]->headOfSendersList->next=NULL;
                hashTableOfRelationship[index]->numberOfRelationship++;
            }else{ //non dovrebbe servire questo ramo
                /*
               senderNodeRef senderNode1=NULL;
               senderNode1=malloc(sizeof(senderNode));
               strcpy(senderNode1->entitySenderName,keyOrig);
               senderNode1->next=hashTableOfRelationship[index]->headOfSendersList;
               if(hashTableOfRelationship[index]->headOfSendersList!=NULL){

                   hashTableOfRelationship[index]->headOfSendersList->prev=senderNode1;

               }
               hashTableOfRelationship[index]->headOfSendersList=senderNode1;
               senderNode1->prev=NULL;
               hashTableOfRelationship[index]->numberOfRelationship++;
                 */
            }
            //gestione dei massimi in caso i-esima cella tabella di hash vuota, non esiste nessun ricevente che ha nodo in questa cella.
            if(relationshipNode->maxRelationshipOfThisType==hashTableOfRelationship[index]->numberOfRelationship){

                if(relationshipNode->headOfMaxEntityList==NULL){

                    relationshipNode->headOfMaxEntityList = malloc(sizeof(maxEntity));
                    strcpy(relationshipNode->headOfMaxEntityList->entityName, keyDest);
                    relationshipNode->headOfMaxEntityList->prev = NULL;
                    relationshipNode->headOfMaxEntityList->next = NULL;
                    relationshipNode->numberOfMax++;
                }else{

                    maxEntityRef newMaxNode = NULL;
                    maxEntityRef nodeRef;
                    nodeRef = relationshipNode->headOfMaxEntityList;
                    while (nodeRef != NULL && (strcmp(nodeRef->entityName, keyDest) < 0)) {

                        newMaxNode = nodeRef;
                        nodeRef = nodeRef->next;
                    }
                    if (nodeRef == NULL) {

                        nodeRef = malloc(sizeof(maxEntity));
                        strcpy(nodeRef->entityName, keyDest);
                        newMaxNode->next=nodeRef;
                        nodeRef->prev=newMaxNode;
                        nodeRef->next=NULL;
                        relationshipNode->numberOfMax++;
                    }else{

                        newMaxNode=malloc(sizeof(maxEntity));
                        strcpy(newMaxNode->entityName, keyDest);
                        if(nodeRef->prev==NULL) {

                            relationshipNode->headOfMaxEntityList=newMaxNode;
                        }else {

                            nodeRef->prev->next=newMaxNode;
                        }
                        newMaxNode->prev=nodeRef->prev;
                        newMaxNode->next=nodeRef;
                        nodeRef->prev=newMaxNode;
                        relationshipNode->numberOfMax++;
                    }
                }
            }
            if(relationshipNode->maxRelationshipOfThisType <hashTableOfRelationship[index]->numberOfRelationship){

                while(relationshipNode->headOfMaxEntityList!=NULL){

                    maxEntityRef nodeRefNext=relationshipNode->headOfMaxEntityList->next;
                    free(relationshipNode->headOfMaxEntityList);
                    relationshipNode->headOfMaxEntityList=nodeRefNext;
                }
                relationshipNode->maxRelationshipOfThisType = hashTableOfRelationship[index]->numberOfRelationship;
                relationshipNode->headOfMaxEntityList=malloc(sizeof(maxEntity));
                strcpy(relationshipNode->headOfMaxEntityList->entityName,keyDest);
                relationshipNode->headOfMaxEntityList->next=NULL;
                relationshipNode->headOfMaxEntityList->prev=NULL;
                relationshipNode->numberOfMax=1;
            }
        }else{

            //verifico che esista un nodo etichettato da un certo destinatario che ha nella lista dei mittenti un certo mittente
            receivingNodeRef curs=hashTableOfRelationship[index];
            //cerco il nodo del mittente
            while(curs!=NULL){

                if(strcmp(curs->entityReceiverName,keyDest)==0){

                    //se entro qui significa che esiste un nodo etichettato da questo ricevente

                    senderNodeRef curs1=curs->headOfSendersList;
                    while(curs1!=NULL){

                        //controlliamo se nella lista esiste gia un certo mittente, se si esco.

                        if(strcmp(curs1->entitySenderName,keyOrig)==0){

                            return;
                        }
                        else{

                            curs1=curs1->next;
                        }
                    }
                    //se arrivo qua signfica che esiste il nodo del ricevente ma nella lista dei mittenti non esiste un certo mittente
                    //lo aggiungiamo adesso in testa

                    if(curs->headOfSendersList==NULL){

                        curs->headOfSendersList=malloc(sizeof(senderNode));
                        strcpy(curs->headOfSendersList->entitySenderName,keyOrig);
                        curs->headOfSendersList->prev=NULL;
                        curs->headOfSendersList->next=NULL;
                        curs->numberOfRelationship++;
                        //gestione dei massimi(codice uguale a quello di sopra)
                        if(relationshipNode->maxRelationshipOfThisType==curs->numberOfRelationship){

                            if(relationshipNode->headOfMaxEntityList==NULL){

                                relationshipNode->headOfMaxEntityList = malloc(sizeof(maxEntity));
                                strcpy(relationshipNode->headOfMaxEntityList->entityName, keyDest);
                                relationshipNode->headOfMaxEntityList->prev = NULL;
                                relationshipNode->headOfMaxEntityList->next = NULL;
                                relationshipNode->numberOfMax++;
                            }else{

                                maxEntityRef newMaxNode = NULL;
                                maxEntityRef nodeRef;
                                nodeRef = relationshipNode->headOfMaxEntityList;
                                while (nodeRef != NULL && (strcmp(nodeRef->entityName, keyDest) < 0)) {

                                    newMaxNode = nodeRef;
                                    nodeRef = nodeRef->next;
                                }
                                if (nodeRef == NULL) {

                                    nodeRef = malloc(sizeof(maxEntity));
                                    strcpy(nodeRef->entityName, keyDest);
                                    newMaxNode->next=nodeRef;
                                    nodeRef->prev=newMaxNode;
                                    nodeRef->next=NULL;
                                    relationshipNode->numberOfMax++;
                                }else{

                                    newMaxNode=malloc(sizeof(maxEntity));
                                    strcpy(newMaxNode->entityName, keyDest);
                                    if(nodeRef->prev==NULL) {

                                        relationshipNode->headOfMaxEntityList=newMaxNode;
                                    }else {

                                        nodeRef->prev->next=newMaxNode;
                                    }
                                    newMaxNode->prev=nodeRef->prev;
                                    newMaxNode->next=nodeRef;
                                    nodeRef->prev=newMaxNode;
                                    relationshipNode->numberOfMax++;
                                }
                            }
                        }
                        if(relationshipNode->maxRelationshipOfThisType <curs->numberOfRelationship){

                            while(relationshipNode->headOfMaxEntityList!=NULL){

                                maxEntityRef nodeRefNext=relationshipNode->headOfMaxEntityList->next;
                                free(relationshipNode->headOfMaxEntityList);
                                relationshipNode->headOfMaxEntityList=nodeRefNext;
                            }
                            relationshipNode->maxRelationshipOfThisType = curs->numberOfRelationship;
                            relationshipNode->headOfMaxEntityList=malloc(sizeof(maxEntity));
                            strcpy(relationshipNode->headOfMaxEntityList->entityName,keyDest);
                            relationshipNode->headOfMaxEntityList->next=NULL;
                            relationshipNode->headOfMaxEntityList->prev=NULL;
                            relationshipNode->numberOfMax=1;
                        }
                        return;
                    }else{

                        senderNodeRef senderNode1=NULL;
                        senderNode1=malloc(sizeof(senderNode));
                        strcpy(senderNode1->entitySenderName,keyOrig);
                        senderNode1->next=curs->headOfSendersList;
                        curs->headOfSendersList->prev=senderNode1;
                        curs->headOfSendersList=senderNode1;
                        senderNode1->prev=NULL;
                        curs->numberOfRelationship++;
                        //gestione dei massimi
                        if(relationshipNode->maxRelationshipOfThisType==curs->numberOfRelationship){

                            if(relationshipNode->headOfMaxEntityList==NULL){

                                relationshipNode->headOfMaxEntityList = malloc(sizeof(maxEntity));
                                strcpy(relationshipNode->headOfMaxEntityList->entityName, keyDest);
                                relationshipNode->headOfMaxEntityList->prev = NULL;
                                relationshipNode->headOfMaxEntityList->next = NULL;
                                relationshipNode->numberOfMax++;
                            }else{

                                maxEntityRef newMaxNode = NULL;
                                maxEntityRef nodeRef;
                                nodeRef = relationshipNode->headOfMaxEntityList;
                                while (nodeRef != NULL && (strcmp(nodeRef->entityName, keyDest) < 0)) {

                                    newMaxNode = nodeRef;
                                    nodeRef = nodeRef->next;
                                }
                                if (nodeRef == NULL) {

                                    nodeRef = malloc(sizeof(maxEntity));
                                    strcpy(nodeRef->entityName, keyDest);
                                    newMaxNode->next=nodeRef;
                                    nodeRef->prev=newMaxNode;
                                    nodeRef->next=NULL;
                                    relationshipNode->numberOfMax++;
                                }else{

                                    newMaxNode=malloc(sizeof(maxEntity));
                                    strcpy(newMaxNode->entityName, keyDest);
                                    if(nodeRef->prev==NULL) {

                                        relationshipNode->headOfMaxEntityList=newMaxNode;
                                    }else {

                                        nodeRef->prev->next=newMaxNode;
                                    }
                                    newMaxNode->prev=nodeRef->prev;
                                    newMaxNode->next=nodeRef;
                                    nodeRef->prev=newMaxNode;
                                    relationshipNode->numberOfMax++;
                                }
                            }
                        }
                        if(relationshipNode->maxRelationshipOfThisType <curs->numberOfRelationship){

                            while(relationshipNode->headOfMaxEntityList!=NULL){

                                maxEntityRef nodeRefNext=relationshipNode->headOfMaxEntityList->next;
                                free(relationshipNode->headOfMaxEntityList);
                                relationshipNode->headOfMaxEntityList=nodeRefNext;
                            }
                            relationshipNode->maxRelationshipOfThisType = curs->numberOfRelationship;
                            relationshipNode->headOfMaxEntityList=malloc(sizeof(maxEntity));
                            strcpy(relationshipNode->headOfMaxEntityList->entityName,keyDest);
                            relationshipNode->headOfMaxEntityList->next=NULL;
                            relationshipNode->headOfMaxEntityList->prev=NULL;
                            relationshipNode->numberOfMax=1;
                        }
                        return;
                    }

                }else{
                    curs=curs->next;
                }
            }
            //se entriamo in questo ramo significa che non esiste nessuno nodo etichettato da un certo ricevente, dobbiamo creare il nodo ed inseirre
            //il mittente nella lista.
            receivingNodeRef receivingNode=malloc(sizeof(receiverNode));
            receivingNode->numberOfRelationship=0;
            receivingNode->headOfSendersList=NULL;
            strcpy(receivingNode->entityReceiverName,keyDest);
            receivingNode->next=hashTableOfRelationship[index];
            hashTableOfRelationship[index]->prev=receivingNode;
            hashTableOfRelationship[index]=receivingNode;
            receivingNode->prev=NULL;
            senderNodeRef curs1=receivingNode->headOfSendersList;
            if(curs1==NULL){

                hashTableOfRelationship[index]->headOfSendersList=malloc(sizeof(senderNode));
                strcpy(hashTableOfRelationship[index]->headOfSendersList->entitySenderName,keyOrig);
                hashTableOfRelationship[index]->headOfSendersList->prev=NULL;
                hashTableOfRelationship[index]->headOfSendersList->next=NULL;
                receivingNode->numberOfRelationship++;
            }else{

                senderNodeRef senderNode1=NULL;
                senderNode1=malloc(sizeof(senderNode));
                strcpy(senderNode1->entitySenderName,keyOrig);
                senderNode1->next=hashTableOfRelationship[index]->headOfSendersList;
                if(hashTableOfRelationship[index]->headOfSendersList!=NULL){

                    hashTableOfRelationship[index]->headOfSendersList->prev=senderNode1;

                }
                hashTableOfRelationship[index]->headOfSendersList=senderNode1;
                senderNode1->prev=NULL;
                receivingNode->numberOfRelationship++;
            }
            //gestione dei massimi.

            if(relationshipNode->maxRelationshipOfThisType==receivingNode->numberOfRelationship){

                if(relationshipNode->headOfMaxEntityList==NULL){

                    relationshipNode->headOfMaxEntityList = malloc(sizeof(maxEntity));
                    strcpy(relationshipNode->headOfMaxEntityList->entityName, keyDest);
                    relationshipNode->headOfMaxEntityList->prev = NULL;
                    relationshipNode->headOfMaxEntityList->next = NULL;
                    relationshipNode->numberOfMax++;
                }else{

                    maxEntityRef newMaxNode = NULL;
                    maxEntityRef nodeRef;
                    nodeRef = relationshipNode->headOfMaxEntityList;
                    while (nodeRef != NULL && (strcmp(nodeRef->entityName, keyDest) < 0)) {

                        newMaxNode = nodeRef;
                        nodeRef = nodeRef->next;
                    }
                    if (nodeRef == NULL) {

                        nodeRef = malloc(sizeof(maxEntity));
                        strcpy(nodeRef->entityName, keyDest);
                        newMaxNode->next=nodeRef;
                        nodeRef->prev=newMaxNode;
                        nodeRef->next=NULL;
                        relationshipNode->numberOfMax++;
                    }else{

                        newMaxNode=malloc(sizeof(maxEntity));
                        strcpy(newMaxNode->entityName, keyDest);
                        if(nodeRef->prev==NULL) {

                            relationshipNode->headOfMaxEntityList=newMaxNode;
                        }else {

                            nodeRef->prev->next=newMaxNode;
                        }
                        newMaxNode->prev=nodeRef->prev;
                        newMaxNode->next=nodeRef;
                        nodeRef->prev=newMaxNode;
                        relationshipNode->numberOfMax++;
                    }
                }
            }
            if(relationshipNode->maxRelationshipOfThisType <receivingNode->numberOfRelationship){

                while(relationshipNode->headOfMaxEntityList!=NULL){

                    maxEntityRef nodeRefNext=relationshipNode->headOfMaxEntityList->next;
                    free(relationshipNode->headOfMaxEntityList);
                    relationshipNode->headOfMaxEntityList=nodeRefNext;
                }
                relationshipNode->maxRelationshipOfThisType = receivingNode->numberOfRelationship;
                relationshipNode->headOfMaxEntityList=malloc(sizeof(maxEntity));
                strcpy(relationshipNode->headOfMaxEntityList->entityName,keyDest);
                relationshipNode->headOfMaxEntityList->next=NULL;
                relationshipNode->headOfMaxEntityList->prev=NULL;
                relationshipNode->numberOfMax=1;
            }
        }
    }
}


//return the pointer to the hash table of relationship
//null if the relationship doesn't exist

receivingNodeRef * searchHashTableOfRelationship(char nameRelationship[]){

    relationshipNodeRef nodeRef=searchRelationship(nameRelationship);
    if(nodeRef!=NULL){

        return nodeRef->hashTableOfRelationship;
    }else{

        return NULL;
    }
}

void report(){

    relationshipNodeRef nodeRef=headOfRelationshipTypeList;
    if(headOfRelationshipTypeList==NULL){

        fputs("none\n",stdout);
    }else{

        while(nodeRef!=NULL){

            if(nodeRef->maxRelationshipOfThisType!=0) {

                maxEntityRef entityRef = nodeRef->headOfMaxEntityList;
                fputs(nodeRef->relationshipName, stdout);
                fputs(" ", stdout);
                while (entityRef != NULL) {

                    fputs(entityRef->entityName, stdout);
                    fputs(" ", stdout);
                    entityRef = entityRef->next;
                }

                printf("%d; ", nodeRef->maxRelationshipOfThisType);
            }
            nodeRef=nodeRef->next;
        }
        fputs("\n",stdout);
    }
}

void printCollisionListEntityHash(int i){

    entityNodeRef hashNodeRef=hashTableEntity[i];
    if(hashNodeRef==NULL){

        printf("%d-esima cella: vuota\n",i);
    }else{

        printf("%d-esima cella: contiene ",i);
        while(hashNodeRef!=NULL){

            printf("%s-> ",hashNodeRef->entityName);
            hashNodeRef=hashNodeRef->next;
        }
        printf("\n");
    }

}

void printEntityHash(){

    printf("ENTITY HASH\n");
    for(int i=0;i<HASH_SIZE;i++){

        printCollisionListEntityHash(i);
    }
    printf("\n");
}

void printRelationshipList(){

    relationshipNodeRef nodeRef=headOfRelationshipTypeList;
    if(headOfRelationshipTypeList==NULL){

        printf("lista vuota\n");
    }else{

        while(nodeRef!=NULL){

            printf("%s (il massimo è %d ricevuto da %d persone) ->",nodeRef->relationshipName,nodeRef->maxRelationshipOfThisType,nodeRef->numberOfMax);
            nodeRef=nodeRef->next;
        }
        printf("\n\n");
    }
}
void printHashTableOfRelationship(char nameRelationship[]){

    printf("%s HASH\n",nameRelationship);
    receivingNodeRef * hashTableOfRelationship=searchHashTableOfRelationship(nameRelationship);
    if(hashTableOfRelationship!=NULL){

        for(int i=0;i<HASH_SIZE;i++){

            receivingNodeRef curs=hashTableOfRelationship[i];
            if(curs==NULL){

                printf("%d-esima cella: vuota",i);
            }else{

                printf("%d-esima cella: ",i);
                while(curs!=NULL){

                    printf("%s riceve %d da: ",curs->entityReceiverName,curs->numberOfRelationship);
                    senderNodeRef curs1=curs->headOfSendersList;
                    while(curs1!=NULL){

                        printf("%s ",curs1->entitySenderName);
                        curs1=curs1->next;
                    }
                    printf("||||||| ");

                    curs=curs->next;
                }
            }
            printf("\n");
        }
        printf("\n");
    }else{

        printf("relazione non monitorata\n");
    }
}


void delrel(char keyOrig[],char keyDest[],char nameRelationship[]){

    int index=hashFunctionForEntity(keyDest);
    relationshipNodeRef relationshipNode=searchRelationship(nameRelationship);
    if(relationshipNode==NULL){
        return;
    }
    receivingNodeRef * hashTableOfRelationship=relationshipNode->hashTableOfRelationship;
    receivingNodeRef curs=hashTableOfRelationship[index];
    senderNodeRef curs1;
    maxEntityRef nodeRef;
    int newMax=0;
    while(curs!=NULL) {

        if (strcmp(curs->entityReceiverName, keyDest) == 0) {

            //esiste un nodo di quel ricevente per la relazione richiesta
            curs1 = curs->headOfSendersList;
            while (curs1 != NULL) {

                if (strcmp(curs1->entitySenderName, keyOrig) == 0) {

                    //esiste nella lista dei mittenti il mittente cercato
                    //cancellazione elemento.
                    if (curs1->prev != NULL) {

                        curs1->prev->next = curs1->next;
                    } else {

                        curs->headOfSendersList = curs1->next;
                    }
                    if (curs1->next != NULL) {

                        curs1->next->prev = curs1->prev;
                    }
                    free(curs1);
                    curs->numberOfRelationship--;
                    if (curs->numberOfRelationship+1 == relationshipNode->maxRelationshipOfThisType &&
                        (relationshipNode->numberOfMax> 1)) {

                        //elemento è nella lista dei massimi ma il numero dei massimi è >1
                        //elimino l'elemento dalla lista dei massimi, decremento il numero di quanti elementi hanno il massimi ma non cambio il valore
                        //massimo delle relazioni( valore che serve per la report)
                        nodeRef = relationshipNode->headOfMaxEntityList;
                        while (nodeRef != NULL) {

                            if(strcmp(nodeRef->entityName, keyDest) == 0){
                                //trovo l'elemento nella lista dei massimi e lo elimino.
                                if(nodeRef->prev!=NULL) {

                                    nodeRef->prev->next = nodeRef->next;
                                }else{

                                    relationshipNode->headOfMaxEntityList=nodeRef->next;
                                }
                                if(nodeRef->next!=NULL){

                                    nodeRef->next->prev=nodeRef->prev;
                                }
                                free(nodeRef);
                                relationshipNode->numberOfMax--;
                                return;

                            } else {

                                nodeRef = nodeRef->next;
                            }
                        }
                        return;
                    }
                    if(relationshipNode->maxRelationshipOfThisType>curs->numberOfRelationship+1){

                        return;
                    }
                    if(curs->numberOfRelationship+1==relationshipNode->maxRelationshipOfThisType&&(relationshipNode->numberOfMax==1)){

                        //elemento è nella lista dei massimi ed è l'unico elemento.
                        //lo elimino dalla lista dei massimi, metto il numero dei massimi a zero e il massimo delle relazioni
                        // ricevute a zero e ricalco i massimi per quella relazione
                        // cercando gli elementi che hanno come valore massimo precedente -1 perchè sicuramente l'elemento
                        //a cui abbiamo cancellato la relazione ha valore massimo precedente -1 relazioni che riceve.
                        nodeRef=relationshipNode->headOfMaxEntityList;
                        //elimino elemento, se tutto funziona come deve deve essere l'unico elemento nella lista.
                        while (nodeRef != NULL) {

                            if(strcmp(nodeRef->entityName, keyDest) == 0){
                                //trovo l'elemento nella lista dei massimi e lo elimino.
                                if(nodeRef->prev!=NULL) {

                                    nodeRef->prev->next = nodeRef->next;
                                }else{

                                    relationshipNode->headOfMaxEntityList=nodeRef->next;
                                }
                                if(nodeRef->next!=NULL){

                                    nodeRef->next->prev=nodeRef->prev;
                                }
                                free(nodeRef);
                                newMax=relationshipNode->maxRelationshipOfThisType-1;
                                relationshipNode->numberOfMax=0;
                                relationshipNode->maxRelationshipOfThisType--;
                                //ricalcolo il massimo.
                                for(int i=0;i<HASH_SIZE;i++){

                                    curs=hashTableOfRelationship[i];
                                    while(curs!=NULL){

                                        if(curs->numberOfRelationship==newMax){

                                            //inserisco l'elemento nella nuova lista dei massimi
                                            if(relationshipNode->headOfMaxEntityList==NULL){

                                                relationshipNode->headOfMaxEntityList = malloc(sizeof(maxEntity));
                                                strcpy(relationshipNode->headOfMaxEntityList->entityName, curs->entityReceiverName);
                                                relationshipNode->headOfMaxEntityList->prev = NULL;
                                                relationshipNode->headOfMaxEntityList->next = NULL;
                                                relationshipNode->numberOfMax++;
                                            }else{

                                                maxEntityRef newMaxNode = NULL;
                                                maxEntityRef nodeRef1;
                                                nodeRef1 = relationshipNode->headOfMaxEntityList;
                                                while (nodeRef1 != NULL && (strcmp(nodeRef1->entityName, curs->entityReceiverName) < 0)) {

                                                    newMaxNode = nodeRef1;
                                                    nodeRef1 = nodeRef1->next;
                                                }
                                                if (nodeRef1 == NULL) {

                                                    nodeRef1 = malloc(sizeof(maxEntity));
                                                    strcpy(nodeRef1->entityName, curs->entityReceiverName);
                                                    newMaxNode->next=nodeRef1;
                                                    nodeRef1->prev=newMaxNode;
                                                    nodeRef1->next=NULL;
                                                    relationshipNode->numberOfMax++;
                                                }else{

                                                    newMaxNode=malloc(sizeof(maxEntity));
                                                    strcpy(newMaxNode->entityName,curs->entityReceiverName);
                                                    if(nodeRef1->prev==NULL) {

                                                        relationshipNode->headOfMaxEntityList=newMaxNode;
                                                    }else {

                                                        nodeRef1->prev->next=newMaxNode;
                                                    }
                                                    newMaxNode->prev=nodeRef1->prev;
                                                    newMaxNode->next=nodeRef1;
                                                    nodeRef1->prev=newMaxNode;
                                                    relationshipNode->numberOfMax++;
                                                }
                                            }
                                            curs=curs->next;
                                        }else{

                                            curs=curs->next;
                                        }
                                    }
                                }
                                return;
                            } else {

                                nodeRef = nodeRef->next;
                            }
                        }
                    }
                } else {

                    curs1 = curs1->next;
                }
            }
            return;
        } else {

            curs = curs->next;
        }
    }
}

void delent(char key[]){

    entityNodeRef nodeToDelete=searchInHashTableEntity(key);
    relationshipNodeRef cursForRelationShip;
    receivingNodeRef * hashTableOfRelationship;
    receivingNodeRef cursForReceiving;
    receivingNodeRef cursForDeleteKey;
    senderNodeRef cursForSender;
    senderNodeRef cursForDeleteSender;
    maxEntityRef cursToDeleteInMaxList;
    int index=hashFunctionForEntity(key);
    int relationshipOfReceiving;

    //elimazione nodo tabella di hash delle entità
    if(nodeToDelete!=NULL) {

        if (nodeToDelete->prev != NULL) {

            nodeToDelete->prev->next = nodeToDelete->next;
        } else {

            hashTableEntity[index] = nodeToDelete->next;
        }
        if (nodeToDelete->next != NULL) {

            nodeToDelete->next->prev = nodeToDelete->prev;
        }
        //per ogni relazione devo cancellare il nodo associato a questa entità e cancellare l'entità da tutte le relazioni.

        //riferimento al nodo della relazione.
        cursForRelationShip=headOfRelationshipTypeList;
        while(cursForRelationShip!=NULL){

            //riferimento alla tabella di hash della relazione.
            hashTableOfRelationship=cursForRelationShip->hashTableOfRelationship;
            for(int i=0;i<HASH_SIZE;i++){

                cursForReceiving=hashTableOfRelationship[i];
                while(cursForReceiving!=NULL){

                    if(strcmp(cursForReceiving->entityReceiverName,key)==0){

                        //sono nel nodo dell'entità da eliminare
                        cursForSender=cursForReceiving->headOfSendersList;
                        relationshipOfReceiving=cursForReceiving->numberOfRelationship;
                        while(cursForSender!=NULL){

                            cursForDeleteSender=cursForSender->next;
                            free(cursForSender);
                            cursForSender=cursForDeleteSender;
                            cursForReceiving->numberOfRelationship--;
                        }
                        cursForReceiving->headOfSendersList=NULL;
                        cursForReceiving->numberOfRelationship=0;
                        //elimino il nodo del ricevente.
                        if(cursForReceiving->prev!=NULL){

                            cursForReceiving->prev->next=cursForReceiving->next;
                        }else{

                            hashTableOfRelationship[index]=cursForReceiving->next;
                        }
                        if(cursForReceiving->next!=NULL){

                            cursForReceiving->next->prev=cursForReceiving->prev;
                        }
                        cursForDeleteKey=cursForReceiving->next;
                        free(cursForReceiving);
                        cursForReceiving=cursForDeleteKey;

                    }else{
                        //sono nel nodo non della chiave da eliminare
                        cursForSender=cursForReceiving->headOfSendersList;
                        while(cursForSender!=NULL) {

                            if (strcmp(cursForSender->entitySenderName, key) == 0){

                                cursForReceiving->numberOfRelationship--;
                                //elimino questo nodo.
                                if(cursForSender->prev!=NULL){

                                    cursForSender->prev->next=cursForSender->next;
                                }else{

                                    cursForReceiving->headOfSendersList=cursForSender->next;
                                }
                                if(cursForSender->next!=NULL){

                                    cursForSender->next->prev=cursForSender->prev;
                                }
                                free(cursForSender);
                                break;
                            } else {

                                cursForSender=cursForSender->next;
                            }
                        }
                        cursForReceiving=cursForReceiving->next;
                    }
                }
            }
            //ricalcolo massimo
            cursForRelationShip->numberOfMax=0;
            cursForRelationShip->maxRelationshipOfThisType=0;
            while(cursForRelationShip->headOfMaxEntityList!=NULL){

                cursToDeleteInMaxList=cursForRelationShip->headOfMaxEntityList->next;
                free(cursForRelationShip->headOfMaxEntityList);
                cursForRelationShip->headOfMaxEntityList=cursToDeleteInMaxList;
            }
            for(int i=0;i<HASH_SIZE;i++){

                cursForReceiving=hashTableOfRelationship[i];
                while(cursForReceiving!=NULL){

                    if(cursForReceiving->numberOfRelationship==cursForRelationShip->maxRelationshipOfThisType){

                        if(cursForRelationShip->headOfMaxEntityList==NULL){

                            cursForRelationShip->headOfMaxEntityList = malloc(sizeof(maxEntity));
                            strcpy(cursForRelationShip->headOfMaxEntityList->entityName,cursForReceiving->entityReceiverName);
                            cursForRelationShip->headOfMaxEntityList->prev = NULL;
                            cursForRelationShip->headOfMaxEntityList->next = NULL;
                            cursForRelationShip->numberOfMax++;
                        }else{

                            maxEntityRef newMaxNode = NULL;
                            maxEntityRef nodeRef;
                            nodeRef = cursForRelationShip->headOfMaxEntityList;
                            while (nodeRef != NULL && (strcmp(nodeRef->entityName,cursForReceiving->entityReceiverName) < 0)) {

                                newMaxNode = nodeRef;
                                nodeRef = nodeRef->next;
                            }
                            if (nodeRef == NULL) {

                                nodeRef = malloc(sizeof(maxEntity));
                                strcpy(nodeRef->entityName,cursForReceiving->entityReceiverName);
                                newMaxNode->next=nodeRef;
                                nodeRef->prev=newMaxNode;
                                nodeRef->next=NULL;
                                cursForRelationShip->numberOfMax++;
                            }else{

                                newMaxNode=malloc(sizeof(maxEntity));
                                strcpy(newMaxNode->entityName, cursForReceiving->entityReceiverName);
                                if(nodeRef->prev==NULL) {

                                    cursForRelationShip->headOfMaxEntityList=newMaxNode;
                                }else {

                                    nodeRef->prev->next=newMaxNode;
                                }
                                newMaxNode->prev=nodeRef->prev;
                                newMaxNode->next=nodeRef;
                                nodeRef->prev=newMaxNode;
                                cursForRelationShip->numberOfMax++;
                            }
                        }
                    }
                    if(cursForReceiving->numberOfRelationship>cursForRelationShip->maxRelationshipOfThisType){

                        while(cursForRelationShip->headOfMaxEntityList!=NULL){

                            maxEntityRef nodeRefNext=cursForRelationShip->headOfMaxEntityList->next;
                            free(cursForRelationShip->headOfMaxEntityList);
                            cursForRelationShip->headOfMaxEntityList=nodeRefNext;
                        }
                        cursForRelationShip->maxRelationshipOfThisType = cursForReceiving->numberOfRelationship;
                        cursForRelationShip->headOfMaxEntityList=malloc(sizeof(maxEntity));
                        strcpy( cursForRelationShip->headOfMaxEntityList->entityName,cursForReceiving->entityReceiverName);
                        cursForRelationShip->headOfMaxEntityList->next=NULL;
                        cursForRelationShip->headOfMaxEntityList->prev=NULL;
                        cursForRelationShip->numberOfMax=1;

                    }

                    cursForReceiving=cursForReceiving->next;
                }
            }

            cursForRelationShip=cursForRelationShip->next;
        }
    }else{

        return;
    }


}

