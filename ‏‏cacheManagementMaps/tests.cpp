
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest/doctest.h"
#include <stdio.h>

extern "C" {
#include "cacheManagement.h"
	extern controlBlock_t* controlBlock;

	//void printLinkedList(Node_t* head) {
	//    Node_t* temp = head;
	//    while (temp != NULL) {
	//        printf("Location: %d, SizeOfBytes: %d -> ", temp->data.location, temp->data.sizeOfBytes); // ����� ����� �� ������
	//        temp = (Node_t*)temp->next; // ���� ������ ����
	//    }
	//    printf("NULL\n"); // ��� ������
	//}
	// ������� ������ �� ����� ����
	void printQueue(Queue_t* queue) {
		QueueNode_t* current = queue->front;
		printf("queue - \n");
		while (current != NULL) {
			printLinkedList((current->data).linkedList);
			printf("MapID: %d, MapSizeInBytes: %d\n", current->data.mapID,
				current->data.mapSizeInBytes);
			current = current->next;
		}
		printf("\n");
	}

	int id = 777;
	int size = 400;

	TEST_CASE("Init") {
		initControlBlock();
	}

	TEST_CASE("ReadPieceOfMap")
	{
		RangeInDataStorage_t r1;
		r1.location = 10;
		r1.sizeOfBytes = 495;
		addNode(controlBlock->emptyPlacesByLocation, &r1);
		addNode(controlBlock->emptyPlacesBySize, &r1);
	
		RangeInDataStorage_t r2;
		r2.location = 0;
		r2.sizeOfBytes = 5;
		addNode(controlBlock->emptyPlacesByLocation, &r2);
		addNode(controlBlock->emptyPlacesBySize, &r2);
		
		RangeInDataStorage_t r3;
		r3.location = 7;
		r3.sizeOfBytes = 1;
		addNode(controlBlock->emptyPlacesByLocation, &r3);
		addNode(controlBlock->emptyPlacesBySize, &r3);

		RangeInDataStorage_t r4;
		r4.location = 5;
		r4.sizeOfBytes = 1;
		addNode(controlBlock->emptyPlacesBySize, &r4);
		addNode(controlBlock->emptyPlacesByLocation, &r4);
		

		RangeInDataStorage_t r5;
		r5.location = 8;
		r5.sizeOfBytes = 5;
		addNode(controlBlock->emptyPlacesBySize, &r5);
		addNode(controlBlock->emptyPlacesByLocation, &r5);
		


		/////////////////////////////////

		Node_t* lst = createNode(8, 1024);
		Node_t* tail = lst;
		insertInTail(&tail, 40, 5);
		insertInTail(&tail, 45, 5);

		MapInfo_t* mapInfoL = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mapInfoL->mapID = 8;
		mapInfoL->mapSizeInBytes = 1024;
		mapInfoL->linkedList = lst;


		Node_t* l1 = createNode(5, 100);
		Node_t* t1 = l1;
		insertInTail(&t1, 30, 5);
		insertInTail(&t1, 35, 5);

		MapInfo_t* mapInfoS = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mapInfoS->mapID = 5;
		mapInfoS->mapSizeInBytes = 100;
		mapInfoS->linkedList = l1;

		Node_t* l2 = createNode(67, 100);
		Node_t* t2 = l2;
		insertInTail(&t2, 20, 5);
		insertInTail(&t2, 25, 5);


		MapInfo_t* mapInfoM = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mapInfoM->mapID = 67;
		mapInfoM->mapSizeInBytes = 200;
		mapInfoM->linkedList = l2;

		addNode(controlBlock->MapsSortedByID, mapInfoL);
		insertToQueueArray(mapInfoL);
		
		addNode(controlBlock->MapsSortedByID, mapInfoS);
		insertToQueueArray(mapInfoS);

		addNode(controlBlock->MapsSortedByID, mapInfoM);
		insertToQueueArray(mapInfoM);


		//after I delete the root, the new root has to be r1
		printf("- = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = \n");

		printRangeInDataStorageTree(controlBlock->emptyPlacesBySize);
		printRangeInDataStorageTree(controlBlock->emptyPlacesByLocation);

		Node_t* lst1 = createNode(22, 50);
		Node_t* tail1 = lst1;
		insertInTail(&tail1, 0, 5);
		insertInTail(&tail1, 5, 5);
		insertInTail(&tail1, 10, 5);
		insertInTail(&tail1, 15, 5);

		controlBlock->EmptyPlaceInCache = 10;
		int offset = 10;//the offset in map
		int id = 22;
		int size = 50;//map size left to read

		removeMapFromCache(67);

		ReadNormalMap(22,500);

		/*printLinkedList(lst1);
		ValuesOfReadingMap_t v = ReadPieceOfMap(tail1, size, offset, id);
		printf("size - %d, offset - %d, id - %d\n",v.BitsLeftToRead,v.offset, id);
		printLinkedList(lst1);
		printf("**********************************************************\n");
		ValuesOfReadingMap_t v1 = ReadPieceOfMap(tail1->next, v.BitsLeftToRead, v.offset, id);
		printf("size - %d, offset - %d, id - %d\n", v1.BitsLeftToRead, v1.offset, id);
		printLinkedList(lst1);
		ValuesOfReadingMap_t v2 = ReadPieceOfMap(tail1->next->next, v1.BitsLeftToRead, v1.offset, id);
		printf("size - %d, offset - %d, id - %d\n", v2.BitsLeftToRead, v2.offset, id);
		printLinkedList(lst1);
		printf("********************************************************\n");

		printf("********************************************************\n");*/

	}
	/*TEST_CASE("TestReadNormalMap") {

	}*/

	TEST_CASE("TestLinkedList") {
		CHECK(createNode(0, 5)->data.location == 0);
		CHECK(createNode(0, 5)->data.sizeOfBytes == 5);
		Node_t* lst = createNode(id, size);
		Node_t* tail = lst;
		insertInTail(&tail, 0, 5);
		printf("%d\n", tail->data.location);
		insertInTail(&tail, 9, 10);
		printf("%d\n", tail->data.location);
		printLinkedList(lst);
	}


	TEST_CASE("TestRangeInDataStorageByLocation") 
	{

		printf("TestRangeInDataStorageByLocation\n");
		RangeInDataStorage_t r1;
		r1.location = 10;
		r1.sizeOfBytes = 3;
		addNode(controlBlock->emptyPlacesByLocation, &r1);
		CHECK(Abs(height(controlBlock->emptyPlacesByLocation->root->left) - height(controlBlock->emptyPlacesByLocation->root->right)) <= 1);

		RangeInDataStorage_t r2;
		r2.location = 0;
		r2.sizeOfBytes = 5;
		addNode(controlBlock->emptyPlacesByLocation, &r2);
		CHECK(Abs(height(controlBlock->emptyPlacesByLocation->root->left) - height(controlBlock->emptyPlacesByLocation->root->right)) <= 1);

		RangeInDataStorage_t r3;
		r3.location = 7;
		r3.sizeOfBytes = 1;
		addNode(controlBlock->emptyPlacesByLocation, &r3);
		printf("\n\n");
		printf("emptyPlacesByLocation\n");
		printRangeInDataStorageTree(controlBlock->emptyPlacesByLocation);
		printf("endEmptyPlacesByLocation\n");
		CHECK(Abs(height(controlBlock->emptyPlacesByLocation->root->left) - height(controlBlock->emptyPlacesByLocation->root->right)) <= 1);

#pragma region check that each node in the right location in tree
		CHECK(controlBlock->emptyPlacesByLocation->root->data == &r3);
		CHECK(controlBlock->emptyPlacesByLocation->root->left->data == &r2);
		CHECK(controlBlock->emptyPlacesByLocation->root->right->data == &r1);
#pragma endregion
		CHECK(FindRangeByLocation(controlBlock->emptyPlacesByLocation->root, 0)->data == &r2);
		CHECK_FALSE(FindRangeByLocation(controlBlock->emptyPlacesByLocation->root, 0)->data == &r3);
		//printTree((AVLNode_t*) & rangesByLocation);

		//after I delete the root, the new root has to be r1

		deleteNodeFromEmptyPlacesByLocation(7);
		CHECK(FindRangeByLocation(controlBlock->emptyPlacesByLocation->root, 7) == NULL);

		printRangeInDataStorageTree(controlBlock->emptyPlacesByLocation);
		CHECK(controlBlock->emptyPlacesByLocation->root->data == &r1);
		CHECK(controlBlock->emptyPlacesByLocation->root->left->data == &r2);
		CHECK(controlBlock->emptyPlacesByLocation->root->right->data == NULL);/*�� ���� �� ������ ���� �� ����� �� ���*/

		UpdateNodeInRangeByLocation((AVLNode_t*)&r2, 3);
		CHECK(((RangeInDataStorage_t*)(controlBlock->emptyPlacesByLocation->root->left->data))->sizeOfBytes == 3);

		printRangeInDataStorageTree(controlBlock->emptyPlacesByLocation);
		//10 3
		//0 5
		//1 7
	}


	TEST_CASE("TestRangeInDataStorageBySize") {
		printf("TestRangeInDataStorageBySize");
		RangeInDataStorage_t r1;
		r1.location = 10;
		r1.sizeOfBytes = 3;
		addNode(controlBlock->emptyPlacesBySize, &r1);
		CHECK(Abs(height(controlBlock->emptyPlacesBySize->root->left) - height(controlBlock->emptyPlacesBySize->root->right)) <= 1);
		
		RangeInDataStorage_t r2;
		r2.location = 0;
		r2.sizeOfBytes = 5;
		addNode(controlBlock->emptyPlacesBySize, &r2);
		CHECK(Abs(height(controlBlock->emptyPlacesBySize->root->left) - height(controlBlock->emptyPlacesBySize->root->right)) <= 1);

		RangeInDataStorage_t r3;
		r3.location = 7;
		r3.sizeOfBytes = 1;
		addNode(controlBlock->emptyPlacesBySize, &r3);
		CHECK(Abs(height(controlBlock->emptyPlacesBySize->root->left) - height(controlBlock->emptyPlacesBySize->root->right)) <= 1);

		RangeInDataStorage_t r4;
		r4.location = 5;
		r4.sizeOfBytes = 1;
		addNode(controlBlock->emptyPlacesBySize, &r4);
		CHECK(Abs(height(controlBlock->emptyPlacesBySize->root->left) - height(controlBlock->emptyPlacesBySize->root->right)) <= 1);

		RangeInDataStorage_t r5;
		r5.location = 8;
		r5.sizeOfBytes = 5;
		addNode(controlBlock->emptyPlacesBySize, &r5);
		CHECK(Abs(height(controlBlock->emptyPlacesBySize->root->left) - height(controlBlock->emptyPlacesBySize->root->right)) <= 1);

#pragma region check that eact node in the right location in tree
		CHECK(controlBlock->emptyPlacesBySize->root->data == &r1);
		CHECK(controlBlock->emptyPlacesBySize->root->left->data == &r3);
		CHECK(controlBlock->emptyPlacesBySize->root->right->data == &r2);


		//UpdateNodeInRangeBySize(rangesBySize.root, (AVLNode_t*)&r2, 0);
		//CHECK(r2.sizeOfBytes == 0);


#pragma endregion
		printf("\n\n\n");
		printRangeInDataStorageTree(controlBlock->emptyPlacesBySize);
		deleteNodeFromEmptyPlacesBySize(&r1);
		printRangeInDataStorageTree(controlBlock->emptyPlacesBySize);
		printf("\n\n\n");


		//CHECK(findNextNode(&rangesBySize, &r2, &compareRangeBySize)->data ==(void*)& r2);
		  // AVLNode_t* nextNode = findNextNode(rangesBySize.root,(AVLNode_t*) & r2, compareRangeBySize);
		  // CHECK(nextNode == (AVLNode_t*)&r3);
	}



	TEST_CASE("TestMapInfoTree")
	{
		// ����� �� AVL ���� MapInfo
		//linkedList for the mapInfo
		Node_t* lst = createNode(id, size);
		Node_t* tail = lst;
		insertInTail(&tail, 0, 5);
		insertInTail(&tail, 9, 10);


		MapInfo_t* mapInfoL = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mapInfoL->mapID = 8;
		mapInfoL->mapSizeInBytes = 1024;
		mapInfoL->linkedList = lst;

		MapInfo_t* mapInfoS = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mapInfoS->mapID = 5;
		mapInfoS->mapSizeInBytes = 100;
		mapInfoS->linkedList = lst;

		MapInfo_t* mapInfoM = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mapInfoM->mapID = 7;
		mapInfoM->mapSizeInBytes = 200;
		mapInfoM->linkedList = lst;

		addNode(controlBlock->MapsSortedByID, mapInfoL);
		CHECK(Abs(height(controlBlock->MapsSortedByID->root->left) - height(controlBlock->MapsSortedByID->root->right)) <= 1);
		addNode(controlBlock->MapsSortedByID, mapInfoS);
		CHECK(Abs(height(controlBlock->MapsSortedByID->root->left) - height(controlBlock->MapsSortedByID->root->right)) <= 1);
		addNode(controlBlock->MapsSortedByID, mapInfoM);
		CHECK(Abs(height(controlBlock->MapsSortedByID->root->left) - height(controlBlock->MapsSortedByID->root->right)) <= 1);

		//add each node synchronyzed to MapsSortedByID and to
#pragma region check that eact node in the righr location in tree
		CHECK(controlBlock->MapsSortedByID->root->data == mapInfoM);
		CHECK(controlBlock->MapsSortedByID->root->left->data == mapInfoS);
		CHECK(controlBlock->MapsSortedByID->root->right->data == mapInfoL);
#pragma endregion
		CHECK(FindMapInfoByID(controlBlock->MapsSortedByID->root, 8)->data == mapInfoL);


		removeMapFromCache(7);
		//CHECK(FindMapInfoByID(controlBlock->MapsSortedByID->root, 8)->data == NULL);

		printf("\n\n");
		//printMapInfoTree(controlBlock->MapsSortedByID);

		//after i delete the root, the new root has to be mapInfoL
		//CHECK(RucursiveDeleteMapFromMapsSortedByID(controlBlock->MapsSortedByID->root, 7)->data == mapInfoL);
		CHECK(controlBlock->MapsSortedByID->root->data == mapInfoL);
		CHECK(FindMapInfoByID(controlBlock->MapsSortedByID->root, 7) == NULL);
		CHECK(controlBlock->MapsSortedByID->root->left->data == mapInfoS);
		CHECK(FindRangeByLocation(controlBlock->emptyPlacesByLocation->root, 9) != NULL);
		printRangeInDataStorageTree(controlBlock->emptyPlacesByLocation);
	}



	TEST_CASE("queueTest") {
		Node_t* lst = createNode(id, size);
		Node_t* tail = lst;
		insertInTail(&tail, 7, 10);
		insertInTail(&tail, 2, 4);

		MapInfo_t* mi = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mi->mapID = 7;
		mi->mapSizeInBytes = 256;
		mi->linkedList = lst;

		MapInfo_t* mi1 = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mi1->mapID = 5;
		mi1->mapSizeInBytes = 55;
		mi1->linkedList = lst;

		MapInfo_t* mi2 = (MapInfo_t*)malloc(sizeof(MapInfo_t));
		mi2->mapID = 1;
		mi2->mapSizeInBytes = 54;
		mi2->linkedList = lst;


		Queue_t* q = createQueue();
		enqueue(q, *mi);
		enqueue(q, *mi1);

		insertToQueueArray(mi);
		insertToQueueArray(mi2);
		insertToQueueArray(mi1);


		//insertToQueueArray(mi);
		for (int i = 0; i < SIZE_OF_QUEUE_ARRAY; i++)
		{
			printf("%d- \n", i);
			printQueue(controlBlock->queueArray[i]);
		}
		//printQueue(queueArray[ind]);
	}
}

