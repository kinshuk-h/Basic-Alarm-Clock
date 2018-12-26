#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
class Sound_Player
{
    size_t volume, startpos, c_song;
	std::string alias;
	static int aliascount;
	bool is_paused, is_stopped, is_playing;
	//std::vector<Song> songs;
public:
	enum class Mode : unsigned short { Immediate = 0, Repeat };
	Sound_Player(std::string name="")
	{
		alias = "mp3_"+std::to_string(aliascount);
		int ret = 0;
		if(!name.empty()) ret = mciSendString(("open \""+name+"\" type mpegvideo alias "+alias).c_str(),nullptr,0,nullptr);
		is_stopped = true; is_playing = false; is_paused = false;
		mciSendString(("setaudio "+alias+" volume to 1000").c_str(),nullptr,0,nullptr);
		char Desc[1024]; mciGetErrorString(ret,Desc,1024); std::cout<<"Error Code "<<ret<<" : "<<Desc<<std::endl;
	}
	~Sound_Player()
	{
	    mciSendString(("close "+alias).c_str(),nullptr,0,nullptr);
    }
	void play(Mode playm = Mode::Immediate, size_t startpos=0)
	{
	    std::string pmde = " wait";
	    if(playm == Mode::Immediate) pmde = "";
	    if(playm == Mode::Repeat) pmde += " repeat";
	    if(startpos>0) pmde+=" from "+std::to_string(startpos);
		mciSendString(("play "+alias+pmde).c_str(),nullptr,0,nullptr);
		is_playing = true; is_stopped = false; is_paused = false;
	}
	void pause()
	{
		if(!is_paused && !is_stopped) { mciSendString(("stop "+alias).c_str(),nullptr,0,nullptr);
		is_paused = true; is_playing = false; }
	}
	void stop()
	{
		if(!is_stopped) { mciSendString(("stop "+alias).c_str(),nullptr,0,nullptr); is_stopped = true; }
	}
    void set_volume(size_t vol = 1000)
    {
        mciSendString(("setaudio "+alias+" volume to "+std::to_string(vol)).c_str(),nullptr,0,nullptr);
    }
};

int Sound_Player::aliascount = 0;

int main(int argc, char** argv)
{
    Sound_Player P1(argv[1]); P1.set_volume(atoi(argv[2]));
    P1.play(Sound_Player::Mode::Immediate); while(true) {}
}
