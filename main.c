#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_CHAR 256    //假设最多256种字符

//哈夫曼树节点结构
typedef struct HuffmanNode{
    char character;                     //字符
    int frequency;                      //频率
    struct HuffmanNode *left, *right;   //左右子节点
}HuffmanNode;

//哈夫曼编码表
typedef struct{
    char character;
    char code[MAX_CHAR];
}HuffmanCode;

//初始化
void initialization()
{
    int n, m;
    char ch[MAX_CHAR];
    int fre[MAX_CHAR];
    HuffmanCode huffmanCodes[MAX_CHAR];
    HuffmanNode* root = NULL;

    printf("请输入字符集大小n：");
    scanf("%d", &n);
    printf("请输入字符和对应的频率:\n");
    for (int i = 0; i < n; i++) {
        printf("字符 %d: ", i + 1);
        scanf(" %c", &ch[i]);
        printf("频率 %d: ", i + 1);
        scanf("%d", &fre[i]);
    }
    //构建哈夫曼树
    root = BuildHuffmanTree(ch, fre, n);
    BuildHuffmanCodes(root, huffmanCodes);
}

//比较函数
int compare(const void *a, const void *b)
{
    return ((HuffmanNode *)a)->frequency - ((HuffmanNode *)b)->frequency;
}

//创建一个新的哈夫曼树节点
HuffmanNode* createNode(char character, int fre)
{
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->character = character;
    node->frequency = fre;
    node->left = node->right = NULL;
    return node;
}

//构建哈夫曼树
HuffmanNode* BuildHuffmanTree(char character[], int fre[], int n)
{
    HuffmanNode* node[MAX_CHAR];
    for(int i = 0; i < n; i++) node[i] = createNode(character[i], fre[i]);
        //以字符频率为基础，生成最小堆
        qsort(node,n,sizeof(HuffmanNode*),compare);

        //合并节点，知道树只剩一个节点
        while(n > 1){
            //取出频率最小的两个节点
            HuffmanNode *left = node[0], *right = node[1];

            //创建新的内部节点
            HuffmanNode* parent = createNode('\0',left->frequency + right->frequency);
            parent->left = left;
            parent->right = right;
            
            //将新的节点插入数组并重新排序
            node[0] = parent;
            for (int i = 1; i < n - 1; i++)
            {
                node[i] = node[i + 1];
            }
            n--;
            qsort(node,n,sizeof(HuffmanNode*),compare);
        }
        return node[0];//返回根结点
    
}
//保存哈夫曼树至huffman文件中
void saveHuffmanTreeToFile(HuffmanNode* root, FILE* fp)
{
    if(!root) return;
    //如果是叶子节点
    if(root->left == NULL && root->right  == NULL)
    {
        fprintf(fp,"%c:%d\n",root->character,root->frequency);
    }
    else{   //内部节点
        fprintf(fp, "#:%d\n", root->frequency);
    }
    //递归存储左右子树
    saveHuffmanTreeToFile(root->left, fp);
    saveHuffmanTreeToFile(root->right, fp);
}

void saveHuffmanTree(HuffmanNode* root, const char* filename)
{
    FILE* fp = fopen(filename, "wb");
    if(!fp){
        printf("打不开文件%s进行写入！\n",filename);
        return;
    }
    saveHuffmanTreeToFile(root, fp);
    fclose(fp);
    printf("哈夫曼树已保存至%s\n",filename);
}
//从Huffman文件中加载哈夫曼树
HuffmanNode* loadHuffmanTreeFromFile(FILE* fp)
{
    char ch;
    int fre;
    //读取第一个非空字符
    while ((ch = fgetc(fp)) == ' ' || ch == '\n' || ch == 'r');
    //当读取内部节点时
    if(ch == '#'){
        fscanf(fp, "%d", &fre);
        HuffmanNode* node = createNode('\0', fre);
        node->left = loadHuffmanTreeFromFile(fp);
        node->right = loadHuffmanTreeFromFile(fp);
        return node;
    }

    //叶子节点
    ungetc(ch, fp);   //放回字符
    fscanf(fp, "%c:%d\n", &ch, &fre);
    return createNode(ch, fre);    
}
HuffmanNode* loadHuffmanTree(const char* filename)
{
    FILE* f = fopen(filename, "r");
    if(!f){
        printf("无法打开文件%s读取\n",filename);
        return;
    }
    HuffmanNode* root = loadHuffmanTreeFromFile(f);
    fclose(f);
    printf("哈夫曼树已加载成功！\n")；
    return root;
}  


//生成哈夫曼编码
void generateHuffmanCodes(HuffmanNode* root, char* code, int length, HuffmanCode* huffmanCodes,int *codeIndex){
    if(root == NULL) return;

    //如果是叶节点，保存字符编码
    if(root->left == NULL && root->right == NULL){
        code[length] = '\0'; //终止符
        huffmanCodes[*codeIndex].character = root->character;
        strcpy(huffmanCodes[*codeIndex].code , code);
        (*codeIndex)++;
        return;
    }

    //递归左子树和右子树
    code[length] = '0';
    generateHuffmanCodes(root->left,code,length+1,huffmanCodes,codeIndex);
    code[length] = '1';
    generateHuffmanCodes(root->right,code,length+1,huffmanCodes,codeIndex);
}

//存储哈夫曼编码
void BuildHuffmanCodes(HuffmanNode* root, HuffmanCode* huffmanCodes)
{
    char code[MAX_CHAR];
    int codeIndex = 0;
    generateHuffmanCodes(root, code, 0, huffmanCodes, &codeIndex);
}


int main()
{
    char command;
    while(1)
    {
        printf("\n选择操作: \n");
        printf("I: 初始化\n");
        printf("C: 编码\n");
        printf("D: 解码\n");
        printf("P: 打印代码\n");
        printf("T: 打印哈夫曼树\n");
        printf("退出按 Q\n");
        printf("Enter command(I/C/D/P/T/Q):");
        scanf("%c",&command);
        switch(command){
            case 'I':initialization(); break;
            case 'C':coding();break;
            case 'D':Decoding();break;
            case 'P':Print();break;
            case 'T':TreePrint();break;
            case 'Q':printf("quit!\n");return 0;
            default: printf("无效选项！\n");
        }
    }


}