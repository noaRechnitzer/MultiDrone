#include"Mock.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define DISK_SIZE 20
#define POINT_TL_RANGE (Point_t){0, 0}
#define POINT_BR_RANGE (Point_t){40, 40}

DiskMangmantCb_t* diskMangmantCb;

Point_t CreatePoint(int x, int y) 
{
    Point_t p;

    if (x < 0 || y < 0)
    {
        throwExceptiontoFile(POINT_NEGATIVE_ERORR);
        errno = POINT_NEGATIVE_ERORR;
    }

    p.x = x;
    p.y = y;
    return p;
}

ImagePoints_t CreateImagePoint(Point_t TL, Point_t BR)
{
    ImagePoints_t imgPoint;
    if (InvalidPoints(TL, BR))
    {
        imgPoint.BR = BR;
        imgPoint.TL = TL;
        return imgPoint;
    }
    imgPoint.BR.x = -1;
    imgPoint.BR.y = -1;
    imgPoint.TL.x = -1;
    imgPoint.TL.y = -1;
    return imgPoint;
}

bool isCorrectPoints(Point_t TL, Point_t BR)
{
    //Checks whether the points have not exceeded the limits and also the tl is smaller than the br
    if (TL.x > BR.x ||
        TL.y > BR.y ||
        TL.x<POINT_TL_RANGE.x ||
        TL.y<POINT_TL_RANGE.y ||
        BR.x>POINT_BR_RANGE.x ||
        BR.y>POINT_BR_RANGE.y)
    {
        throwExceptiontoFile(POINT_NOT_IN_RANGE_ERORR);
        errno = POINT_NOT_IN_RANGE_ERORR;
        return false;
    }

    return true;
}

bool isCorrectSize(Point_t TL, Point_t BR)
{
    //Checks if the image is the right size
    if (TL.x + SIZE_OF_IMG_LENGTH == BR.x && TL.y + SIZE_OF_IMG_WIDTH == BR.y)
    {
        return true;
    }
    throwExceptiontoFile(IMG_IS_NOT_IN_CORRECT_SIZE);
    errno = IMG_IS_NOT_IN_CORRECT_SIZE;
    return false;
}

bool InvalidPoints(Point_t TL, Point_t BR)
{
    if (!isCorrectPoints(TL, BR) || !isCorrectSize(TL, BR))
    {
        return false;
    }
    return true;
}

ImageInfo_t* CreateImageInfo(ImagePoints_t imgPoints, int* imgData)
{
    ImageInfo_t* img = (ImageInfo_t*)malloc(sizeof(ImageInfo_t));
    if (!img)
    {
        throwExceptiontoFile(ALLOCATE_ERROR);

        return NULL;
    }
    img->imgId = ++diskMangmantCb->imgIdCount;
    img->imgPoints.BR = imgPoints.BR;
    img->imgPoints.TL = imgPoints.TL;
    img->disk_ptr = addImgToDisk(imgPoints.TL, imgPoints.BR, imgData, img->imgId);

    return img;
}

void initDiskMangmantCb(void) 
{
    diskMangmantCb = (DiskMangmantCb_t*)malloc(sizeof(DiskMangmantCb_t));
    if (!diskMangmantCb)
    {
        throwExceptiontoFile(ALLOCATE_ERROR);
        return;
    }
    diskMangmantCb->linkedList = createLinkedList();
    diskMangmantCb->quadTree = createQuadTree(POINT_TL_RANGE, POINT_BR_RANGE);
    memset(diskMangmantCb->arraySearchInfo, NULL, sizeof(diskMangmantCb->arraySearchInfo));
    diskMangmantCb->imgIdCount = 0;

}

void printLinkList(FILE* file)
{
    if (diskMangmantCb->linkedList->head->next != NULL)
    {
        UnitNodeLinkedList_t* movingTheLoop = diskMangmantCb->linkedList->head->next;
        while (movingTheLoop != diskMangmantCb->linkedList->tail)
        {
            fprintf(file, "imgId= %d , TL=( %d , %d ), BR=( %d , %d ), diskPtr= %s \n", movingTheLoop->imgInfo->imgId, movingTheLoop->imgInfo->imgPoints.TL.x, movingTheLoop->imgInfo->imgPoints.TL.y,
                movingTheLoop->imgInfo->imgPoints.BR.x, movingTheLoop->imgInfo->imgPoints.BR.y, movingTheLoop->imgInfo->disk_ptr);
            movingTheLoop = movingTheLoop->next;
        }
    }
}

LinkedList_t* createLinkedList(void)
{
    LinkedList_t* linkedList = (LinkedList_t*)malloc(sizeof(LinkedList_t));
    if (!linkedList)
    {
        throwExceptiontoFile(ALLOCATE_ERROR);
        return NULL;
    }
    linkedList->head = (UnitNodeLinkedList_t*)malloc(sizeof(UnitNodeLinkedList_t));
    linkedList->tail = (UnitNodeLinkedList_t*)malloc(sizeof(UnitNodeLinkedList_t));
    if (!linkedList->head || !linkedList->tail) {
        throwExceptiontoFile(ALLOCATE_ERROR);
        return NULL;
    }
    linkedList->head->next = NULL;
    linkedList->tail->prev = NULL;
    linkedList->head->prev = NULL;
    linkedList->tail->next = NULL;
    linkedList->AmountOfLinks = 0;

    return linkedList;
}

UnitNodeLinkedList_t* createUnitNodeLinkedList(ImageInfo_t* imgInfo)
{
    UnitNodeLinkedList_t* node = (UnitNodeLinkedList_t*)malloc(sizeof(UnitNodeLinkedList_t));
    if (!node)
    {
        throwExceptiontoFile(ALLOCATE_ERROR);
        return NULL;
    }
    node->imgInfo = imgInfo;
    node->positionOnTree_ptr = NULL;
    node->next = NULL;
    node->prev = NULL;

    return node;
}

void insertToLinkList(UnitNodeLinkedList_t* node)
{
    //if it the first node to be insert
    if (diskMangmantCb->linkedList->head->next == NULL)
    {
        diskMangmantCb->linkedList->head->next = node;
        diskMangmantCb->linkedList->tail->prev = node;
        node->prev = diskMangmantCb->linkedList->head;
        node->next = diskMangmantCb->linkedList->tail;
    }
    else
    {
        node->prev = diskMangmantCb->linkedList->head;
        node->next = diskMangmantCb->linkedList->head->next;
        diskMangmantCb->linkedList->head->next = node;
        node->next->prev = node;
    }
    diskMangmantCb->linkedList->AmountOfLinks += 1;
}

QuadNode_t* createQuadNode(ImagePoints_t imagePoint) 
{
    QuadNode_t* quadNode = (QuadNode_t*)malloc(sizeof(QuadNode_t));
    //Checks that the assignment worked
    if (!quadNode) {
        throwExceptiontoFile(ALLOCATE_ERROR);
        return NULL;
    }
    //Initializes the data
    quadNode->imagePoints.BR = imagePoint.BR;
    quadNode->imagePoints.TL = imagePoint.TL;
    quadNode->parent = NULL;
    
    return quadNode;
}

void connectBetweenDatStructures(UnitNodeLinkedList_t* nodePtr, QuadNode_t* quadNode)
{
    quadNode->LinkedList_ptr = nodePtr;
    nodePtr->positionOnTree_ptr = quadNode;
}

QuadTree_t* createQuadTree(Point_t TL, Point_t BR)
{
    QuadTree_t* quadTree = (QuadTree_t*)malloc(sizeof(QuadTree_t));
    //Checks that the assignment worked
    if (!quadTree)
    {
        throwExceptiontoFile(ALLOCATE_ERROR);
        return NULL;
    }
    //Initializes the data
    quadTree->TL = TL;
    quadTree->BR = BR;
    quadTree->botLeftTree = NULL;
    quadTree->botRightTree = NULL;
    quadTree->topRightTree = NULL;
    quadTree->topLeftTree = NULL;
    quadTree->quadNode = NULL;
    quadTree->parent = NULL;
    quadTree->numOfNonNULLQuadTrees = 0;

    return quadTree;
}

void insertTotheQuadtree(QuadNode_t* node, QuadTree_t* quadTree)
{
    //Finds the intersection points that divide the space into four
    int midX = (quadTree->TL.x + quadTree->BR.x) / 2;
    int midY = (quadTree->TL.y + quadTree->BR.y) / 2;
    //The recursion breakpoint when the node found his correct place and everything is NULL 
    if (quadTree->quadNode == NULL && quadTree->topLeftTree == NULL &&
        quadTree->topRightTree == NULL && quadTree->botLeftTree == NULL &&
        quadTree->botRightTree == NULL)
    {
        quadTree->quadNode = node;
        node->parent = quadTree;
        return;
    }
    //The place I want to enter is taken
    if (quadTree->quadNode != NULL)
    {
        //If the point I want to add already exists
        if (quadTree->quadNode->imagePoints.TL.x == node->imagePoints.TL.x && quadTree->quadNode->imagePoints.TL.y == node->imagePoints.TL.y) {
            removeIfExist(quadTree->quadNode->LinkedList_ptr);
            QuadNode_t* saveToRemove = quadTree->quadNode;
            quadTree->quadNode = node;
            free(saveToRemove);
            return;
        }
        else
        {
            QuadNode_t* existingNode = quadTree->quadNode;
            quadTree->quadNode = NULL;
            // Checks whether to enter the TL
            if (existingNode->imagePoints.TL.x <= midX && existingNode->imagePoints.TL.y <= midY)
            {
                if (quadTree->topLeftTree == NULL)
                {
                    quadTree->topLeftTree = createQuadTree(quadTree->TL, CreatePoint(midX, midY));
                    quadTree->topLeftTree->parent = quadTree;
                    quadTree->numOfNonNULLQuadTrees += 1;
                }
                insertTotheQuadtree(existingNode, quadTree->topLeftTree);
            }
            else
            {
                // Checks whether to enter the BL
                if (existingNode->imagePoints.TL.x <= midX && existingNode->imagePoints.TL.y > midY)
                {
                    if (quadTree->botLeftTree == NULL)
                    {
                        quadTree->botLeftTree = createQuadTree(CreatePoint(quadTree->TL.x, midY + 1), CreatePoint(midX, quadTree->BR.y));
                        quadTree->botLeftTree->parent = quadTree;
                        quadTree->numOfNonNULLQuadTrees += 1;
                    }
                    insertTotheQuadtree(existingNode, quadTree->botLeftTree);
                }
                else
                {
                    // Checks whether to enter the TR
                    if (existingNode->imagePoints.TL.x > midX && existingNode->imagePoints.TL.y <= midY)
                    {
                        if (quadTree->topRightTree == NULL)
                        {
                            quadTree->topRightTree = createQuadTree(CreatePoint(midX + 1, quadTree->TL.y), CreatePoint(quadTree->BR.x, midY));
                            quadTree->topRightTree->parent = quadTree;
                            quadTree->numOfNonNULLQuadTrees += 1;
                        }
                        insertTotheQuadtree(existingNode, quadTree->topRightTree);
                    }
                    else // Checks whether to enter the BR
                    {
                        if (quadTree->botRightTree == NULL)
                        {
                            quadTree->botRightTree = createQuadTree(CreatePoint(midX + 1, midY + 1), quadTree->BR);
                            quadTree->botRightTree->parent = quadTree;
                            quadTree->numOfNonNULLQuadTrees += 1;
                        }
                        insertTotheQuadtree(existingNode, quadTree->botRightTree);
                    }
                }
            }
        }
    }
    // Check which child quadrant the node should go into
    if (node->imagePoints.TL.x <= midX) {
        if (node->imagePoints.TL.y <= midY) {
            // Top-left quadrant
            if (quadTree->topLeftTree == NULL)
            {
                quadTree->topLeftTree = createQuadTree(quadTree->TL, CreatePoint(midX, midY));
                quadTree->topLeftTree->parent = quadTree;
                quadTree->numOfNonNULLQuadTrees += 1;
            }
            insertTotheQuadtree(node, quadTree->topLeftTree);
        }
        else {
            // Bottom-left quadrant
            if (quadTree->botLeftTree == NULL)
            {
                quadTree->botLeftTree = createQuadTree(CreatePoint(quadTree->TL.x, midY + 1), CreatePoint(midX, quadTree->BR.y));
                quadTree->botLeftTree->parent = quadTree;
                quadTree->numOfNonNULLQuadTrees += 1;
            }
            insertTotheQuadtree(node, quadTree->botLeftTree);
        }
    }
    else
    {
        if (node->imagePoints.TL.y <= midY)
        {
            //Top-right quadrant
            if (quadTree->topRightTree == NULL)
            {
                quadTree->topRightTree = createQuadTree(CreatePoint(midX + 1, quadTree->TL.y), CreatePoint(quadTree->BR.x, midY));
                quadTree->topRightTree->parent = quadTree;
                quadTree->numOfNonNULLQuadTrees += 1;
            }
            insertTotheQuadtree(node, quadTree->topRightTree);
        }
        else {
            //Bottom-right quadrant
            if (quadTree->botRightTree == NULL)
            {
                quadTree->botRightTree = createQuadTree(CreatePoint(midX + 1, midY + 1), CreatePoint(quadTree->BR.x, quadTree->BR.y));
                quadTree->botRightTree->parent = quadTree;
                quadTree->numOfNonNULLQuadTrees += 1;
            }
            insertTotheQuadtree(node, quadTree->botRightTree);
        }
    }
}

void searchImgsAtQuadTreeByRange(QuadTree_t* quadTree, Point_t TL, Point_t BR, int* count, int* idArray)
{
    if (quadTree == NULL)
        return;
    //if the range is nagetive values
    if (TL.x < 0 || TL.y < 0 || BR.x < 0 || BR.y < 0) {
        throwExceptiontoFile(POINT_NEGATIVE_ERORR);
        errno = POINT_NEGATIVE_ERORR;
        return;
    }
    //if the searching range out of quadTree range
    if (quadTree->TL.x >= BR.x || quadTree->BR.x <= TL.x ||
        quadTree->TL.y >= BR.y || quadTree->BR.y <= TL.y) {
        return;
    }
    //chack if has quadNode
    if (quadTree->quadNode != NULL) {
        if (quadTree->quadNode->imagePoints.TL.x >= TL.x
            && quadTree->quadNode->imagePoints.TL.y >= TL.y
            && quadTree->quadNode->imagePoints.BR.y <= BR.y
            && quadTree->quadNode->imagePoints.BR.x <= BR.x) {
            //if don't have enough place
            if (*count == SIZE_OF_CACHE)
            {
                return;
            }
            //enter into the arrays
            diskMangmantCb->arraySearchInfo[*count] = quadTree->quadNode->LinkedList_ptr->imgInfo;
            idArray[*count] = quadTree->quadNode->LinkedList_ptr->imgInfo->imgId;
            (*count)++;
        }
        return;
    }
    //recsive function
    searchImgsAtQuadTreeByRange(quadTree->topLeftTree, TL, BR, count, idArray );
    searchImgsAtQuadTreeByRange(quadTree->topRightTree, TL, BR, count, idArray);
    searchImgsAtQuadTreeByRange(quadTree->botLeftTree, TL, BR, count, idArray );
    searchImgsAtQuadTreeByRange(quadTree->botRightTree, TL, BR, count, idArray);
}

void removeIfExist(UnitNodeLinkedList_t* node)
{
    UnitNodeLinkedList_t* saveToRemove = node;
    node->next->prev = node->prev;
    node->prev->next = node->next;
    saveToRemove->next = NULL;
    saveToRemove->prev = NULL;
    diskMangmantCb->linkedList->AmountOfLinks--;
    free(saveToRemove);
}

void removeData(void)
{
    UnitNodeLinkedList_t* node = removeNodeFromLinkedList();
    //if the linkedLixt remove func return node
    if (node)
    {
        removeFromdArraySearchInfo(node->imgInfo->imgId);
        if (removeQuadNodeFromTree(node->positionOnTree_ptr))
        {
            free(node->imgInfo);
            free(node);
        }
    }
}

UnitNodeLinkedList_t* removeNodeFromLinkedList(void)
{
    UnitNodeLinkedList_t* saveToRemove;
    //Checking if it has links
    if (diskMangmantCb->linkedList->AmountOfLinks != 0)
    {
        //Checking if it has when one link
        if (diskMangmantCb->linkedList->AmountOfLinks != 1)
        {
            saveToRemove = diskMangmantCb->linkedList->tail->prev;
            diskMangmantCb->linkedList->tail->prev = diskMangmantCb->linkedList->tail->prev->prev;
            diskMangmantCb->linkedList->tail->prev->next = diskMangmantCb->linkedList->tail;
            diskMangmantCb->linkedList->AmountOfLinks--;
            return saveToRemove;
        }
        else
        {
            saveToRemove = diskMangmantCb->linkedList->tail->prev;
            diskMangmantCb->linkedList->head->next = NULL;
            diskMangmantCb->linkedList->tail->prev = NULL;
            diskMangmantCb->linkedList->AmountOfLinks--;

            return saveToRemove;
        }
    }
    else
    {
        printf("no node to remove");
        return NULL;
    }
}

void removeFromdArraySearchInfo(int id)
{
    //runing on the arraySearchInfo to find the id
    for (int i = 0; i < diskMangmantCb->lengthOfArraySearchInfo; i++)
    {
        //if found id
        if (diskMangmantCb->arraySearchInfo[i]->imgId == id)
        {
            diskMangmantCb->arraySearchInfo[i] = diskMangmantCb->arraySearchInfo[diskMangmantCb->lengthOfArraySearchInfo - 1];
            diskMangmantCb->arraySearchInfo[diskMangmantCb->lengthOfArraySearchInfo - 1] = NULL;
            diskMangmantCb->lengthOfArraySearchInfo -= 1;
            return;
        }
    }
    return;
}

bool removeQuadNodeFromTree(QuadNode_t* quadNode) {
    QuadTree_t* saveToRemove;
    //check if has a grandFather
    if (quadNode->parent->parent != NULL)
    {
        QuadTree_t* grandfather = quadNode->parent->parent;
        //Boolean variable if you entered the while loop
        bool ifWhile = false;
        bool isTowSons = false;//if the quadTree have two sons 
        //chack if the grandFather have two qaudTree that they that are not null
        if (grandfather->numOfNonNULLQuadTrees == 2) {
            //check if the second grandFather son is TL
            if (grandfather->topLeftTree != NULL && grandfather->topLeftTree->quadNode != NULL && grandfather->topLeftTree->quadNode != quadNode) {
                saveToRemove = grandfather->topLeftTree;
                grandfather->topLeftTree->quadNode->parent = grandfather;
                grandfather->quadNode = grandfather->topLeftTree->quadNode;
                grandfather->topLeftTree = NULL;
                grandfather->numOfNonNULLQuadTrees -= 1;
                isTowSons = true;
                free(saveToRemove);
            }
            //check if the second grandFather son is TR
            if (grandfather->topRightTree != NULL && grandfather->topRightTree->quadNode != NULL && grandfather->topRightTree->quadNode != quadNode) {
                saveToRemove = grandfather->topRightTree;
                grandfather->topRightTree->quadNode->parent = grandfather;
                grandfather->quadNode = grandfather->topRightTree->quadNode;
                grandfather->topRightTree = NULL;
                grandfather->numOfNonNULLQuadTrees -= 1;
                isTowSons = true;
                free(saveToRemove);
            }
            //check if the second grandFather son is BL
            if (grandfather->botLeftTree != NULL && grandfather->botLeftTree->quadNode != NULL && grandfather->botLeftTree->quadNode != quadNode) {
                saveToRemove = grandfather->botLeftTree;
                grandfather->botLeftTree->quadNode->parent = grandfather;
                grandfather->quadNode = grandfather->botLeftTree->quadNode;
                grandfather->botLeftTree = NULL;
                grandfather->numOfNonNULLQuadTrees -= 1;
                isTowSons = true;
                free(saveToRemove);
            }
            //check if the second grandFather son is BR
            if (grandfather->botRightTree != NULL && grandfather->botRightTree->quadNode != NULL && grandfather->botRightTree->quadNode != quadNode) {
                saveToRemove = grandfather->botRightTree;
                grandfather->botRightTree->quadNode->parent = grandfather;
                grandfather->quadNode = grandfather->botRightTree->quadNode;
                grandfather->botRightTree = NULL;
                grandfather->numOfNonNULLQuadTrees -= 1;
                isTowSons = true;
                free(saveToRemove);
            }
            
            //while the quadNode perent isn't the root and he singal son
            while (grandfather->parent != NULL && grandfather->parent->numOfNonNULLQuadTrees == 1 && isTowSons==true) {
                ifWhile = true;
                saveToRemove = grandfather;
                grandfather->parent->quadNode = grandfather->quadNode;
                grandfather->quadNode->parent = grandfather->parent;
                grandfather = grandfather->parent;
                free(saveToRemove);
            }


        }
        grandfather->numOfNonNULLQuadTrees -= 1;
        //entered the while
        if (!ifWhile)
        {
            //if the grandfather->botLeftTree was free
            if (grandfather->botLeftTree == quadNode->parent)
            {
                grandfather->botLeftTree = NULL;
            }
            else
            {
                //if the grandfather->botRightTree was free
                if (grandfather->botRightTree == quadNode->parent)
                {
                    grandfather->botRightTree = NULL;
                }
                else
                {
                    //if the grandfather->topLeftTree was free
                    if (grandfather->topLeftTree == quadNode->parent)
                    {
                        grandfather->topLeftTree = NULL;
                    }
                    //if the grandfather->topRightTree was free
                    else
                    {
                        grandfather->topRightTree = NULL;
                    }
                }
            }
        }
        else//else of ifwhile
        {
            grandfather->topRightTree = NULL;
            grandfather->topLeftTree = NULL;
            grandfather->botLeftTree = NULL;
            grandfather->botRightTree = NULL;
        }
        quadNode->LinkedList_ptr->positionOnTree_ptr = NULL;
        free(quadNode->parent);
        free(quadNode);
        return true;
    }
    else
    {
        quadNode->parent->botLeftTree = NULL;
        quadNode->parent->botRightTree = NULL;
        quadNode->parent->topLeftTree = NULL;
        quadNode->parent->topRightTree = NULL;
        quadNode->parent->numOfNonNULLQuadTrees = 0;
        quadNode->LinkedList_ptr->positionOnTree_ptr = NULL;
        free(quadNode);
        return true;
    }
}

void moveToTheBeginning(UnitNodeLinkedList_t* nodePtr)
{
    //Checks if there is only one link
    if (nodePtr->prev == diskMangmantCb->linkedList->head)
    {
        return;
    }
    //Moves the link to the beginning of the list and arranges the pointers
    nodePtr->next->prev = nodePtr->prev;
    nodePtr->prev->next = nodePtr->next;
    nodePtr->prev = diskMangmantCb->linkedList->head;
    nodePtr->next = diskMangmantCb->linkedList->head->next;
    diskMangmantCb->linkedList->head->next->prev = nodePtr;
    diskMangmantCb->linkedList->head->next = nodePtr;
}

bool loadImageFromDiskToCache(int imgId, int* addrassToLoading)
{
    for (int i = 0; i < diskMangmantCb->lengthOfArraySearchInfo; i++) {
        if (diskMangmantCb->arraySearchInfo[i]->imgId == imgId) {
            return loadImageToCache(diskMangmantCb->arraySearchInfo[i]->disk_ptr, addrassToLoading);
        }
    }
    return false;
}

int getImagesIdInRangedByTwoPoints(Point_t topLeft, Point_t bottomRight, int* arrayOfImagesId)
{
    diskMangmantCb->lengthOfArraySearchInfo = 0;
    searchImgsAtQuadTreeByRange(diskMangmantCb->quadTree, topLeft, bottomRight, &diskMangmantCb->lengthOfArraySearchInfo, arrayOfImagesId);
    return diskMangmantCb->lengthOfArraySearchInfo;
}

void addImgToDiskMangmant(Point_t TL, Point_t BR, int* imgData)
{
    //chack if it out the range
    if (!InvalidPoints)
    {
        return;
    }
    //disk is full
    if (diskMangmantCb->linkedList->AmountOfLinks == DISK_SIZE)
    {
        removeData();
    }
    initImg(TL, BR, imgData);//create and insert
}

void saveBeforeShutdown(void)
{
    saveTheQuadTreeToDisk(diskMangmantCb->quadTree);
    saveTheLinkedListToDisk(diskMangmantCb->linkedList);
    freeQuadTreeAndLinkedListAndArr();
}

void bootWhenTheDeviceIsTurnedOn(void)
{
    if (readingIsThisTheFirstTimeToTurnOn() == 0)//if is a first time that turnd on
    {
        initDiskMangmantCb();
        wirteItWasTrunedOn(1);
        return;
    }
    initDiskMangmantCb();
    diskMangmantCb->linkedList = loadTheLinkedlist();
    diskMangmantCb->quadTree = loadTheQuadTree();
}

void saveTheQuadTreeToDisk(QuadTree_t* pointerToQuadTreeTable)
{
    FILE* file;
    errno_t err = fopen_s(&file, ".\\diskMoceFolder\\DiskQuedTree.txt", "w");
    //if didn't succeed to open
    if (err != 0)
    {
        printf("The file didn't open, try again ---------------Exception\n");
        return;
    }
    printQuadTree(1, pointerToQuadTreeTable, file);
    fclose(file);
}

void saveTheLinkedListToDisk(LinkedList_t* pointerToLinkedListTable)
{
    FILE* file;
    errno_t err = fopen_s(&file, ".\\diskMoceFolder\\DisklinkedList.txt", "w");
    // If failed to open the file
    if (err != 0)
    {
        printf("The file didn't open, try again ---------------Exception\n");
        return;
    }
    printLinkList(file);
    fclose(file);
}

bool loadImageToCache(int* diskAddress, int* cachAddress)
{
    //cachAddress = *(diskAddress);
    return true;
}

void deleteImageFromDisk(int* diskAdress)
{

}

int* addImgToDisk(Point_t TL, Point_t BR, char* imgData, int imgId)
{
    FILE* file;
    errno_t err;
    // Attempt to open the file using fopen_s
    err = fopen_s(&file, ".\\diskMoceFolder\\Disk.txt", "a");
    if (err != 0) {
        // Print an error message using perror or fprintf

        printf("the file didnt open try again ---------------Exceptin");
        return;
    }
    // Write the exception to the file
    fprintf(file, "data:=%s ", imgData);
    fprintf(file, ",id image=%d TL.x=%d, TL.y=%d, BR.x=%d, BR.y=%d\n", imgId, TL.x, TL.y, BR.x, BR.y);
    // Close the file
    fclose(file);

    return NULL;
}


void wirteItWasTrunedOn(int num)
{
    FILE* file;
    errno_t err;
    // Attempt to open the file using fopen_s
    err = fopen_s(&file, ".\\diskMoceFolder\\FlagFlush.txt", "w");
    if (err != 0) {
        // Print an error message using perror or fprintf
        printf("the file didnt open try again ---------------Exceptin");
        return;
    }
    // Write the exception to the file
    fprintf(file, "%d\n", num);
    // Close the file
    fclose(file);
}

int readingIsThisTheFirstTimeToTurnOn()
{
    char num = '0';
    int flag;
    FILE* file;
    errno_t err;
    // Attempt to open the file using fopen_s
    err = fopen_s(&file, ".\\diskMoceFolder\\FlagFlush.txt", "w");
    if (err != 0) {
        // Print an error message using perror or fprintf
        printf("the file didnt open try again ---------------Exceptin");
        return;
    }
    // Read the number from the file
    size_t readCount = fread(num, sizeof(char), 1, file);
    if (readCount != 1 && readCount != 0) {
        printf("the didnt  succsess to read ---------------Exceptin");
        return 0;
    }
    if (readCount == 0)
        num = '0';
    flag = num - '0';
    // Close the file
    fclose(file);
    return flag;
}

QuadTree_t* loadTheQuadTree(void)
{
    return NULL;
}

LinkedList_t* loadTheLinkedlist(void)
{
    return NULL;
}

char* EnumToString(Exception exception)
{
    switch (exception)
    {
    case POINT_NEGATIVE_ERORR:
        return "point is negative";
    case POINT_NOT_IN_RANGE_ERORR:
        return "point not in range";
    case ALLOCATE_ERROR:
        return "error in allocat";
    case IMG_IS_NOT_IN_CORRECT_SIZE:
        return "img isn't in correct size";
    }
}

void throwExceptiontoFile(Exception exception) 
{
    FILE* file;
    errno_t err;
    // Attempt to open the file using fopen_s
    err = fopen_s(&file, ".\\diskMoceFolder\\Exceptions.txt", "a");
    if (err != 0) {
        // Print an error message using perror or fprintf
        printf("the file didnt open try again ---------------Exceptin");
        return;
    }
    char* exceptionString = EnumToString(exception);
    // Write the exception to the file
    fprintf(file, "%s\n", exceptionString);
    // Close the file
    fclose(file);
}

void freeQuadTreeAndLinkedListAndArr(void) 
{
    while (diskMangmantCb->linkedList->AmountOfLinks!=0)//As long as there are nodes in the linkedList
        removeData();
    free(diskMangmantCb->quadTree);//free the root of a quadTree
    free(diskMangmantCb->linkedList->head);// free the linkedList head
    free(diskMangmantCb->linkedList->tail);// free the linkedList tail
    free(diskMangmantCb->linkedList);//free the linkedList
    free(diskMangmantCb);
}

void initImg(Point_t TL, Point_t BR, int* imgData)
{
    ImagePoints_t tlrb = CreateImagePoint(TL, BR);
    ImageInfo_t* imgInfo = CreateImageInfo(tlrb, imgData);
    UnitNodeLinkedList_t* node = createUnitNodeLinkedList(imgInfo);
    QuadNode_t* quadNode = createQuadNode(tlrb);
    connectBetweenDatStructures(node, quadNode);
    //inserting in to the linkedList & quadTree
    insertToLinkList(node);
    insertTotheQuadtree(quadNode, diskMangmantCb->quadTree);
    return;
}

void printQuadTree(int depth, QuadTree_t* quadTree, FILE* file)
{
    if (!quadTree)
        return;
    if (!file)
        return;
    //Print the current node's bounds
    for (int i = 0; i < depth; i++)
        fprintf(file, "%s", "   ");

    fprintf(file, "QuadTree Node: TL(%d,%d) - BR(%d,%d) num(%d)   \n",
        quadTree->TL.x, quadTree->TL.y,
        quadTree->BR.x, quadTree->BR.y,
        quadTree->numOfNonNULLQuadTrees);

    if (quadTree->quadNode != NULL)
    {
        for (int i = 0; i < depth + 1; i++)
            fprintf(file, "    ");

        fprintf(file, "  QuadNode: TL(%d,%d) - BR(%d,%d)\n",
            quadTree->quadNode->imagePoints.TL.x, quadTree->quadNode->imagePoints.TL.y,
            quadTree->quadNode->imagePoints.BR.x, quadTree->quadNode->imagePoints.BR.y);
    }
    printQuadTree(depth + 1, quadTree->topLeftTree, file);
    printQuadTree(depth + 1, quadTree->topRightTree, file);
    printQuadTree(depth + 1, quadTree->botLeftTree, file);
    printQuadTree(depth + 1, quadTree->botRightTree, file);
}

 



