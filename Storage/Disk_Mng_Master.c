#include "Disk_Mng_Master.h"
#include "Disk_Mng_Master_API.h"

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

 Disk_Management_CB_t* disk_mng_CB;
 int length, startAdress, howManyToLoad;
void disk_mng_initialize_CB()
{
    disk_mng_CB = (Disk_Management_CB_t*)allocate_memory(sizeof(Disk_Management_CB_t), "Failed to allocate memory for disk control block", "disk_mng_initialize_CB");
}

void disk_mng_initialize()
{
    int startAdress = 0;
    int howManyToLoad = sizeof(int);
    int address=0;//����� ����� 
    disk_loadDataForInitializeDataStructers(&address, &startAdress,&howManyToLoad);
    if (address != 1)//������ �� ���� ���  ����  ������
    {
        ////restart the first address int disk with 1 to know if its first time or not
        disk_saveDataFromStructersToDisk(&address, &startAdress, &howManyToLoad);
        disk_mng_firstInitialize();
    }
    else
    {
        disk_mng_normalInitialize();
    }
    arrayInLoaded_initialize();
}




void disk_mng_saveData()
{
    stack_saveData();
    array_saveData();
    avlTree_saveData();

}

void disk_mng_firstInitialize()
{    
    disk_mng_initialize_CB();
    stack_firstInitialize();
    array_firstInitialize();
    avlTree_firstInitialize();
    disk_mng_CB->mapIdIndex = 0;

}

void disk_mng_normalInitialize()
{
    disk_mng_initialize_CB();
    stack_normalInitialize();
    array_normalInitialize();
    avlTree_normalInitialize();
}

void arrayInLoaded_initialize()
{
    disk_mng_CB->disk_MapsInLoadedToCache = (DiskMapsInLoadedToCache_t**)allocate_memory(CACHE_SIZE *sizeof(DiskMapsInLoadedToCache_t*), "Failed to allocate memory for array in loaded ", "arrayInLoaded_initialize");
    if (disk_mng_CB->disk_MapsInLoadedToCache != NULL) {
        for (int i = 0; i < CACHE_SIZE; i++) {
            disk_mng_CB->disk_MapsInLoadedToCache[i] = NULL;
        }
    }
}

DiskMapsInLoadedToCache_t* arrayInLoaded_create(int mapId, int index)
{
    DiskMapsInLoadedToCache_t* arrayInfo = (DiskMapsInLoadedToCache_t*)allocate_memory(sizeof(DiskMapsInLoadedToCache_t), "Failed to allocate memory for array in loaded info", "arrayInLoaded_create");
    arrayInfo->mapId = mapId;
    arrayInfo->index = index;
    return arrayInfo;
}

//initialize on the first time we turn o the computer the stack will initialize with all the indexes.
void stack_firstInitialize() {
    disk_mng_CB->diskFreeIndexesInArray=(DiskFreeIndexesInArray_t*)allocate_memory(sizeof(DiskFreeIndexesInArray_t), "Failed to allocate memory for stack ", "stack_firstInitialize");
    disk_mng_CB->diskFreeIndexesInArray->top = NULL;
    disk_mng_CB->diskFreeIndexesInArray->size = 0;
    for (size_t i = 0; i < DISK_SIZE; i++)
    {
        stack_push(i);
    }
}

void stack_normalInitialize() {
    disk_mng_CB->diskFreeIndexesInArray = (DiskFreeIndexesInArray_t*)allocate_memory(sizeof(DiskFreeIndexesInArray_t), "Failed to allocate memory for stack ", "stack_normalInitialize");
    int length = 0;
    int startAdress = 4;
    int howManyToLoad = sizeof(int);
    //load the second address in disk with stack size
    disk_loadDataForInitializeDataStructers(&length, &startAdress, &howManyToLoad);
    startAdress = 12;
    howManyToLoad = length * sizeof(int);
    //load all the data from stack
    disk_loadDataForInitializeDataStructers(&(disk_mng_CB->diskFreeIndexesInArray), &startAdress,&howManyToLoad) ;
    
}

void stack_saveData()
{
    int startAdress = 4;
    int howManyToLoad = 4+sizeof(int);
    ////save size of stack in the second address in the disk
    disk_saveDataFromStructersToDisk(&(disk_mng_CB->diskFreeIndexesInArray->size), &startAdress, &howManyToLoad);

    startAdress = 12;
    howManyToLoad = disk_mng_CB->diskFreeIndexesInArray->size * sizeof(int);
    //save all the data from stack
    disk_saveDataFromStructersToDisk(&(disk_mng_CB->diskFreeIndexesInArray), &startAdress, &howManyToLoad);
}

bool stack_is_empty() {
    return  disk_mng_CB->diskFreeIndexesInArray->top == NULL;
}

StackNode_t* stackNode_create(int index)
{   
    StackNode_t* new_node = (StackNode_t*)allocate_memory(sizeof(StackNode_t), "Failed to allocate memory for max heap", "stackNode_create");
    new_node->freeIndex = index;
    new_node->next = disk_mng_CB->diskFreeIndexesInArray->top;
    return new_node;
}


void stack_push(int index) 
{
    StackNode_t* new_node = stackNode_create(index);
    disk_mng_CB->diskFreeIndexesInArray->size++;
    disk_mng_CB->diskFreeIndexesInArray->top = new_node;
}

int stack_pop() {
    if (stack_is_empty()) {
        return -1;
    }
    StackNode_t* temp = disk_mng_CB->diskFreeIndexesInArray->top;
    int value = temp->freeIndex;
    disk_mng_CB->diskFreeIndexesInArray->top = disk_mng_CB->diskFreeIndexesInArray->top->next;
    disk_mng_CB->diskFreeIndexesInArray->size--;
    free(temp);
    return value;
}

int stack_top() {
    if (stack_is_empty()) {
        return -1;
    }
    return disk_mng_CB->diskFreeIndexesInArray->top->freeIndex;
}

//array fuctions

void array_firstInitialize() {
    disk_mng_CB->arrayForAllMApsInformation = (ArrayInfo_t**)allocate_memory(DISK_SIZE*sizeof(ArrayInfo_t*), "Failed to allocate memory for array ", "array_firstInitialize");
    if (disk_mng_CB->arrayForAllMApsInformation != NULL) {
        for (int i = 0; i < DISK_SIZE; i++) {
            disk_mng_CB->arrayForAllMApsInformation[i] = NULL;
        }
    }
}
void array_normalInitialize() {
    disk_mng_CB->arrayForAllMApsInformation = (ArrayInfo_t**)allocate_memory(sizeof(ArrayInfo_t*), "Failed to allocate memory for array ", "array_normalInitialize");
    int startAdress = 12 + disk_mng_CB->diskFreeIndexesInArray->size* sizeof(int);
    int howManyToLoad = DISK_SIZE * sizeof(ArrayInfo_t**);
    //load all the data from array
    disk_loadDataForInitializeDataStructers(&(disk_mng_CB->arrayForAllMApsInformation), &startAdress, &howManyToLoad);
}

void array_saveData()
{
    int startAdress = 12+disk_mng_CB->diskFreeIndexesInArray->size * sizeof(int)+DISK_SIZE * sizeof(ArrayInfo_t**);
    int howManyToLoad =DISK_SIZE* (sizeof(ArrayInfo_t**));
    //save all the data from array
    disk_saveDataFromStructersToDisk(&(disk_mng_CB->arrayForAllMApsInformation), &startAdress, &howManyToLoad);
}


ArrayInfo_t* arrayInfo_create(int* diskPointer, int size, MapRange_t* range,AVLNodeInfo_t* avlNodeInfo)
{
    ArrayInfo_t* arrayInfo = (ArrayInfo_t*)allocate_memory(sizeof(ArrayInfo_t), "Failed to allocate memory for avlTree", "arrayInfo_create");
    disk_mng_CB->mapIdIndex++;
    arrayInfo->mapid = disk_mng_CB->mapIdIndex;
    arrayInfo ->diskPointer = diskPointer;
    arrayInfo->size = size;
    arrayInfo->range =range;
    arrayInfo->avlNodeInfo = avlNodeInfo;
    return arrayInfo;
}

void array_addToArray(ArrayInfo_t* arrayInfo, int index)
{
    disk_mng_CB->arrayForAllMApsInformation[index] = arrayInfo;
}

void array_deleteFromArray(int index)
{
    disk_mng_CB->arrayForAllMApsInformation[index] = NULL;
}

void array_deleteArrayInfo(ArrayInfo_t* arrayInfo)
{
    disk_deleteMap(arrayInfo->diskPointer);
    free(arrayInfo);
}

//maprange functions

MapRange_t* mapRange_create(Point_t bottomRight, Point_t topLeft) 
{
    MapRange_t* mapRange = (MapRange_t*)allocate_memory(sizeof(MapRange_t), "Failed to allocate memory for max heap", "mapRange_create");
    mapRange->bottomRight = bottomRight;
    mapRange->topLeft = topLeft;
    return mapRange;
}
int disk_mng_deleteMapFromDiskManagementDataStructures(int sizeToFree)
{
    AVLNode_t* nodeToDelete = avlTree_FindingTheNodeThatIsSuitableForDeletion(disk_mng_CB->disk_SortByMapSize->root);//find the suitable map to delete
    ArrayInfo_t* arrayInfo = disk_mng_CB->arrayForAllMApsInformation[nodeToDelete->avlNodeInfo->arrayIndex];//finding the object that corresponds to deletion in the array
    if (disk_mng_searchForSuitableMapInLoadingArray(arrayInfo->mapid)==-1)//check if the map not in loaded to the cache
    {
        bool isDeleteSuccess = disk_deleteMap(arrayInfo->diskPointer);//send API to the disk to delete the map
        if (isDeleteSuccess)
        {
            cache_deleteMap(arrayInfo->mapid);//send API to the cache to delete the map
            stack_push(nodeToDelete->avlNodeInfo->arrayIndex);//push the index to the stack of free indexes
            sizeToFree -= arrayInfo->size;//reducing the size that needs to be deleted
            array_deleteFromArray(nodeToDelete->avlNodeInfo->arrayIndex);//delete the map from the array
            array_deleteArrayInfo(arrayInfo);//delete the array object
            disk_mng_CB->disk_SortByMapSize->root = avlTree_deleteNode(disk_mng_CB->disk_SortByMapSize->root, nodeToDelete);//delete the map from the AVL tree
            disk_mng_CB->disk_SortByMapSize->totalElements--;
            avlNode_delete(nodeToDelete);//delete the avl node

        }
        else
        {
            test_writeExceptionToFile(Error_When_Deleting_Map_from_Disk, "disk_mng_deleteMapFromDiskManagementDataStructures");
        }

    }
}

void disk_mng_delete(int mapSize)
{
    while (mapSize>0)
    {
        mapSize = disk_mng_deleteMapFromDiskManagementDataStructures(mapSize);
    }
}

//AVL node info
AVLNodeInfo_t* avlNodeInfo_create(int mapSize, int arrayIndex)
{
    AVLNodeInfo_t* newAVLNodeInfo = (AVLNodeInfo_t*)allocate_memory(sizeof(AVLNodeInfo_t), "Failed to allocate memory for AVL node info", "avlNodeInfo_create");
    newAVLNodeInfo->mapSize = mapSize;
    newAVLNodeInfo->arrayIndex = arrayIndex;
    newAVLNodeInfo->lru = disk_mng_CB->disk_SortByMapSize->lruCounter+1;
    return newAVLNodeInfo;
}

void avlNodeInfo_delete(AVLNodeInfo_t* nodeInfo)
{
    if (nodeInfo != NULL) {
        free(nodeInfo);
    }
}

//AVL node 
AVLNode_t* avlNode_create(AVLNodeInfo_t* avlNodeInfo) {
    AVLNode_t* node = (AVLNode_t*)allocate_memory(sizeof(AVLNode_t), "Failed to allocate memory for AVL node", "avlTree_insert");
    node->avlNodeInfo = avlNodeInfo;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}
void avlNode_delete(AVLNode_t* node) {
    if (node != NULL) {
        avlNodeInfo_delete(node->avlNodeInfo);//?
        free(node);
    }
}

int avlNode_height(AVLNode_t* N) {
    if (N == NULL)
        return 0;
    return N->height;
}



AVLNode_t* avlTree_rightRotate(AVLNode_t* y) {
    AVLNode_t* x = y->left;
    AVLNode_t* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max(avlNode_height(y->left), avlNode_height(y->right)) + 1;
    x->height = max(avlNode_height(x->left), avlNode_height(x->right)) + 1;
    return x;
}

AVLNode_t* avlTree_leftRotate(AVLNode_t* x) {
    AVLNode_t* y = x->right;
    AVLNode_t* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max(avlNode_height(x->left), avlNode_height(x->right)) + 1;
    y->height = max(avlNode_height(y->left), avlNode_height(y->right)) + 1;
    return y;
}

int avlTree_getBalance(AVLNode_t* N) {
    if (N == NULL)
        return 0;
    return avlNode_height(N->left) - avlNode_height(N->right);
}
AVLNode_t* avlTree_minValueNode(AVLNode_t* node) {
    AVLNode_t* current = node;

    while (current->left != NULL)
        current = current->left;

    return current;
}
AVLNode_t* avlTree_insert(AVLNode_t* node, AVLNode_t* newNode) {
    if (node == NULL)
    {
        return newNode;
    }

    if (newNode->avlNodeInfo->mapSize < node->avlNodeInfo->mapSize)
        node->left = avlTree_insert(node->left, newNode);
    else
        node->right = avlTree_insert(node->right, newNode);

    node->height = 1 + max(avlNode_height(node->left), avlNode_height(node->right));

    int balance = avlTree_getBalance(node);

    if (balance > 1 && newNode->avlNodeInfo->mapSize < node->left->avlNodeInfo->mapSize)
        return avlTree_rightRotate(node);

    if (balance < -1 && newNode->avlNodeInfo->mapSize > node->right->avlNodeInfo->mapSize)
        return avlTree_leftRotate(node);

    if (balance > 1 && newNode->avlNodeInfo->mapSize > node->left->avlNodeInfo->mapSize) {
        node->left = avlTree_leftRotate(node->left);
        return avlTree_rightRotate(node);
    }

    if (balance < -1 && newNode->avlNodeInfo->mapSize < node->right->avlNodeInfo->mapSize) {
        node->right = avlTree_rightRotate(node->right);
        return avlTree_leftRotate(node);
    }

    return node;
}

void avlTree_firstInitialize() {
    disk_mng_CB->disk_SortByMapSize = (DiskSortByMapSize_t*)allocate_memory(sizeof(DiskSortByMapSize_t), "Failed to allocate memory for AVL Tree", "avlTree_initialize");
    disk_mng_CB->disk_SortByMapSize->root = NULL;
    disk_mng_CB->disk_SortByMapSize->totalElements = 0;
    disk_mng_CB->disk_SortByMapSize->lruCounter = 0;
}

void avlTree_normalInitialize() 
{
    disk_mng_CB->disk_SortByMapSize = (DiskSortByMapSize_t**)allocate_memory(sizeof(DiskSortByMapSize_t*), "Failed to allocate memory for stack ", "avlTree_normalInitialize");
    int length = 0;
    int startAdress = 8;
    int howManyToLoad = sizeof(int);
    //load the third address in disk with avlTree size
    disk_loadDataForInitializeDataStructers(&length, &startAdress, &howManyToLoad);
    startAdress = 12+ disk_mng_CB->diskFreeIndexesInArray->size * sizeof(int) + DISK_SIZE * sizeof(ArrayInfo_t**);
    howManyToLoad = length * sizeof(DiskSortByMapSize_t**);
    //load all the data from avlTree
    disk_loadDataForInitializeDataStructers(&(disk_mng_CB->disk_SortByMapSize), &startAdress, &howManyToLoad);
}

void avlTree_saveData() 
{
    int startAdress = 8;
   int  howManyToLoad = sizeof(int);
   ////save size of avkTree in the third address in the disk
   disk_saveDataFromStructersToDisk(&(disk_mng_CB->disk_SortByMapSize->totalElements), &startAdress, &howManyToLoad);


   startAdress = 12+ disk_mng_CB->diskFreeIndexesInArray->size * sizeof(int) + DISK_SIZE * sizeof(ArrayInfo_t**) + DISK_SIZE * (sizeof(ArrayInfo_t**));
   howManyToLoad = disk_mng_CB->disk_SortByMapSize->totalElements * sizeof(int);
   //save all the data from stack
   disk_saveDataFromStructersToDisk(&(disk_mng_CB->disk_SortByMapSize), &startAdress, &howManyToLoad);
}


void avlTree_insertElement(AVLNodeInfo_t* nodeInfo) {
    disk_mng_CB->disk_SortByMapSize->lruCounter++;
    AVLNode_t* newNode = avlNode_create(nodeInfo);
    disk_mng_CB->disk_SortByMapSize->root = avlTree_insert(disk_mng_CB->disk_SortByMapSize->root, newNode);
    disk_mng_CB->disk_SortByMapSize->totalElements++;
}


AVLNode_t* avlTree_FindingTheNodeThatIsSuitableForDeletion(AVLNode_t* node) {
    if (node == NULL) {
        return NULL;
    }

    AVLNode_t* eligibleNode = NULL;
    AVLNode_t* largestNode = node;

    // Check the right subtree first for the largest node
    AVLNode_t* rightResult = avlTree_FindingTheNodeThatIsSuitableForDeletion(node->right);
    if (rightResult != NULL) {
        if (rightResult->avlNodeInfo->lru <= disk_mng_CB->disk_SortByMapSize->lruCounter * 0.7) {
            if (eligibleNode == NULL || rightResult->avlNodeInfo->mapSize > (eligibleNode ? eligibleNode->avlNodeInfo->mapSize : 0)) {
                eligibleNode = rightResult;
            }
        }
        if (rightResult->avlNodeInfo->mapSize > largestNode->avlNodeInfo->mapSize) {
            largestNode = rightResult;
        }
    }

    // Check if the current node is eligible for deletion
    if (node->avlNodeInfo->lru <= disk_mng_CB->disk_SortByMapSize->lruCounter * 0.7) {
        if (eligibleNode == NULL || node->avlNodeInfo->mapSize > eligibleNode->avlNodeInfo->mapSize) {
            eligibleNode = node;
        }
    }
    if (node->avlNodeInfo->mapSize > largestNode->avlNodeInfo->mapSize) {
        largestNode = node;
    }

    // Check the left subtree if no eligible node found in the right subtree or current node
    AVLNode_t* leftResult = avlTree_FindingTheNodeThatIsSuitableForDeletion(node->left);
    if (leftResult != NULL) {
        if (leftResult->avlNodeInfo->lru <= disk_mng_CB->disk_SortByMapSize->lruCounter * 0.7) {
            if (eligibleNode == NULL || leftResult->avlNodeInfo->mapSize > eligibleNode->avlNodeInfo->mapSize) {
                eligibleNode = leftResult;
            }
        }
        if (leftResult->avlNodeInfo->mapSize > largestNode->avlNodeInfo->mapSize) {
            largestNode = leftResult;
        }
    }

    // Return the eligible node if found, otherwise return the node with the largest mapSize
    return eligibleNode ? eligibleNode : largestNode;
}
AVLNode_t* avlTree_deleteNode(AVLNode_t* root, AVLNode_t* node) {
    if (root == NULL) {
        return root;
    }

    // STEP 1: PERFORM STANDARD BST DELETE
    if (node->avlNodeInfo->mapSize < root->avlNodeInfo->mapSize) {
        root->left = avlTree_deleteNode(root->left, node);
    }
    else if (node->avlNodeInfo->mapSize > root->avlNodeInfo->mapSize) {
        root->right = avlTree_deleteNode(root->right, node);
    }
    else {
        // This is the node to be deleted
        if ((root->left == NULL) || (root->right == NULL)) {
            AVLNode_t* temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL) {
                // Simply remove the current node
                root = NULL;

            }
            else {
                // One child case
                // Update root to point to the non-empty child
                *root = *temp;
                // Make sure to set the child pointers of the old node to NULL
                root->left = temp->left;
                root->right = temp->right;

            }

        }
        else {
            // Node with two children: Get the inorder successor (smallest in the right subtree)
            AVLNode_t* temp = avlTree_minValueNode(root->right);

            // Copy the inorder successor's data to this node
            root->avlNodeInfo = temp->avlNodeInfo;

            // Delete the inorder successor
            root->right = avlTree_deleteNode(root->right, temp);

        }
    }

    // If the tree had only one node then return
    if (root == NULL) {
        return root;
    }

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(avlNode_height(root->left), avlNode_height(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to check whether this node became unbalanced)
    int balance = avlTree_getBalance(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && avlTree_getBalance(root->left) >= 0) {
        return avlTree_rightRotate(root);
    }

    // Left Right Case
    if (balance > 1 && avlTree_getBalance(root->left) < 0) {
        root->left = avlTree_leftRotate(root->left);
        return avlTree_rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && avlTree_getBalance(root->right) <= 0) {
        return avlTree_leftRotate(root);
    }

    // Right Left Case
    if (balance < -1 && avlTree_getBalance(root->right) > 0) {
        root->right = avlTree_rightRotate(root->right);
        return avlTree_leftRotate(root);
    }

    return root;
}
void* allocate_memory(size_t size, const char* description, const char* functionName) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        test_writeExceptionToFile(Error_When_Allocating_Memory_Space, functionName);
        perror(description);
        return NULL;
    }
    return ptr;
}
void disk_mng_addMap(MapRange_t* range, int size, int* map)
{
    if (disk_mng_isTheDataCorrect(range,size,map))
    {
        int* diskPointer = disk_addMap(map);
        if (diskPointer != NULL)//success
        {
            if (!disk_isThereEnoughSpace(size))
            {
                void disk_mng_delete(int size);
            }
            disk_mng_addMapToDiskManagementDataStructures(range, size, diskPointer);
        }
        else
        {
            test_writeExceptionToFile(Error_When_Adding_Map_To_Disk, "disk_mng_addMap");
        }

    }
}

void disk_mng_addMapToDiskManagementDataStructures(MapRange_t* range, int size, int* diskPointer)
{
    int index = stack_pop();
    AVLNodeInfo_t* avlNodeInfo = avlNodeInfo_create(size, index);
    avlTree_insertElement(avlNodeInfo);
    ArrayInfo_t* arrayInfo = arrayInfo_create(diskPointer, size, range,avlNodeInfo);
    array_addToArray(arrayInfo, index);
     
}

bool disk_mng_isTheDataCorrect(MapRange_t* range, int size, int* map)
{
    if (map == NULL)
    {
        test_writeExceptionToFile(Error_Worng_Map_Variable, "disk_mng_checkDataStructures_map");
        return true;
    }
    if (size <=0)
    {
        test_writeExceptionToFile(Error_Worng_Size_Variable, "disk_mng_checkDataStructures_mapSize");
        return false;
    }
    if (isCorrectRange(range))
    {
        test_writeExceptionToFile(Error_Worng_Map_Range, "disk_mng_checkDataStructures_range");
        return false;
    }
    return true;
}
void test_writeExceptionToFile(Exception exception, const char* source) {
    FILE* file;
    errno_t err = fopen_s(&file, "errors.log.txt", "a");
    if (err != 0) {
        fprintf(stderr, "Error opening file for writing\n");
        return;
    }
    const char* error_message;
    switch (exception) {
    case Error_When_Allocating_Memory_Space:
        error_message = "Error: Error when allocating memory space.";
        break;
    case Error_When_Deleting_Map_from_Disk:
        error_message = "Error: Error when deleting map from disk.";
        break;
    case Error_When_Adding_Map_To_Disk:
        error_message = "Error: Error when adding map to disk.";
        break;
    case Error_Worng_Size_Variable:
        error_message = "Error: Error worng size variable.";
        break;
    case Error_Worng_Map_Variable:
        error_message = "Error: Error worng map variable.";
        break;
    case Error_Worng_Map_Range:
        error_message = "Error: Error worng map range.";
        break;
    default:
        error_message = "Error: Unknown exception.";
        break;
    }

    fprintf(file, "%s:\n%s\n", source, error_message);
    fclose(file);
}
bool isCorrectRange(MapRange_t* range)
{

    if (range->topLeft.x < 0 || range->topLeft.y < 0 || range->bottomRight.x<0 || range->bottomRight.y<0//negetavie
        || range->topLeft.x >  range->bottomRight.x || range->topLeft.y >  range->bottomRight.y //worng
        || range->topLeft.x<POINT_TL_RANGE.x || range->topLeft.y<POINT_TL_RANGE.y //not in disk range
        || range->bottomRight.x>POINT_BR_RANGE.x || range->bottomRight.y>POINT_BR_RANGE.y)//not in disk range
    {
        return false;
    }

    return true;
}

bool disk_mng_isTheMapInRange(MapRange_t* rangeFromCache, MapRange_t* range)
{
    if ((range->bottomRight.x >= rangeFromCache->topLeft.x >= range->topLeft.x) && (range->topLeft.y >= rangeFromCache->topLeft.y >= range->bottomRight.y) ||
        (range->bottomRight.x >= rangeFromCache->bottomRight.x >= range->topLeft.x) && (range->topLeft.y >= rangeFromCache->bottomRight.y >= range->bottomRight.y) ||
        (range->bottomRight.x >= rangeFromCache->topLeft.x >= range->topLeft.x) && (range->topLeft.y >= rangeFromCache->bottomRight.y >= range->bottomRight.y) ||
        (range->bottomRight.x >= rangeFromCache->bottomRight.x >= range->topLeft.x) && (range->topLeft.y >= rangeFromCache->topLeft.y >= range->bottomRight.y)   )
    {
    return true;
    }
    return false;
}


bool disk_mng_loadMapFromDiskToCache(int mapId, int offset, int size, int* freeAddress)
{
    int* diskPointer;
    int startAddress;
    bool isLoadedSuccess=false;
    int index = disk_mng_searchForSuitableMapInLoadingArray(mapId);
    if (index!=-1)//Checking whether the map was found in the loading array
    {
        diskPointer = disk_mng_CB->arrayForAllMApsInformation[index]->diskPointer;//Finding the appropriate disk pointer
        disk_mng_CB->arrayForAllMApsInformation[index] = NULL;
        startAddress = diskPointer+(offset*sizeof(int));
        isLoadedSuccess = disk_loadMapToCache(startAddress, size, freeAddress);
        if (!isLoadedSuccess)
        {
            test_writeExceptionToFile(Error_When_Load_Map_From_Disk_To_Cache, "disk_mng_loadMapFromDiskToCache");
        }

    }
    return isLoadedSuccess;
}

int disk_mng_searchForSuitableMapInLoadingArray(int mapId)
{
    DiskMapsInLoadedToCache_t* map;
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        map = disk_mng_CB->disk_MapsInLoadedToCache[i];
        if (map!=NULL&&mapId == map->mapId)
        {
            return map->index;
        }
    }
    return -1;
}
