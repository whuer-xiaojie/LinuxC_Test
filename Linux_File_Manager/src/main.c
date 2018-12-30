
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <pthread.h>
#include <dirent.h>  
#include <unistd.h> 

#include <sqlite3.h>
#include "keyboard.h"

#define XML_PATH "schemes"
#define DATE_BASE "downloadFile.db"
#define MAX_STRING_LEN 1024

const int major_version = 1;
const int minor_version = 1;

static char *progname = NULL;
static char *scheme_name = NULL;
static sqlite3 *Db_Handle=NULL;

static pthread_t autothread;
static int quit = 0;

static void usage(void)
{
	fprintf(stderr, "xStudio display test assistant\n");
	fprintf(stderr, "maybe you can run the program with [-h/help] to get help information\n");
}
static void print_error_info(void)
{
	fprintf(stderr, "Not support order type !\n");
	fprintf(stderr, "Please run the program :[%s -h or %s help] to get help information\n", progname,progname);
}
static void print_help_info(void)
{
	fprintf(stderr, "You can run the program with those order:\n");
	fprintf(stderr, "-h/help      get help information\n");
	fprintf(stderr, "-v/version   get the version of this program\n");
	fprintf(stderr, "-add/add     add all scheme in current directory to database\n");
	fprintf(stderr, "-c/clear     clear all schemes in database\n");
	fprintf(stderr, "-a/active 'scheme-name'  active the scheme that named 'scheme-name'\n");
	fprintf(stderr, "-i/insert 'scheme-name'  insert the scheme that named 'scheme-name' to database\n");
	fprintf(stderr, "-d/delete 'scheme-name'  delete the scheme that named 'scheme-name' from database\n");
	fprintf(stderr, "-auto/automation  active all the scheme automatically");
}
static void print_version_info(void)
{
	fprintf(stderr, "xStudio display test assistant\n");
	fprintf(stderr, "%s version:%d.%d\n", progname, major_version, minor_version);
}

static int file_type_is_xml(char *fileName)
{
	char *ptr; char c = '.';
	int start, end, lenth;
	ptr = strrchr(fileName, c);    /* get the last "." position */
	if (ptr == NULL)
		return -1;
	start = (ptr - fileName) + 1;    /* get the start index */
	end = strlen(fileName);
	lenth = end - start;
	char stbuf[256];
	strncpy(stbuf, fileName + start, lenth);
	stbuf[lenth] = '\0';
	if (strcasecmp(stbuf, "xml") == 0 || strcasecmp(stbuf, "XML") == 0)
		return 0;
	else
		return -1;
}
static int check_file_exist(char *filepath)
{
	if (access(filepath, R_OK) != -1)
		return 0;
	else
		return -1;
}
static int get_database_handle(void)
{
	char *database_path = DATE_BASE;
	int ret = 0;
	if (check_file_exist(database_path) != 0)
	{
		fprintf(stderr, "%s():database file:%s is not exist\n", __FUNCTION__, database_path);
		return -1;
	}
	ret = sqlite3_open(database_path, &Db_Handle);
	if (ret != 0)
	{
		fprintf(stderr, "%s():open database file:%s failed \n", __FUNCTION__, database_path);
		ret = -1;
		return ret;
	}
	ret = 0;
	return ret;
}
static int clear_database_table(char *table_name)
{
	int ret = -1; char *errMsg;
	char sql_order[MAX_STRING_LEN];
	sprintf(sql_order, "%s%s%s", "delete from ", table_name, ";");
	ret = sqlite3_exec(Db_Handle, sql_order, NULL, 0, &errMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "%s():operation database failed:%s\n", __FUNCTION__, errMsg);
		sqlite3_free(errMsg);
		return -1;
	}
	return 0;
}
static int delete_xml_from_database(char *xml_name)
{
	int ret = -1; char *errMsg;
	char sql_order[MAX_STRING_LEN];
	sprintf(sql_order, "%s%s%s", "delete from Playlists where Name='", xml_name, "';");
	ret = sqlite3_exec(Db_Handle, sql_order, NULL, 0, &errMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "%s():operation database failed:%s\n", __FUNCTION__, errMsg);
		sqlite3_free(errMsg);
		return -1;
	}
	return 0;
}
static int add_test_xml_to_database(char *xml_name,char *id)
{
	char sql_order[MAX_STRING_LEN];
	int ret; char *errMsg;

	char Pid[MAX_STRING_LEN]; char Name[MAX_STRING_LEN]; char Fin[MAX_STRING_LEN];
	char Path[MAX_STRING_LEN]; char Cur[MAX_STRING_LEN]; char Back[MAX_STRING_LEN];

	char *order = "insert into Playlists values(";

	sprintf(Pid, "%s%s%s", "'", id, "',");
	sprintf(Name, "%s%s%s", "'", xml_name, "',");
	sprintf(Fin, "%s", "'',");
	sprintf(Path, "%s", "'',");
	sprintf(Cur, "%s", "'3',");
	sprintf(Back, "%s", "'');");

	sprintf(sql_order, "%s%s%s%s%s%s%s", order, Pid, Name, Fin, Path, Cur, Back);
	fprintf(stderr, "%s():sql_order=%s\n", __FUNCTION__, sql_order);
	ret = sqlite3_exec(Db_Handle, sql_order, NULL, 0, &errMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "%s():database operation failed:%s\n", __FUNCTION__, errMsg);
		sqlite3_free(errMsg);
		return -1;
	}
	return 0;
}
static int check_xml_exist_in_database(char *xml_name)
{
	char sql_order[MAX_STRING_LEN];

	int ret; int sql_order_len;
	char *errMsg; 
	sqlite3_stmt *stmt = NULL;
	int count = 0;
	char *order = "select * from Playlists where Name='";
	sprintf(sql_order, "%s%s%s", order, xml_name, "';");
	sql_order_len = strlen(sql_order);

	if (sqlite3_prepare_v2(Db_Handle, sql_order, sql_order_len, &stmt, &errMsg) == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			++count;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (count == 1)
		return 0;
	else
		return -1;
}

static int change_current_show_scheme_status(void)
{
	int ret; char *errMsg;
	char *order = "update Playlists set  BCurrentShow = 3 where BCurrentShow = 1;";
	ret = sqlite3_exec(Db_Handle, order, NULL, 0, &errMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "%s():database operation failed:%s\n", __FUNCTION__, errMsg);
		sqlite3_free(errMsg);
		return -1;
	}
	return 0;
}
static int change_current_active_scheme_status(char *active_name)
{
	char sql_order[MAX_STRING_LEN]; char *errMsg; int ret;
	sprintf(sql_order, "%s%s%s", "update Playlists set  BCurrentShow = 1 where Name ='", active_name, "';");
	if (change_current_show_scheme_status() != 0)
	{
		return -1;
	}
	ret = sqlite3_exec(Db_Handle, sql_order, NULL, 0, &errMsg);
	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "%s():database operation failed:%s\n", __FUNCTION__, errMsg);
		sqlite3_free(errMsg);
		return -1;
	}
	return 0;
}

static int add_all_xmlfile_to_database(char *scheme_dir)
{
	DIR *dir;
    struct dirent *ptr;
	char base[MAX_STRING_LEN]; 
	char count = '1';
	if ((dir=opendir(scheme_dir)) == NULL)
    {
		fprintf(stderr, "%s():opendir failed:%s\n", __FUNCTION__, XML_PATH);
        exit(1);
	}
	while ((ptr=readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
           continue;
		else if (ptr->d_type == 8)    ///file
		{
			if (file_type_is_xml(ptr->d_name) == 0)
			{
				fprintf(stderr, "%s():file=%s\n", __FUNCTION__, ptr->d_name);
				add_test_xml_to_database(ptr->d_name,&count);
				count++;
			}
		}
   }
   closedir(dir);
   return 1;
}
static void active_all_scheme_onebyone()
{
	DIR *dir;
	struct dirent *ptr;
	char base[MAX_STRING_LEN];
	//char count = '1';
	if ((dir = opendir(XML_PATH)) == NULL)
	{
		fprintf(stderr, "%s():opendir failed:%s\n", __FUNCTION__, XML_PATH);
		exit(1);
	}
	while (!quit)
	{
		if ((ptr = readdir(dir)) == NULL)
		{
			quit = 1;
			break;
		}
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
			continue;
		else if (ptr->d_type == 8)    ///file
		{
			if (file_type_is_xml(ptr->d_name) == 0)
			{
				if (check_xml_exist_in_database(ptr->d_name) != 0)
				{
					//fprintf(stderr, "active scheme failed \n");
					//close_database();
					continue;
				}
				if (change_current_active_scheme_status(ptr->d_name) == 0)
				{
					fprintf(stderr, "active scheme success current showed scheme:%s\n", ptr->d_name);
					//close_database();
					sleep(30);
					continue;
				}
			}
		}
	}
	closedir(dir);
	return 1;
}
static void close_database(void)
{
	if (Db_Handle)
	{
		sqlite3_close(Db_Handle);
		Db_Handle = NULL;
	}
}

static void clear_all_scheme(void)
{
	char *scheme_table = "Playlists";
	if (get_database_handle() != 0)
	{
		fprintf(stderr, "clear all scheme failed\n");
		return;
	}
	if (clear_database_table(scheme_table) == 0)
		fprintf(stderr, "clear all scheme success\n");
	else
		fprintf(stderr, "clear all scheme failed\n");
	close_database();
	return;
}
static void add_all_scheme(void)
{
	char *scheme_table = "Playlists";
	if (get_database_handle() != 0)
	{
		fprintf(stderr, "add all schemes to database failed\n");
		return;
	}
	if (clear_database_table(scheme_table) != 0)
	{
		fprintf(stderr, "add all schemes to database failed\n");
		close_database();
		return;
	}
	add_all_xmlfile_to_database(XML_PATH);
	close_database();
	return;
}
static void active_scheme(char *scheme_name)
{
	if (get_database_handle() != 0)
	{		
		fprintf(stderr, "active scheme failed\n");
		return;
	}
	if (check_xml_exist_in_database(scheme_name) != 0)
	{
		fprintf(stderr, "active scheme failed \n");
		close_database();
		return;
	}
	if (change_current_active_scheme_status(scheme_name) != 0)
	{
		fprintf(stderr, "active scheme failed\n");
		close_database();
		return;
	}
	close_database();
	fprintf(stderr, "active scheme success\n");
	return;
}
static void insert_scheme(char *scheme_name)
{
	if (get_database_handle() != 0)
	{
		fprintf(stderr, "insert scheme failed\n");
		return;
	}
	if (check_xml_exist_in_database(scheme_name) == 0)
	{
		fprintf(stderr, "insert scheme success\n");
		close_database();
		return;
	}
	if (add_test_xml_to_database(scheme_name, scheme_name) != 0)
	{
		fprintf(stderr, "insert scheme failed\n");
		close_database();
		return;
	}
	fprintf(stderr, "insert scheme success\n");
	close_database();
	return;
}
static void delete_scheme(char *scheme_name)
{
	if (get_database_handle() != 0)
	{
		fprintf(stderr, "delete scheme failed\n");
		return;
	}
	if (check_xml_exist_in_database(scheme_name) != 0)
	{
		fprintf(stderr, "delete scheme success\n");
		close_database();
		return;
	}
	if (delete_xml_from_database(scheme_name) != 0)
	{
		fprintf(stderr, "delete scheme failed\n");
		close_database();
		return;
	}
	fprintf(stderr, "delete scheme success\n");
	close_database();
	return;
}

static void auto_active_scheme(void)
{
	char *scheme_table = "Playlists";
	if (get_database_handle() != 0)
	{
		fprintf(stderr, "add all schemes to database failed\n");
		return;
	}
	if (clear_database_table(scheme_table) != 0)
	{
		fprintf(stderr, "add all schemes to database failed\n");
		close_database();
		return;
	}
	add_all_xmlfile_to_database(XML_PATH);
	active_all_scheme_onebyone();
	close_database();
	return;
}

static void *get_keybord_info(void)
{
	char c;
	if (init_keyboard() != 0)
	{
		fprintf(stderr, "%s():initial keyboard failed!!\n", __FUNCTION__);
		return;
	}
	else
		fprintf(stderr, "Press Q to exit\n");
	while (!quit)
	{
		while ((c = __getch()) >= 0)
		{
			if (c == 'Q' || c == 'q')
			{
				quit = 1;
				//fprintf(stderr, "%s(): quit=1 \n", __FUNCTION__);
				break;
			}
		}
	}
	close_keyboard();
}
int main(int argc, char *argv[])
{
	char *p; 
	char *order; char *scheme_name;
	progname = argv[0];
	if ((p = strrchr(progname, '/')) != NULL)
		progname = p + 1;
	if (argc < 2 || argc > 3)
	{
		usage();
		return 0;
	}
	if (argc == 2)
	{
		order = argv[1];
		if (strcasecmp(order, "-v") == 0 || strcasecmp(order, "version") == 0)
		{
			print_version_info();
			return 0;
		}
		else if (strcasecmp(order, "-h") == 0 || strcasecmp(order, "help") == 0)
		{
			print_help_info();
			return 0;
		}
		else if (strcasecmp(order, "-add") == 0 || strcasecmp(order, "add") == 0)
		{
			add_all_scheme();
			return 0;
		}	
		else if (strcasecmp(order, "-c")==0 || strcasecmp(order, "clear") == 0)
		{
			clear_all_scheme();
			return 0;
		}
		else if (strcasecmp(order, "-auto") == 0 || strcasecmp(order, "automation") == 0)
		{
			pthread_create(&autothread, NULL, get_keybord_info, NULL);
			auto_active_scheme();
			pthread_join(autothread,NULL);
			return 0;
		}
		else
		{
			print_error_info();
			return 0;
		}
	}
	else if (argc == 3)
	{
		order = argv[1]; scheme_name = argv[2];
		if (file_type_is_xml(scheme_name) != 0)
		{
			print_error_info();
			return 0;
		}
		if (strcasecmp(order, "-a") == 0 || strcasecmp(order, "active") == 0)
		{
			active_scheme(scheme_name);
			return 0;
		}
		else if (strcasecmp(order, "-i") == 0 || strcasecmp(order, "insert") == 0)
		{
			insert_scheme(scheme_name);
			return 0;
		}
		else if (strcasecmp(order, "-d") == 0 || strcasecmp(order, "delete") == 0)
		{
			delete_scheme(scheme_name);
			return 0;
		}
		else
		{
			print_error_info();
			return 0;
		}
	}
	
}