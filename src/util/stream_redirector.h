// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <fstream>

using namespace std;


class StreamRedirector {
	private:
		std::streambuf* savedBuf_;
		std::ios& stream_;

	public:
		explicit StreamRedirector(std::ios& stream, std::streambuf* newBuf)
			: savedBuf_(stream.rdbuf()), stream_(stream)
		{
			stream_.rdbuf(newBuf);
		}

		~StreamRedirector()
		{
			stream_.rdbuf(savedBuf_);
		}
};
