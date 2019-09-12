#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/stat.h> 

char * FILE_NAME = "./read_file.txt";    
char * FILE_NAME_1 = "./write_file.txt";    

#define true 1
#define false 0

/******* 定义相关的配置 *********/

static char word_root_sort = false;   //显示共用部分
static char all_etyma_word = false;   ///显示第二个文件内容
static char filter_count = 1;         //拆分单词，过滤重复组合的数量
static char min_letter_num = 0;       //最少字母组合的个数
/********************************/

#define MAXLEN 100
 
typedef struct TreeNode
{
    int count;
    char word[MAXLEN];
	struct TreeNode *left;
	struct TreeNode *right;
}TreeNode;

void insert(struct TreeNode **root,char* word)
{
	struct TreeNode *hroot = *root;
	
	if(hroot == NULL)
	{
		hroot = (struct TreeNode*)malloc(sizeof(struct TreeNode));
 
		hroot->count = 1;
		strcpy(hroot->word,word);
 
		hroot->left = NULL;
		hroot->right = NULL;
 
		*root = hroot;
		
	}else{
		struct TreeNode *temp = hroot;
 
		while(1)
		{
			if(strcmp(temp->word,word) != 0)
			{
				if(temp->right == NULL)
					break;
				temp = temp->right;
			}else
				break;
		}
		
		if(strcmp(temp->word,word) == 0)
		{
			temp->count++;
		}
		else
		{
			struct TreeNode *newtree=(struct TreeNode*)malloc(sizeof(struct TreeNode));
 
			newtree->count = 1;
			strcpy(newtree->word,word);
 #if 1
	#if 1
			temp->right = newtree;
			newtree->left = temp;
			newtree->right = NULL;
	#else
			newtree->right = temp->right;
			newtree->left = temp;
			
			temp->right = newtree;
			temp->right->left = newtree;
	#endif
#else			
			newtree->right = temp;
			newtree->left = temp->left;
			temp->left->right = newtree;
			temp->left = newtree;
			
#endif		
		}
	}
}


void print(struct TreeNode *root,struct TreeNode *obj)
{
    struct TreeNode *temp_r,*temp_o;
	int i=0;
	if(!all_etyma_word){
		for (temp_r = root; temp_r != NULL; temp_r = temp_r->right)
		{
			if(temp_r->count >= filter_count){
				if(word_root_sort){
					/*200词根在位置中出现的次数排名*/
					for (temp_o = obj; temp_o != NULL; temp_o = temp_o->right)
					{
						if(strcmp(temp_o->word,temp_r->word) == 0){
							printf("word = %-5s, %d:%d\n",temp_r->word,temp_r->count,temp_o->count);
							break;
						}
					}
				}else{
					printf("word = %-5s, %d\n",temp_r->word,temp_r->count);
				}
			}
		}
	}else{
		for (temp_r = obj; temp_r != NULL; temp_r = temp_r->right)
		{
			if(temp_r->count >= filter_count){
				if(word_root_sort){
					printf("**** etyma = %s ****\n",temp_r->word);
					/*200词根在位置中出现的次数排名*/
					for (temp_o = root; temp_o != NULL; temp_o = temp_o->right)
					{
						if(strstr(temp_o->word,temp_r->word) != NULL){       //参1为长，参2为短
							printf("\t%s\n",temp_o->word);
						}
					}
					printf("\n");
				}else{
					printf("word = %-5s, %d\n",temp_r->word,temp_r->count);
				}
			}
		}
	}
}

void swap(struct TreeNode *H,struct TreeNode *p,struct TreeNode *t)  
{
	struct TreeNode *temp;
	if(t->right==NULL) //t结点是否为尾结点
	{
		if(p->right==t) //p,t结点是否相邻
		{
			//与尾结点相邻的交换代 
			t->right=p;
			t->left=p->left;
			p->right=NULL;
			p->left->right=t;
			p->left=t;
		}
		else
		{
			//与尾结点不相邻的交换代 
			t->right=p->right;
			t->left->right=p;
			temp=t->left;
			t->left=p->left;
			p->right->left=t;
			p->right=NULL;
			p->left->right=t;
			p->left=temp;
		}
	}
	else
	{
		if(p->right==t) //p,t结点是否相邻
		{
			//相邻的交换代 
			t->right->left=p;
			temp=t->right;
			t->right=p;
			t->left=p->left;
			p->right=temp;
			p->left->right=t;
			p->left=t;
		}
		else
		{
			//不相邻的交换代 
			t->right->left=p;
			temp=t->right;
			t->right=p->right;
			p->right->left=t;
			p->right=temp;
			t->left->right=p;
			temp=t->left;
			t->left=p->left;
			p->left->right=t;
			p->left=temp;
		}
	}
}
 
void sort(struct TreeNode * H)  //选择排序算法
{
	struct TreeNode *i,*j,*k;
	if(!H->right)  //双向链表为空则返回
	return;
	for(i=H->right;i->right!=NULL;i=k->right)  //i=k->right是关键,k记录了双向链表交换指针后的下一个i的值(地址)
	{
		for(j=i->right,k=i;j!=NULL;j=j->right)
			if(k->count > j->count)
				k=j;
		if(k!=i)
			swap(H,i,k);
	}
}


//过滤只剩下字母
int letter_filter(char *buff)
{
	char str[50];
	int i = 0;
	int flag_on = 0;
	int ret =0 ;
	char *buf = buff;
	
	if(strlen(buf) < 2)
		return -1;
	
	while(i < (strlen(buf)))
	{
		if(!((buf[i] >= 'A' && buf[i] <= 'Z') || (buf[i] >= 'a' && buf[i] <= 'z'))){
			buf[i]='\0';
			if(!flag_on)
				ret = -1;
			break;
		}
		i++;
		if(i == 2)
			flag_on=1;
	}
	
	return ret;
}

void count_words(FILE * file_wfd,struct TreeNode **root)
{
	
	int read_len;  
	char *p;

	char i,j;
	char *p_bak;
	char p_temp[20];
	unsigned char file_buffer[100*100]={0};  
	struct TreeNode **lxmroot = root;

	while(fgets(file_buffer,sizeof(file_buffer),file_wfd)!= NULL)   //以行读取
	{       
		p = strtok(file_buffer, " ");    //以" "为分割符来切开每个单词,p指向每个单词的首字母
		while(p) 
		{
			if(letter_filter(p) == 0){
				p_bak=p;
				if(min_letter_num){                //拆分单词，将所有组合写入文件
					for(i=0;i<strlen(p)-1;i++){                      //单词的位置移动
						for(j=min_letter_num;j <= strlen(p_bak);j++){       //写到文件的单词字母大小变化
							memset(p_temp, 0, sizeof(p_temp));
							strncpy(p_temp,p_bak,j);
							insert(lxmroot,p_temp);
						}
						p_bak++;
					}
				}else{
					insert(lxmroot,p_bak);
				}
			}
			p = strtok(NULL, " ,\n"); 
		}
		memset(file_buffer, 0, sizeof(file_buffer));
	}  
	sort(*lxmroot);
}
     
        
		
static int param_parse(int argc, char *argv[])
{
	int i;
	
	for(i=1; i < argc; i++)
	{
		if(strncmp(argv[i],"-h",2) == 0){
			printf("usage:  %s [subcommand] [options]\
					\n\t%s <-h>  		[help ] \
					\n\t%s <-c>  <num>	[count filter word ] \
					\n\t%s <-n>  <num>	[num min letter,split word ]\
					\n\
					\n\t%s <--ws>  [--w Displays the contents of the second file]\
					\n\t\t	[--s display Shared parts] \
					\n",argv[0],argv[0],argv[0],argv[0],argv[0]);
			
			return -1;
		
		}else if(strncmp(argv[i],"-c",2) == 0){  //过滤重复组合的数量
			if(argv[i+1] == NULL){
				printf("[ERROR]\tcmd %s \n\tusage:  %s  <-h>  		[help ]  \n",argv[i],argv[0]);
				return -1;
			}
			filter_count = atoi(argv[i+1]);      
			
		}else if(strncmp(argv[i],"-n",2) == 0){   //拆分单词，过滤重复组合的数量
			if(argv[i+1] == NULL){
				printf("[ERROR]\tcmd %s \n\tusage:  %s  <-h>  		[help ]  \n",argv[i],argv[0]);
				return -1;
			}
			min_letter_num = atoi(argv[i+1]);    
			
		}else if(strncmp(argv[i],"--",2) == 0){
			if(strstr(argv[i],"w") != NULL){
				all_etyma_word = true;           //显示第二个文件内容
			}
			
			if(strstr(argv[i],"s") != NULL){     //显示共用部分
				word_root_sort = true;            
			}
		}
	}
	
	return 0;
}

int main(int argc, char *argv[])  
{  
	FILE * file_rfd;  
	FILE * file_wfd; 
	struct TreeNode *lxmroot = NULL;
	struct TreeNode *lxm_obj = NULL;
	
	if(param_parse(argc,argv) == -1){
		return 0;
	}

	file_rfd = fopen(FILE_NAME,"rb");        //从该文件中读取内容  
	file_wfd = fopen(FILE_NAME_1,"rb");      //将结果写到该文件中

	if(file_rfd == NULL||file_wfd==NULL)  
	{  
	    if(file_rfd == NULL)
		    printf("[ERROR] file: %s not found or IsEmpty\n",FILE_NAME);  
	    if(file_wfd == NULL)
		    printf("[ERROR] file: %s not found or IsEmpty\n",FILE_NAME_1);  
		    
		return 0;
	} 
	else   
	{  
		printf("File Open successed!\n");  
	}  

	count_words(file_rfd,&lxmroot); 
	count_words(file_wfd,&lxm_obj);
	
	print(lxmroot,lxm_obj);

	fclose(file_rfd);  
	fclose(file_wfd);  
	return 0;  
}