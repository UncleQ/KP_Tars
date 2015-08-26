//////////////////////////////////////////////////////////////////////////   
//    
// ReliefF.cpp   
// Create : Fancy 2009-5-1 23:25   
// Describe : ReliefF 算法的主程序，对特征进行分析、提取(实现文件)   
// Version : 1.0   
//   
//////////////////////////////////////////////////////////////////////////   
   
//#include "stdafx.h"   
#include "ReliefF.h"   
#include "Resource.h"   
#include <comutil.h>   
   
#define IDS_ERROR_INIT "Memory malloc failed!"  
#define IDS_ERROR_FILE "Can not open the file!"  
#define IDS_ERROR_EXIT "%s file %d line error!"

	
int nRange1,nRange2;
//CProgressCtrl *progress;   
//CProgressCtrl *progress2;   
   
ReliefF::ReliefF()   
{   
    normal = 0;   
    unnormal = 0;   
}   
   
ReliefF::~ReliefF()   
{   
    //Add what you need   
}   
   
MATRIX* ReliefF::build_matrix(UINT width, UINT height)   
{   
    MATRIX *_matrix;   
    UINT i;   
   
    _matrix = (MATRIX*)malloc(sizeof(MATRIX));   
    if(!_matrix)   
        printf(IDS_ERROR_INIT);   
   
    _matrix->width  = width;   
    _matrix->height = height;   
    _matrix->data = (REAL**) malloc (height * sizeof (REAL*));   
    if (_matrix->data == NULL)   
        printf(IDS_ERROR_INIT);   
    for(i = 0; i < height; i++)   
    {   
        _matrix->data[i] = (REAL*)malloc(width * sizeof(REAL));   
        if(NULL == _matrix->data[i])   
            printf(IDS_ERROR_INIT);   
    }   
   
    return _matrix;   
}   
   
void ReliefF::err_exit(CHAR* file, UINT line)   
{   
    printf(IDS_ERROR_EXIT, file, line);   
    exit(0);   
}   
   
//////////////////////////////////////////////////////////////////////////   
//   
// Given the name of a file of numeric data,this routine counts   
// the numbers of rows and columns.It's assumed that the number   
// of entries is the same in each row,and an error is flagged if this   
// is not the case.   
//   
//////////////////////////////////////////////////////////////////////////   
void ReliefF::GetFileSize(CHAR* fileName, UINT* width, UINT* height)   
{   
    FILE *file;   
    UINT buf_size = 0xFFFF, _width = 0;   
    CHAR *buffer, *ptr;   
   
    *width = *height = 0;   
   
    buffer = (CHAR*)malloc(buf_size * sizeof(CHAR));   
    if(NULL == buffer)   
        printf(IDS_ERROR_INIT);   
   
    // Open price file - abort if filename invalid   
    file = fopen(fileName, "r");   
    if(NULL == file)   
    {   
        printf(IDS_ERROR_FILE, fileName);   
        err_exit(fileName, 0);   
    }   
   
    // Get number of entries in first row   
    //////////////////////////////////////////////////////////////////////////   
    //   
    // char* fgets(char* string, int n, FILE* stream);   
    // string 结果数据的首地址；n-1：一次读入数据长度，其默认值为1K，即1024；   
    // stream 文件指针   
    //   
    //////////////////////////////////////////////////////////////////////////   
    if(NULL != fgets(buffer, buf_size, file))   
    {   
        ++*height;   
        //////////////////////////////////////////////////////////////////////////   
        //   
        // char* strtok(char *s, char *delim);   
        // 分解字符串为一组字符串。s为要分解的字符串，delim为分隔符字符串。   
        // 首次调用时，s指向要分解的字符串，之后再次调用要把s设成NULL。   
        //   
        //////////////////////////////////////////////////////////////////////////   
        ptr = strtok(buffer, " e");   
        while (NULL != ptr)   
        {   
            ++*width;   
            ptr = strtok(NULL, " e");   
        }   
    }   
   
    *width /= 2;   
   
    // Count numbers of subsequent rows   
    //////////////////////////////////////////////////////////////////////////   
    //   
    // int feof(FILE* stream);   
    // 检测流上的文件结束符   
    //   
    //////////////////////////////////////////////////////////////////////////   
    while (!feof(file))   
    {   
        if (NULL != fgets(buffer, buf_size, file))   
        {   
            // if line is more than a NL char   
            if (strlen(buffer) > strlen("\n"))   
            {   
                ++*height;   
                _width = 0;   
                ptr = strtok(buffer, " e");   
                while (NULL != ptr)   
                {   
                    ++_width;   
                    ptr = strtok(NULL, " e");   
                }   
   
                _width /= 2;   
                if (*width != _width)   
                {   
                    printf("*width != _width");   
                }   
            }   
        }   
    }   
    free(buffer);   
}   
   
// Routine to build a decision tree, based on Quinlan's ID3 algorithm.   
NODE* ReliefF::ID3(MATRIX *matrix, NODE *parent, UINT target, UINT state)   
{   
    NEGENTROPY negentropy_struct;   
   
    NODE *node;   
    UINT n_vas = matrix->width, n_samples = matrix->height, i, j, split;   
    REAL **data = matrix->data;   
    REAL best_threshold, min_negentropy, _negentopy;   
    Mysort*sort = (Mysort*)malloc(n_vas * sizeof(Mysort));   
    REAL test;   
    // Allocate memory for this node   
    node = (NODE*)malloc(sizeof(NODE));   
    if (!node)   
        err_exit("Test", 0);   
   
    // Set up links in decision tree   
    // Set address of parent node   
    node->parent = parent;   
   
    // parent to child; not relevant for root node   
    if (NULL != parent)   
    {   
        // Pass address of this node to the parent node   
        if (ON == state)   
            parent->on = node;   
        else if (OFF == state)   
            parent->off = node;   
    }   
   
    //////////////////////////////////////////////////////////////////////////   
    //   
    // Select attribute with lowest negentropy for splitting. Scan through   
    // all attributes (except the target) and all data samples. This is   
    // pretty inefficient for data sets with repeated values, but will do   
    // for illustrative purposes   
    //   
    //////////////////////////////////////////////////////////////////////////   
   
    min_negentropy = 1.0;   
    progress2->SetRange(0, n_vas*n_samples);   
   
    for (i = 0; i < n_vas; i++)   
    {   
        min_negentropy = 10;   
   
        for (j = 0; j < n_samples; j++)   
        {   
            if (i != target)   
            {   
                progress2->StepIt();   
   
                // Set trial values for this  node...   
                // 基因号码   
                node->idx = i;   
                // 设置本身   
                node->threshold = data[i][j];   
   
                // ...and calculate the negentropy of this partition   
                // 计算信息熵   
                negentropy_struct = negentropy(data, n_samples, node, target);   
                // 信息熵   
                _negentopy = negentropy_struct.ne;   
   
                // If this negentropy is lower than any other, retain the index and threshold for future use   
                // 取信息最小负熵   
                if (_negentopy < min_negentropy)   
                {   
                    min_negentropy = _negentopy;   
                    if (3 == i)   
                        i = 3;// ?   
   
                    sort[i].genedata = min_negentropy;   
                    sort[i].index = i;   
                    split = i;   
                    best_threshold = data[i][j];   
                }   
            }   
        }   
        sort[i].index = i;   
        sort[i].genedata = min_negentropy;   
    }   
   
    bubble(sort, n_vas);   
    // 基因号   
    node->idx = split;   
    // 基因表达域值   
    node->threshold = best_threshold;   
   
    return node;   
}   
   
NEGENTROPY ReliefF::negentropy(REAL** data, UINT n_samples, NODE* local, UINT target)   
{   
    NEGENTROPY ret_val;   
    NODE *_node, *_parent;   
    UINT on_ctr, off_ctr, p1, p2, i, _match;   
    REAL p_on, p_off, negentropy_on, negentropy_off;   
   
    on_ctr = off_ctr = p1 = p2 = 0;   
   
    for (i =0; i < n_samples; i++)   
    {   
        _match = 1;   
        _node = local;   
        while (NULL != _node->parent)   
        {   
            _parent = _node->parent;   
            if (_node == _parent->on)   
            {   
                // if parent node is ON   
                // 不表达   
                if(data[_parent->idx][i] < _parent->threshold)   
                    _match = 0;   
            }   
            else if (_node == _parent->off)   
            {   
                // if parent node is OFF   
                // 表示表达   
                if (data[_parent->idx][i] >= _parent->threshold)   
                    _match = 0;   
            }   
            _node = _parent;   
        }   
   
        if (_match)   
        {   
            if (data[local->idx][i] >= local->threshold)   
            {   
                on_ctr++;   
                if (data[target][i] >= 0.5)   
                    p1++;   
            }   
            else   
            {   
                off_ctr++;   
                if (data[target][i] >= 0.5)   
                {   
                    p2++;   
                }   
            }   
        }   
    }   
   
    if (on_ctr > 0)   
    {   
        p_on = (REAL) p1 / (REAL)on_ctr;   
        p_off = 1 - p_on;   
        negentropy_on = -entropy(p_on) - entropy(p_off);   
    }   
    else   
        negentropy_on = 0.0;   
   
    // 2: Entropy of subtables with activation OFF   
   
    if (off_ctr > 0)   
    {   
        p_on = (REAL)p2 / (REAL)off_ctr;   
        p_off = 1 - p_on;   
        negentropy_off = entropy(p_on) - entropy(p_off);   
    }   
    else   
        negentropy_off = 0.0;   
   
    ret_val.ne = (negentropy_on * on_ctr + negentropy_off * off_ctr);   
    ret_val.ne /= (on_ctr + off_ctr);   
   
    //////////////////////////////////////////////////////////////////////////   
    //   
    // If all values examined were the same, set "ret_val.status" to   
    // the target value since this will be an end-of-branch node   
    //   
    //////////////////////////////////////////////////////////////////////////   
   
    if ((p1 == on_ctr) && (p2 == off_ctr))   
        ret_val.status = ON;   
    else if((p1 == 0) && (p2 == 0))   
        ret_val.status = OFF;   
    else   
        ret_val.status = INACTIVE;   
   
    return ret_val;   
}   
   
//////////////////////////////////////////////////////////////////////////   
//   
// Displays algebraic equivalent of the decision tree   
//   
//////////////////////////////////////////////////////////////////////////   
void ReliefF::print_tree(NODE* node, CHAR** tag_names)   
{   
    CString tree;   
   
    if (NULL == node->on)   
    {   
        if (ON == node->idx)   
        {   
            printf(/*L*/"ON");   
        }   
        else if(OFF == node->idx)   
        {   
            printf(/*L*/"OFF");   
        }   
        return ;   
    }   
    else   
    {   
        tree.Format(/*L*/"if { %s >= %1.2f then ", tag_names[node->idx], node->threshold);   
        printf(tree);   
    }   
   
    print_tree(node->on, tag_names);   
    printf(" else ");   
    print_tree(node->off, tag_names);   
    printf(" } ");   
}   
   
//////////////////////////////////////////////////////////////////////////   
//   
// 函数名: fscanf   
// 功 能: 从一个流中执行格式化输入   
//   
//////////////////////////////////////////////////////////////////////////   
void ReliefF::read_matrix(CHAR filename[], MATRIX* matrix)   
{   
    UINT i, j;   
    UINT height = matrix->height;   
    UINT width = matrix->width;   
    REAL **data = matrix->data;   
    FILE *f;   
   
    // Open price file   
    if (NULL == (f = fopen(filename, "r")))   
    {   
        printf("\n File not found: %s\n", filename);   
        err_exit("error", 0);   
    }   
   
    for (i = 0; i < height; i++)   
        for (j = 0; j < width; j++)   
        {   
            fscanf(f, "%1f\n", &data[i][j]);   
        }   
   
        fclose(f);   
}   
   
//////////////////////////////////////////////////////////////////////////   
//   
// sscanf() - 从一个字符串中读进与指定格式相符的数据   
//   
//////////////////////////////////////////////////////////////////////////   
CHAR** ReliefF::read_tags(CHAR* tag_file, UINT width)   
{   
    FILE *f;   
    CHAR **_varname;   
    UINT i;   
    CHAR buffer[0xFF];   
   
    f = fopen(tag_file, "r");   
    if (NULL == f)   
    {   
        printf("\n File not found : %s\n", tag_file);   
        err_exit("Test", 0);   
    }   
   
    _varname = (CHAR**)malloc(width * sizeof(CHAR*));   
    for (i = 0; i < width; i++)   
        _varname[i] = (CHAR*)malloc(0xFF * sizeof(CHAR));   
   
    i = 0;   
    while (!feof(f))   
    {   
        if (NULL == fgets(buffer, 0xFF, f))   
        {   
            if (strlen(buffer) > strlen("\n"))   
            {   
                if (i > width-1)   
                {   
                    printf("\nMore variable names were detected than data items.");   
                    printf("\nPlease correct this problem before proceeding");   
                    exit(0);   
                }   
                sscanf(buffer, "%[a-zA-Z0-9-_;:!@#$%^&*(){}[]]", _varname[i]);   
                i++;   
            }   
        }   
    }   
   
    if (i < width)   
    {   
        printf("\nFewer variable names than data items were detected.");   
        printf("\nPlease correct this problem before proceeding");   
        exit(0);   
    }   
   
    fclose(f);   
   
    return _varname;   
}   
   
void ReliefF::FreeMatrix(MATRIX* _matrix)   
{   
    UINT i;   
    for (i = 0; i < _matrix->height; i++)   
        free(_matrix->data[i]);   
   
    free(_matrix->data);   
    free(_matrix);   
}   
   
void ReliefF::free_tags(CHAR** varname, UINT width)   
{   
    UINT i;   
    for (i = 0; i < width; i++)   
    {   
        free(varname[i]);   
    }   
    free(varname);   
}   
   
//////////////////////////////////////////////////////////////////////////   
//   
// Frees the memory allocated to a tree structure   
//   
//////////////////////////////////////////////////////////////////////////   
void ReliefF::free_tree(NODE* node)   
{   
    if (NULL == node)   
    {   
        return ;   
    }   
    else   
    {   
        free_tree(node->on);   
        free_tree(node->off);   
        free(node);   
    }   
}   
   
void ReliefF::bubble(Mysort *buff, int len)   
{   
    int i, j;   
    REAL temp;   
    int tempindex;   
    for (i = len; i > 0; i--)   
    {   
        for(j = len-1; j > 0; j--)   
        {   
            if (buff[j].genedata > buff[j-1].genedata)   
            {   
                temp = buff[j].genedata;   
                tempindex = buff[j].index;   
                buff[j].genedata = buff[j-1].genedata;   
                buff[j].index = buff[i-1].index;   
                buff[j-1].genedata = temp;   
                buff[j-1].index = tempindex;   
            }   
        }   
    }   
   
    FILE *m_fp1;   
    m_fp1 = fopen("c:\\lihui.txt", "w+");   
   
    fprintf(m_fp1, "基因个数=%d\n", len);   
    for (i = len-1; i > 0; i--)   
    {   
        fprintf(m_fp1, "%d", buff[i].index+1);   
        fprintf(m_fp1, " ");   
        fprintf(m_fp1, "%f", buff[i].genedata);   
        fprintf(m_fp1, "\n");   
    }   
   
    fclose(m_fp1);   
}   
   
void ReliefF::Relief(MATRIX *matrix, UINT x, UINT y, UINT target, UINT K)   
{   
    // 特征数   
    UINT n_vars = matrix->width;   
    // 样本数   
    UINT n_samples = matrix->height;   
    UINT i, j;   
   
    REAL** data = matrix->data;   
   
    REAL* weight = (REAL*)malloc(n_vars * sizeof(REAL));   
   
    // 每个特征的权重初始化为0(第一步)   
    for (i = 0; i < n_vars; i++)   
    {   
        weight[i] = 0.0000;   
    }   
   
    progress2->SetRange(0, n_vars * n_samples);   
   
    int k1 = 0;   
    int m = 0;   
    int e = 0;   
    REAL temp1 = 0.00;   
    REAL temp2 = 0.00;   
   
    REAL** temphit1 = (REAL**)malloc(n_vars * sizeof(REAL*));   
    if (temphit1 == NULL)   
    {   
        printf(IDS_ERROR_INIT);   
    }   
    for(i = 0; i < n_vars; i++)   
    {   
        temphit1[i] = (REAL*)malloc(n_samples * sizeof(REAL));   
        if (temphit1 == NULL)   
        {   
            printf(IDS_ERROR_INIT);   
        }   
    }   
   
    REAL** temphit2 = (REAL**)malloc(n_vars * sizeof(REAL*));   
    if (temphit2 == NULL)   
    {   
        printf(IDS_ERROR_INIT);   
    }   
    for (i = 0; i < n_vars; i++)   
    {   
        temphit2[i] = (REAL*)malloc(n_samples * sizeof(REAL));   
        if (temphit2 == NULL)   
        {   
            printf(IDS_ERROR_INIT);   
        }   
    }   
    REAL a, b, a1, b1;   
    for (j = 0; j < n_samples; j++)   
        for (i = 0; i < n_vars; i++)   
        {   
            temphit1[i][j] = 0.0;   
            temphit2[i][j] = 0.0;   
        }   
               
   
    for (k1 = 0; k1 < n_samples; k1++)   
    {   
        for (j = 0; j < n_samples; j++)   
        {   
            for (i = 0; i < n_vars; i++)   
            {   
                //if (i != target)   
                //{   
                    a1 = data[i][k1];   
                    b1 = data[i][j];   
                    progress->StepIt();   
                    REAL temp = (a1 - b1) * (a1 - b1);   
                    if (data[target][j] == data[target][k1])   
                    {   
                        temphit1[i][k1] = temphit1[i][k1] + temp;   
                    }   
                    else   
                    {   
                        temphit2[i][k1] = temphit2[i][k1] + temp;   
                    }   
                //}   
            }   
        }   
    }   
   
    for (k1 = 0; k1 < n_samples; k1++)   
    {   
        for (m = 0; m < n_vars-1; m++)   
        {   
            for(e = 0; e < K; e++)   
            {   
                temp1 += (temphit1[m][e] - data[m][k1]) * (temphit1[m][e] - data[m][k1]);   
                temp2 += (temphit2[m][e] - data[m][k1]) * (temphit2[m][e] - data[m][k1]);   
            }   
            temp1 /= K;   
            temp2 /= K;   
            REAL temp = weight[m] - temp1 + temp2;   
            weight[m] = temp;   
            temp1 = 0;   
            temp2 = 0;   
        }   
    }   
   
    FILE *fp;   
    fp = fopen("C:\\fancy_relief.txt", "w+");   
   
    fprintf(fp, "基因个数=%d\n", n_vars);   
    for (i = 0; i < n_vars; i++)   
    {   
        REAL temp = weight[i];   
        fprintf(fp, "%f", weight[i]);   
        fprintf(fp, " ");   
        fprintf(fp, "\n");   
    }   
   
    fclose(fp);   
}   
   
MATRIX* ReliefF::ReadFromDataset(CString filename, UINT width, UINT height)   
{   
    MATRIX *_matrix;   
    UINT nwidth = 0, nheight = 0, i;   
   
    _matrix = (MATRIX*) malloc (sizeof (MATRIX));   
    if (!_matrix)   
        printf(IDS_ERROR_INIT);   
   
    _matrix->width  = width;   
    _matrix->height = height;   
   
    _matrix->data = (REAL**) malloc (height * sizeof (REAL*));   
    if (NULL == _matrix->data)   
        printf(IDS_ERROR_INIT);   
   
    for (i = 0; i < height; i++)   
    {   
        _matrix->data[i] = (REAL*)malloc(width * sizeof(REAL));   
        if (NULL == _matrix->data[i])   
            printf(IDS_ERROR_INIT);   
    }   
   
    FILE *file;   
    UINT buf_size = 0xFFFF;   
    CHAR *buffer, *ptr;   
   
    buffer = (CHAR*)malloc(buf_size * sizeof(CHAR));   
    if(NULL == buffer)   
        printf(IDS_ERROR_INIT);   
   
    // Open price file - abort if filename invalid   
    char *temp = filename.GetBuffer(filename.GetLength());   
    file = fopen(temp, "r");   
    if(NULL == file)   
    {   
        printf(IDS_ERROR_FILE, filename);   
        err_exit(temp, 0);   
    }   
   
    CString real, imaginary;   
    while (!feof(file))   
    {   
        if (NULL != fgets(buffer, buf_size, file))   
        {   
            // if line is more than a NL char   
            if (strlen(buffer) > strlen("\n"))   
            {      
                nwidth = 0;   
                ptr = strtok(buffer, " e");   
                while (NULL != ptr)   
                {   
                    real = ptr;   
                    ptr = strtok(NULL, " e");   
                    if (NULL != ptr)   
                    {   
                        imaginary = ptr;   
                    }   
                    REAL test = CStringToInt(real) * pow(10, CStringToInt(imaginary));   
                    _matrix->data[nheight][nwidth] = test;   
                    ++nwidth;   
                    ptr = strtok(NULL, " e");   
                }   
   
                if (width != nwidth)   
                {   
                    printf("width != nwidth");   
                }   
   
                ++nheight;   
            }   
        }   
    }   
    free(buffer);   
   
    FILE *fp;   
    fp = fopen("C:\\fancy_data.txt", "w+");   
   
    fprintf(fp, "基因个数=%d\n", width);   
    for (int j = 0; j < height; j++)   
        for (int i = 0; i < width; i++)   
        {   
            fprintf(fp, "%f", _matrix->data[j][i]);   
            fprintf(fp, " ");   
            fprintf(fp, "\n");   
        }   
   
    fclose(fp);   
   
    return _matrix;   
}   
   
double ReliefF::CStringToInt(CString str)      
{      
    char *m_str;      
    int nSize = 2 * str.GetLength();        
    char *pAnsiString = new char[nSize+1];        
    wcstombs(pAnsiString, str.AllocSysString(), nSize+1);        
    double x = strtod(pAnsiString, &m_str);      
    delete[] pAnsiString;     
    return x;      
}      
   
void ReliefF::Master(MATRIX *matrix, UINT K)   
{   
    UINT nFeatureNum = matrix->width;   
    UINT nSampleNum = matrix->height;   
    REAL **data = matrix->data;   
    REAL *weight = (REAL*)malloc(nFeatureNum * sizeof(REAL));   
    UINT i, j, l, y;   
   
    progress2->SetRange(0, nFeatureNum * nSampleNum);   
   
    // 每个特征的权重初始化为0(第一步)   
    for (i = 0; i < nFeatureNum; i++)   
    {   
        weight[i] = 0.0000;   
    }   
   
    for (l = 0; l < nSampleNum; l++)   
    {   
        int k = rand() % nSampleNum;   
        REAL y_k = data[l][k];   
        for (y = 0; y < nSampleNum; y++)   
        {   
            for (i = 0; i < nFeatureNum; i++)   
            {   
                for (j = 0; j < K; j++)   
                {   
                }   
            }   
        }   
    }   
   
    //  for (k1 = 0; k1 < n_samples; k1++)   
    //  {   
    //      for (m = 0; m < n_vars-1; m++)   
    //      {   
    //          for(e = 0; e < K; e++)   
    //          {   
    //              temp1 += (temphit1[m][e] - data[m][k1]) * (temphit1[m][e] - data[m][k1]);   
    //              temp2 += (temphit2[m][e] - data[m][k1]) * (temphit2[m][e] - data[m][k1]);   
    //          }   
    //          temp1 /= K;   
    //          temp2 /= K;   
    //          REAL temp = weight[m] - temp1 + temp2;   
    //          weight[m] = temp;   
    //          temp1 = 0;   
    //          temp2 = 0;   
    //      }   
    //  }   
   
        FILE *fp;   
        fp = fopen("C:\\fancy_relief.txt", "w+");   
   
        fprintf(fp, "基因个数=%d\n", nFeatureNum);   
        for (i = 0; i < nFeatureNum; i++)   
        {   
            fprintf(fp, "%f", weight[i]);   
            fprintf(fp, " ");   
            fprintf(fp, "\n");   
        }   
   
        fclose(fp);   
}  


