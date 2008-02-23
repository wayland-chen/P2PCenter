/*
	Copyright (c) 2004-2005 Cory Nelson

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "stdafx.h"
#include "resource.h"

using std::string;
using std::pair;
using libtorrent::entry;

typedef entry::dictionary_type dict_t;

Configuration::Configuration() :
	uplimit(-1),downlimit(-1),firstport(6881),lastport(6889),maxup(3),
	maxcon(200),torrentmaxcon(40) {
	char folder[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, folder);

	boost::filesystem::path::default_name_check(boost::filesystem::native);

	this->savepath=folder;
	this->savepath/=loadstringa(IDS_MYDOWNLOADS);

	this->columns[0]=256;
	this->columns[1]=64;
	this->columns[2]=88;
	this->columns[3]=88;
	this->columns[4]=96;
	this->columns[5]=64;
	this->columns[6]=88;
	this->columns[7]=88;
	this->columns[8]=64;
	this->columns[9]=48;
	this->columns[10]=48;

	this->position.top=0;
	this->position.left=0;
	this->position.bottom=0;
	this->position.right=0;
}

static bool getopt(const dict_t &root, const char *key, int &i) {
	dict_t::const_iterator iter=root.find(key);
	if(iter!=root.end()) {
		i=(int)iter->second.integer();
		return true;
	}
	else return false;
}

static bool getopt(const dict_t &root, const char *key, long &i) {
	dict_t::const_iterator iter=root.find(key);
	if(iter!=root.end()) {
		i=(long)iter->second.integer();
		return true;
	}
	else return false;
}

bool Configuration::Load() {
	try {
		entry config=bdecode(getmodulepath()/"arctic.conf");
		const dict_t &config_d=config.dict();

		dict_t::const_iterator windowing=config_d.find("windowing");
		if(windowing!=config_d.end()) {
			const dict_t &windowing_d=windowing->second.dict();

			dict_t::const_iterator position=windowing_d.find("position");
			if(position!=windowing_d.end()) {
				const dict_t &position_d=position->second.dict();

				getopt(position_d, "top", this->position.top);
				getopt(position_d, "left", this->position.left);
				getopt(position_d, "bottom", this->position.bottom);
				getopt(position_d, "right", this->position.right);
			}

			dict_t::const_iterator columns=windowing_d.find("columns");
			if(columns!=windowing_d.end()) {
				const dict_t &columns_d=columns->second.dict();

				getopt(columns_d, "name", this->columns[0]);
				getopt(columns_d, "size", this->columns[1]);
				getopt(columns_d, "downloaded", this->columns[2]);
				getopt(columns_d, "uploaded", this->columns[3]);
				getopt(columns_d, "status", this->columns[4]);
				getopt(columns_d, "progress", this->columns[5]);
				getopt(columns_d, "down", this->columns[6]);
				getopt(columns_d, "up", this->columns[7]);
				getopt(columns_d, "health", this->columns[8]);
				getopt(columns_d, "seeds", this->columns[9]);
				getopt(columns_d, "peers", this->columns[10]);
			}
		}

		dict_t::const_iterator network=config_d.find("network");
		if(network!=config_d.end()) {
			const dict_t &network_d=network->second.dict();

			getopt(network_d, "uplimit", this->uplimit);
			getopt(network_d, "downlimit", this->downlimit);
			getopt(network_d, "firstport", this->firstport);
			getopt(network_d, "lastport", this->lastport);
		}

		dict_t::const_iterator filesystem=config_d.find("filesystem");
		if(filesystem!=config_d.end()) {
			const dict_t &filesystem_d=filesystem->second.dict();

			dict_t::const_iterator savepath=filesystem_d.find("savepath");
			if(savepath!=filesystem_d.end())
				this->savepath=savepath->second.string();
		}

		if(this->uplimit>-1 && this->uplimit<12) this->downlimit=this->uplimit*5;
	}
	catch(...) {
		return false;
	}

	return true;
}

bool Configuration::Save() {
	entry::dictionary_type config;

	{
		entry::dictionary_type windowing;

		{
			entry::dictionary_type position;
			position["top"]=this->position.top;
			position["left"]=this->position.left;
			position["bottom"]=this->position.bottom;
			position["right"]=this->position.right;

			windowing["position"]=position;
		}

		{
			entry::dictionary_type columns;
			columns["name"]=this->columns[0];
			columns["size"]=this->columns[1];
			columns["downloaded"]=this->columns[2];
			columns["uploaded"]=this->columns[3];
			columns["status"]=this->columns[4];
			columns["progress"]=this->columns[5];
			columns["down"]=this->columns[6];
			columns["up"]=this->columns[7];
			columns["health"]=this->columns[8];
			columns["seeds"]=this->columns[9];
			columns["peers"]=this->columns[10];

			windowing["columns"]=columns;
		}

		config["windowing"]=windowing;
	}

	{
		entry::dictionary_type network;
		network["uplimit"]=this->uplimit;
		network["downlimit"]=this->downlimit;
		network["firstport"]=this->firstport;
		network["lastport"]=this->lastport;
		
		config["network"]=network;
	}

	{
		entry::dictionary_type filesystem;
		filesystem["savepath"]=this->savepath.native_directory_string();
		
		config["filesystem"]=filesystem;
	}

	return bencode(getmodulepath()/"arctic.conf", config);
}
