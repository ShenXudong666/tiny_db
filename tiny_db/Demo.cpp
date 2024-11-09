#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "BPlusTree.h"


// 随机建立一棵树
void Test1(BPlusTree* pTree, int count)
{
    pTree->ClearTree();

    //srand( (unsigned)time( NULL ) );//这是一个种子，如果不要随机功能，请把此句话注释掉
    for (int i = 0; i < count; i++)
    {
        int x = rand() % 999 + 1;
        (void)pTree->Insert(x);
    }

    printf("successed!\n");
}

// 在树中查找某数据
void Test2(BPlusTree* pTree, int data)
{
    char sPath[255] = { 0, };

    (void)pTree->Search(data, sPath);
    printf("\n%s", sPath);
}

// 在树中插入某数据
void Test3(BPlusTree* pTree, int data)
{
    bool success = pTree->Insert(data);
    if (true == success)
    {
        printf("\nsuccessed!\n");
    }
    else
    {
        printf("\nfailed!\n");
    }
}

// 在树中删除某数据
void Test4(BPlusTree* pTree, int data)
{
    bool success = pTree->Delete(data);
    if (true == success)
    {
        printf("\nsuccessed!\n");
    }
    else
    {
        printf("\nfailed!\n");
    }
}

// 对树进行旋转
BPlusTree* Test5(BPlusTree* pTree)
{
    BPlusTree* pNewTree = pTree->RotateTree();
    delete pTree;
    printf("\nsuccessed!\n");
    return pNewTree;
}

// 打印
void Test6(BPlusTree* pTree)
{
    pTree->PrintTree();
}

// 对树进行检查
void Test7(BPlusTree* pTree)
{
    bool success = pTree->CheckTree();
    if (true == success)
    {
        printf("\nsuccessed!\n");
    }
    else
    {
        printf("\nfailed!\n");
    }
}


int main7(int argc, char* argv[])
{
    BPlusTree* pTree = new BPlusTree;

    int x = 1;
    int y = 0;
    while (0 != x)
    {
        printf("\n\n");
        printf("    *******************************************************************\n");
        printf("    *           欢迎进入B+树演示程序，请选择相应功能。                *\n");
        printf("    *           1。随机建立一棵B+树；                                 *\n");
        printf("    *           2。在B+树中查找一个数；                               *\n");
        printf("    *           3。在B+树中插入一个数；                               *\n");
        printf("    *           4。在B+树中删除一个数；                               *\n");
        printf("    *           5。对B+树旋转，进行重新平衡；                         *\n");
        printf("    *           6。显示整个B+树；                                     *\n");
        printf("    *           7。检查整个B+树；                                     *\n");
        printf("    *           0。退出程序；                                         *\n");
        printf("    *******************************************************************\n");
        printf("\n    您的选择是：");


        scanf("%d", &x);
        switch (x)
        {
        case 1:
            printf("请输入数据个数(10-150)：");
            scanf("%d", &y);
            Test1(pTree, y);
            break;

        case 2:
            printf("请输入要查找的数值：");
            scanf("%d", &y);
            Test2(pTree, y);
            break;

        case 3:
            printf("请输入要插入的数值：");
            scanf("%d", &y);
            Test3(pTree, y);
            break;

        case 4:
            printf("请输入要删除的数值：");
            scanf("%d", &y);
            Test4(pTree, y);
            break;

        case 5:
            pTree = Test5(pTree);
            break;

        case 6:
            Test6(pTree);
            break;

        case 7:
            Test7(pTree);
            break;

        case 0:
            delete pTree;
            return 0;
            break;

        default:
            break;
        }
    }

    delete pTree;
    return 0;
}
