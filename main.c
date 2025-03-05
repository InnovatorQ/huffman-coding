#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 256
#define MAX_CODE_LEN 100

#pragma warning(disable:4996)

typedef struct HuffmanNode {
    char character;
    int frequency;
    struct HuffmanNode* left, * right;
} HuffmanNode;

typedef struct {
    char character;
    char code[MAX_CODE_LEN];
} HuffmanCode;

// 函数声明
void initialization();
void coding(const char* treeFile, const char* inputFile, const char* outputFile);
void Decoding(const char* codefile, const char* treeFile, const char* textfile);
void Print(const char* codefile, const char* codePrint);
void printHuffmanTree(HuffmanNode* root, int depth, FILE* fp);
void TreePrint(const char* huffmanTree, const char* treeprint);
int compare(const void* a, const void* b);
HuffmanNode* createNode(char character, int fre);
HuffmanNode* BuildHuffmanTree(char character[], int fre[], int n);
void saveHuffmanTreeToFile(HuffmanNode* root, FILE* fp);
void saveHuffmanTree(HuffmanNode* root, const char* filename);
HuffmanNode* loadHuffmanTreeFromFile(FILE* fp);
HuffmanNode* loadHuffmanTree(const char* filename);
void generateHuffmanCodes(HuffmanNode* root, char* code, int length, HuffmanCode* huffmanCodes, int* codeIndex);
void BuildHuffmanCodes(HuffmanNode* root, HuffmanCode* huffmanCodes);
char* findHuffmanCode(char ch, HuffmanCode* codeTable, int tableSize);
void freeHuffmanTree(HuffmanNode* root);

// 释放哈夫曼树
void freeHuffmanTree(HuffmanNode* root) {
    if (!root) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

// 初始化
void initialization() {
    int n;
    char ch[MAX_CHAR];
    int fre[MAX_CHAR];

    printf("请输入字符集大小n：");
    scanf("%d", &n);

    printf("请输入字符和对应的频率:\n");
    for (int i = 0; i < n; i++) {
        int valid = 0;
        while (!valid) {
            valid = 1;
            printf("字符 %d: ", i + 1);
            scanf(" %c", &ch[i]);
            // 检查重复字符
            for (int j = 0; j < i; j++) {
                if (ch[j] == ch[i]) {
                    printf("字符%c已存在，请重新输入！\n", ch[i]);
                    valid = 0;
                    break;
                }
            }
        }
        printf("频率 %d: ", i + 1);
        scanf("%d", &fre[i]);
    }

    HuffmanNode* root = BuildHuffmanTree(ch, fre, n);
    saveHuffmanTree(root, "huffman.txt");
    freeHuffmanTree(root);
}

// 比较函数
int compare(const void* a, const void* b) {
    return (*(HuffmanNode**)a)->frequency - (*(HuffmanNode**)b)->frequency;
}

// 创建节点
HuffmanNode* createNode(char character, int fre) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->character = character;
    node->frequency = fre;
    node->left = node->right = NULL;
    return node;
}

// 构建哈夫曼树
HuffmanNode* BuildHuffmanTree(char character[], int fre[], int n) {
    HuffmanNode* nodes[MAX_CHAR];
    for (int i = 0; i < n; i++) {
        nodes[i] = createNode(character[i], fre[i]);
    }

    while (n > 1) {
        qsort(nodes, n, sizeof(HuffmanNode*), compare);
        HuffmanNode* left = nodes[0];
        HuffmanNode* right = nodes[1];
        HuffmanNode* parent = createNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;

        nodes[0] = parent;
        int original_n = n;
        for (int i = 1; i < original_n - 1; i++) {
            nodes[i] = nodes[i + 1];
        }
        n--;
    }
    return nodes[0];
}

// 保存哈夫曼树
void saveHuffmanTreeToFile(HuffmanNode* root, FILE* fp) {
    if (!root) return;
    if (!root->left && !root->right) {
        fprintf(fp, "%c:%d\n", root->character, root->frequency);
    }
    else {
        fprintf(fp, "#:%d\n", root->frequency);
    }
    saveHuffmanTreeToFile(root->left, fp);
    saveHuffmanTreeToFile(root->right, fp);
}

void saveHuffmanTree(HuffmanNode* root, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("无法打开文件 %s\n", filename);
        return;
    }
    saveHuffmanTreeToFile(root, fp);
    fclose(fp);
}

// 加载哈夫曼树
HuffmanNode* loadHuffmanTreeFromFile(FILE* fp) {
    char line[256];
    if (!fgets(line, sizeof(line), fp)) return NULL;

    char ch;
    int freq;
    if (sscanf(line, "%c:%d", &ch, &freq) != 2) return NULL;

    if (ch == '#') {
        HuffmanNode* node = createNode('\0', freq);
        node->left = loadHuffmanTreeFromFile(fp);
        node->right = loadHuffmanTreeFromFile(fp);
        return node;
    }
    else {
        return createNode(ch, freq);
    }
}

HuffmanNode* loadHuffmanTree(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("无法打开文件 %s\n", filename);
        return NULL;
    }
    HuffmanNode* root = loadHuffmanTreeFromFile(fp);
    fclose(fp);
    return root;
}

// 生成编码表
void generateHuffmanCodes(HuffmanNode* root, char* code, int depth, HuffmanCode* codes, int* index) {
    if (!root) return;
    if (depth >= MAX_CODE_LEN - 1) {
        printf("编码过长！\n");
        return;
    }

    if (!root->left && !root->right) {
        code[depth] = '\0';
        codes[*index].character = root->character;
        strcpy(codes[*index].code, code);
        (*index)++;
        return;
    }

    code[depth] = '0';
    generateHuffmanCodes(root->left, code, depth + 1, codes, index);
    code[depth] = '1';
    generateHuffmanCodes(root->right, code, depth + 1, codes, index);
}

// 编码函数
void coding(const char* treeFile, const char* inputFile, const char* outputFile) {
    HuffmanNode* root = loadHuffmanTree(treeFile);
    if (!root) return;

    HuffmanCode codes[MAX_CHAR];
    char code[MAX_CODE_LEN];
    int index = 0;
    generateHuffmanCodes(root, code, 0, codes, &index);

    // 计算总位数
    FILE* input = fopen(inputFile, "r");
    if (!input) {
        printf("无法打开输入文件%s\n", inputFile);
        freeHuffmanTree(root);
        return;
    }

    size_t totalBits = 0;
    char ch;
    while ((ch = fgetc(input)) != EOF) {
        for (int i = 0; i < index; i++) {
            if (codes[i].character == ch) {
                totalBits += strlen(codes[i].code);
                break;
            }
        }
    }
    rewind(input);

    // 计算补位
    int padding = (8 - (totalBits % 8)) % 8;

    // 写入编码文件
    FILE* output = fopen(outputFile, "wb");
    if (!output) {
        fclose(input);
        freeHuffmanTree(root);
        return;
    }
    fputc(padding, output);

    unsigned char buffer = 0;
    int bitCount = 0;
    while ((ch = fgetc(input)) != EOF) {
        char* currCode = NULL;
        for (int i = 0; i < index; i++) {
            if (codes[i].character == ch) {
                currCode = codes[i].code;
                break;
            }
        }
        if (!currCode) continue;

        for (int i = 0; currCode[i]; i++) {
            if (currCode[i] == '1') {
                buffer |= (1 << (7 - bitCount));
            }
            if (++bitCount == 8) {
                fwrite(&buffer, 1, 1, output);
                buffer = 0;
                bitCount = 0;
            }
        }
    }

    // 处理剩余位
    if (bitCount > 0) {
        fwrite(&buffer, 1, 1, output);
    }

    fclose(input);
    fclose(output);
    freeHuffmanTree(root);
    printf("编码完成\n");
}

// 解码函数
void Decoding(const char* codefile, const char* treeFile, const char* textfile) {
    HuffmanNode* root = loadHuffmanTree(treeFile);
    if (!root) {
        printf("哈夫曼树加载失败\n");
        return;
    }

    FILE* in = fopen(codefile, "rb");
    if (!in) {
        printf("无法打开编码文件 %s\n", codefile);
        freeHuffmanTree(root);
        return;
    }

    // 读取 padding 信息（首字节存储）
    int padding = fgetc(in);
    if (padding == EOF) {
        printf("编码文件读取失败\n");
        fclose(in);
        freeHuffmanTree(root);
        return;
    }

    FILE* out = fopen(textfile, "w");
    if (!out) {
        printf("无法创建解码文件 %s\n", textfile);
        fclose(in);
        freeHuffmanTree(root);
        return;
    }

    // 计算总位数
    fseek(in, 0, SEEK_END);
    long fileSize = ftell(in);
    long totalBits = (fileSize - 1) * 8 - padding;
    fseek(in, 1, SEEK_SET);  // 回到编码数据起始位置

    HuffmanNode* current = root;
    unsigned char byte;
    int bitsRead = 0;

    while (bitsRead < totalBits && fread(&byte, 1, 1, in) == 1) {
        for (int i = 7; i >= 0 && bitsRead < totalBits; i--) {
            int bit = (byte >> i) & 1;
            current = bit ? current->right : current->left;

            if (!current->left && !current->right) {  // 叶子节点
                fputc(current->character, out);
                current = root;
            }
            bitsRead++;
        }
    }

    fclose(in);
    fclose(out);
    freeHuffmanTree(root);
    printf("解码完成，结果存入 %s\n", textfile);
}

void Print(const char* codefile, const char* codePrint)
{
    FILE* inFile = fopen(codefile, "rb");
    FILE* outFile = fopen(codePrint, "w");

    if (!inFile || !outFile) {
        printf("无法打开%s或%s进行读写\n", codefile, codePrint);
        return;
    }

    printf("\n**编码文件 (`codefile.bin`) 内容：**\n");
    unsigned char buffer;
    int count = 0;

    while (fread(&buffer, 1, 1, inFile)) {  // 逐字节读取
        for (int i = 7; i >= 0; i--) {      // 遍历 8 位
            int bit = (buffer >> i) & 1;
            printf("%d", bit);
            fputc(bit ? '1' : '0', outFile);  // 写入 codeprint 文件
            count++;

            if (count % 50 == 0) {  // 每 50 位换行
                printf("\n");
                fputc('\n', outFile);
            }
        }
    }

    if (count % 50 != 0) printf("\n");

    fclose(inFile);
    fclose(outFile);
    printf("编码内容已存入 `codeprint`\n");
}

void printHuffmanTree(HuffmanNode* root, int depth, FILE* fp)
{
    if (!root) return;

    for (int i = 0; i < depth; i++) {
        printf("  ");    // 终端打印缩进
        fprintf(fp, "  ");  // 文件存储缩进
    }

    if (root->left == NULL && root->right == NULL) {
        printf("'%c' (%d)\n", root->character, root->frequency);
        fprintf(fp, "'%c' (%d)\n", root->character, root->frequency);
    }
    else {
        printf("# (%d)\n", root->frequency);
        fprintf(fp, "# (%d)\n", root->frequency);
    }

    printHuffmanTree(root->left, depth + 1, fp);
    printHuffmanTree(root->right, depth + 1, fp);
}

void TreePrint(const char* huffmanTree, const char* treeprint)
{
    HuffmanNode* root = loadHuffmanTree(huffmanTree);
    if (!root) {
        printf("哈夫曼树未加载，无法打印！\n");
        return;
    }
    FILE* fp = fopen(treeprint, "w");
    if (!fp) {
        printf("无法打开 `treeprint` 进行写入！\n");
        return;
    }

    printf("\n**哈夫曼树 (`huffman.txt`) 结构：**\n");
    printHuffmanTree(root, 0, fp);

    fclose(fp);
    printf("哈夫曼树已存入 `treeprint`\n");
}


// 主函数
int main() {
    char cmd;
    while (1) {
        printf("\n选项: I-初始化 C-编码 D-解码 P-打印 T-树 Q-退出\n");
        printf("输入命令: ");
        scanf(" %c", &cmd);
        switch (cmd) {
        case 'I': initialization(); break;
        case 'C': coding("huffman.txt", "tobetrans.dat", "codefile.bin"); break;
        case 'D': Decoding("codefile.bin", "huffman.txt", "textfile.txt"); break;
        case 'P': Print("codefile.bin", "codeprint.txt"); break;
        case 'T': TreePrint("huffman.txt", "treeprint.txt"); break;
        case 'Q': return 0;
        default: printf("无效命令\n");
        }
    }
}