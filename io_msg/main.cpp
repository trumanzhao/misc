#include <stdio.h>
#include <time.h>  
#include <assert.h>
#include "io_msg.h"

struct item 
{
	int cate;
	int idx;
	int count;
	char name[32];

	void print() const { printf("item:%d,%d,%d,%s\n", cate, idx, count, name); }

	MEMBER_IO(cate, idx, count, name)
};

void on_login(int k, std::map<std::string, std::string> m, const item& it)
{
	printf("k=%d\n", k);
	for (auto& node : m)
	{
		printf("%s:%s\n", node.first.c_str(), node.second.c_str());
	}
	it.print();
}

int main(int argc, char **argv)
{
	msg_archiver saver;	
	std::map<std::string, std::string> m = { {"a", "1"}, {"ab", "12"}, {"abc", "123"} };
	item my_item = {1,2,3,"xx"};

	size_t data_len= 0;
	const char* data = saver.save(data_len, "on_login", 9527, m, my_item);
	if (data)
	{
		msg_dispatcher<const char*> loader;	
		loader.bind("on_login", on_login);		
		if (!loader.dispatch(data, data_len))
			puts("load error !");
	}

	return 0;
}
