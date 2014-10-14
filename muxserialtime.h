
/**
 *
 * Author: Domenico Monaco, domenico.monaco@kiuz.it - monaco.kiuz.it
 *
 * Description: This class manage aggregation of temporal data acquisition
 *
 * Version: 0.1.1
 *
 * Date: 17/07/2014
 * License: GPL v2
 * Copyright: Domenico Monaco
 *
 **/

#include "Statistic.h"
#include "Arduino.h"

#include <string>

class MuxSerialTime{
protected:
    
    static const int array = 3;
    static const int delayDiff = 5;
    
    String ValueLineDivider;
    
    bool syncTime;
    char* LastDateTime;
    char* zone;
    
    long int GroupMultiply;
    
    long int GroupsDelay[3];
    Statistic grouped[3];
    
    long int GroupsElapsed[3];
    long int currentTime;
    
    int skyps;
    
    String type;
    String addr;
    
public:
    
    /**
     * Enable to using time by Arduino
     * Recieve String of time in ISO format
     */
    void enableTime(char* date, char* z){
        LastDateTime = date;
        zone = z;
        syncTime = true;
    }
    
    
    /**
     * Make configuration of this class
     * ty = type of sensor
     * ad = address of sensor
     * divider = symbol used to divide values into serial outputs
     * skyp = numbers of skips befor read
     * Multiply
     */
    void config(String ty="none", String ad = "none", String divider = "#", int skyp=5, long int Multiply=60){
        syncTime=false;
        MuxSerialTime::addr = ad;
        MuxSerialTime::type =  ty;
        
        MuxSerialTime::GroupMultiply=Multiply;
        
        for(int i=0;i<array;i++){
            if(i==0) MuxSerialTime::GroupsDelay[i] = 1;
            else MuxSerialTime::GroupsDelay[i]= i * delayDiff;
        }
        
        MuxSerialTime::ValueLineDivider = divider;
        MuxSerialTime::skyps = skyp;
        
        for(int i=0;i<array;i++){
            MuxSerialTime::GroupsElapsed[i]= millis();
        }
    }
    
    /**
     * put new value with new time
     */
    void putValue(char* date, double value, bool useTime){
        if(syncTime){LastDateTime = date;}
        putValue(value);
    }
    
    /**
     *
     * put new only new value
     */
    void putValue(double value){
        
        MuxSerialTime::currentTime = millis();
        
        if(MuxSerialTime::skyps == 0){
            
            for(int i=0;i<array;i++){
                if(
                   ((MuxSerialTime::currentTime-MuxSerialTime::GroupsElapsed[i])/1000)
                   >= (MuxSerialTime::GroupMultiply*MuxSerialTime::GroupsDelay[i])
                   ) {
                    
                    /**
                     * ISSUE#1: changed 0T,1T...nT ID into 0M,1M...nM
                     * where n are size of time aggregation
                     *
                     * BUGFIX#1: fixed problem of conversion int into string
                     * with String() arduino function
                     **/
                    
                /**    String timeSizeToString = String(MuxSerialTime::GroupsDelay[i]);
                    String timeID = (timeSizeToString + "M");
                    MuxSerialTime::GroupsElapsed[i] = MuxSerialTime::currentTime;
                    
                    PrintTimedStats(MuxSerialTime::grouped[i], timeID ,"A");
                    
                    MuxSerialTime::grouped[i]=Statistic();
                    MuxSerialTime::grouped[i].clear();**/
                    
                }
                
                MuxSerialTime::grouped[i].add(value);
            }
            
            /**
             * ISSUE#1: changed 0T,1T...nT ID into 0M,1M...nM
             * where n are size of time aggregation
             **/
            MuxSerialTime::HEADSerialLine("0M", "S");
            Serial.println(value);
            
        }else{
            skyps--;
        }
    }
    
    void PrintTimedStats(Statistic group, String IDLine, String ValueType){
        MuxSerialTime::HEADSerialLine(IDLine,ValueType);
        MuxSerialTime::BODYSerialLine(group);
    }
    
    void BODYSerialLine(Statistic group){
        Serial.print(group.average());
        Serial.print(MuxSerialTime::ValueLineDivider);
        Serial.print(group.minimum());
        Serial.print(MuxSerialTime::ValueLineDivider);
        Serial.print(group.maximum());
        Serial.print(MuxSerialTime::ValueLineDivider);
        Serial.println(group.pop_stdev());
        group.clear();
    }
    
    void HEADSerialLine(String IDLine, String TypeValue){
        
     /**   if(syncTime != false){
            Serial.print(LastDateTime);
            Serial.print(MuxSerialTime::ValueLineDivider);
        }else{
            Serial.print("-");
            Serial.print(MuxSerialTime::ValueLineDivider);
            Serial.print("-");
            Serial.print(MuxSerialTime::ValueLineDivider);
            Serial.print("-");
            Serial.print(MuxSerialTime::ValueLineDivider);
        }**/
        
        Serial.print(IDLine);
        Serial.print(MuxSerialTime::ValueLineDivider);
        Serial.print(MuxSerialTime::type);
        Serial.print(MuxSerialTime::ValueLineDivider);
        Serial.print(MuxSerialTime::addr);
        Serial.print(MuxSerialTime::ValueLineDivider);
        Serial.print(TypeValue);
        Serial.print(MuxSerialTime::ValueLineDivider);
    }
};