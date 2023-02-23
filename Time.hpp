#include <time.h> 
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include<ctime>
#include<stdint.h>
class Time{
	public:
	 std::string myTime;
	 std::map<std::string,int> monthToNumber;
	 std::map<std::string,int> dayToNumber;
	public:
	Time(std::string myTime){
		this->myTime=myTime;
		monthToNumber.insert(std::pair<std::string,int>("Jan",1));
		monthToNumber.insert(std::pair<std::string,int>("Feb",2));
		monthToNumber.insert(std::pair<std::string,int>("Mar",3));
		monthToNumber.insert(std::pair<std::string,int>("Apr",4));
		monthToNumber.insert(std::pair<std::string,int>("May",5));
		monthToNumber.insert(std::pair<std::string,int>("Jun",6));
		monthToNumber.insert(std::pair<std::string,int>("Jul",7));
		monthToNumber.insert(std::pair<std::string,int>("Aug",8));
		monthToNumber.insert(std::pair<std::string,int>("Sep",9));
		monthToNumber.insert(std::pair<std::string,int>("Oct",10));
		monthToNumber.insert(std::pair<std::string,int>("Nov",11));
		monthToNumber.insert(std::pair<std::string,int>("DEc",12));
		dayToNumber.insert(std::pair<std::string,int>("Mon",1));
		dayToNumber.insert(std::pair<std::string,int>("Tue",2));
		dayToNumber.insert(std::pair<std::string,int>("Wed",3));
		dayToNumber.insert(std::pair<std::string,int>("Thu",4));
		dayToNumber.insert(std::pair<std::string,int>("Fri",5));
		dayToNumber.insert(std::pair<std::string,int>("Sat",6));
		dayToNumber.insert(std::pair<std::string,int>("Sun",0));
	}
	struct tm get_time_struct(std::string eTime){
		int weekday_start=eTime.find_first_not_of(" ");
		int weekday_end=eTime.find_first_of(",");
		struct tm given_time;
		std::string weekday=eTime.substr(weekday_start,weekday_end-weekday_start);
		int wDay=dayToNumber.find(weekday)->second;
		given_time.tm_wday=wDay;
	    std::vector<std::string> time_info;
	    std::string left=eTime.substr(weekday_end+1,eTime.length()-weekday_end-1);
	    std::string sum=std::string("");
	    for(int i=0;i<left.length();i++){
	    	std::string thispos=left.substr(i,1);
	    	if(thispos!=" "||thispos!=":"){
	    		sum+=thispos;
			}else{
				if(!sum.empty()){
					time_info.push_back(sum);
				}
				sum="";
			}
		}
		for(int j=0;j<time_info.size();j++){
			if(j==0){
				std::string day=time_info[0];
				given_time.tm_mday=atoi(day.c_str());
			}else if(j==1){
				std::string month=time_info[1];
				int monthNum=this->monthToNumber.find(month)->second;
				given_time.tm_mon=monthNum-1;
			}else if(j==2){
				std::string year=time_info[2];
				given_time.tm_year=atoi(year.c_str())-1900;
			}else if(j==3){
				std::string hour=time_info[3];
				given_time.tm_hour=atoi(hour.c_str());
			}else if(j==4){
				std::string minute=time_info[4];
				given_time.tm_min=atoi(minute.c_str());
			}else if(j==5){
				std::string second=time_info[5];
				given_time.tm_sec=atoi(second.c_str());
			}
		}
		given_time.tm_isdst=0;
		return given_time;
	}
	bool before(std::time_t now){
		struct tm expire_time=this->get_time_struct(this->myTime);
		time_t expire_timet=mktime(&expire_time);
		return now>expire_timet;
	}
};
