////////////////////////////////////////////////////////////////////////// 
//  
// ReliefF.h 
// Create : Fancy 2009-5-1 17:05 
// Describe : ReliefF 算法的主程序，对特征进行分析、提取(头文件) 
// Version : 1.0 
// 
////////////////////////////////////////////////////////////////////////// 
 
#ifndef _RELIEFF_H_ 
#define _RELIEFF_H_ 
 
#include <math.h> 
 
#if _MSC_VER > 1000 
#pragma once 
#endif 

typedef unsigned short UINT;
typedef double REAL; 
typedef char CHAR; 
 
enum UINT1 { INACTIVE, OFF, ON }; 
 
#define LN_2 0.693147180559945309417 
#define  entropy(x) (x > 0 ? x * log(x) / LN_2 : 0.0) 
 
typedef struct node 
{ 
	UINT idx;// ID code for attribute 
	REAL threshold;// Numerical threshold for attribute test 
	struct node *on;// Address of "on" node 
	struct node *off;// Address of "off" node 
	struct node *parent;// Address of parent node 
} NODE; 
 
typedef struct ne_struct 
{ 
	REAL ne; 
	UINT status; 
} NEGENTROPY; 
 
typedef struct  mysort_struct 
{ 
	REAL genedata; 
	int index; 
} Mysort; 
 
typedef struct matrix  
{ 
	UINT width; 
	UINT height; 
	REAL **data; 
} MATRIX; 
 
class ReliefF 
{ 
public: 
	ReliefF(); 
	~ReliefF(); 
	 
	void bubble(Mysort *buff, int len); 
	NEGENTROPY negentropy(REAL** data, UINT n_samples, NODE* local, UINT target); 
	void print_tree(NODE* node, CHAR** tag_names); 
	void free_tree(NODE* node); 
	NODE* ID3(MATRIX *matrix, NODE *parent, UINT target, UINT state); 
	void err_exit(CHAR* file, UINT line); 
	MATRIX *build_matrix(UINT width, UINT height); 
	void read_matrix(CHAR filename[], MATRIX* matrix);	 
	CHAR **read_tags(CHAR*, UINT); 
	void free_tags(CHAR**, UINT); 
	double CStringToInt(CString str); 
 
	MATRIX* ReadFromDataset(CString filename, UINT width, UINT height); 
	void FreeMatrix(MATRIX* _matrix); 
	void GetFileSize(CHAR* fileName, UINT* width, UINT* height); 
	void Relief(MATRIX *matrix, UINT x, UINT y, UINT target, UINT K); 
	void Master(MATRIX *matrix, UINT K); 
 
private: 
	int normal; 
	int unnormal; 
	REAL *hit1; 
	REAL *hit; 
 
}; 
 
 
#endif 