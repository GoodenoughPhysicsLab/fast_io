﻿#include <fast_io_dsal/list.h>
#include <fast_io.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::list<::std::size_t> lst;
	for(::std::size_t i{};i!=100;++i)
	{
		lst.push_back(i);
	}
	for(auto const & e : lst)
	{
		println(e);
	}
}