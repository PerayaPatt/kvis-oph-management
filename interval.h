#ifndef INTERVAL_H
#define INTERVAL_H
#include<stdio.h>
#define MXT 1000
typedef struct period //information of each stations
{
    int id; //id of station
    int first;
    int second;
    double val; //preference value of the station
}period;
period pd[MXT];
int func(period t[MXT], int m, int st , int en, int arr[])
{
    static double dp[MXT];
    static int pre[MXT];
    int k=0;
    //filter the station
    if(m>MXT) m=MXT;
    for (int i=0;i<m;i++)
    {
        if(t[i].first>=st && t[i].second<=en && t[i].val>0)
        {
            t[k++] = t[i];
        }
    }
    m=k;
    //sorting
    for (int i=0 ; i<m-1 ;i++)
    {
        for (int j=i+1;j<m;j++)
        {
            if(t[i].second > t[j].second)
            {
                struct period temp;
                temp = t[i];
                t[i] = t[j];
                t[j] = temp;
            }
        }
    }
    for (int i=0;i<m;i++)
    {
        dp[i] = t[i].val;
        pre[i] = -1;
    }
    //find the highest preference value
    for(int i=0;i<m;i++)
    {
        for(int j=0;j<i;j++)
        {
            if(t[j].second<=t[i].first)
            {
                if(dp[j] + t[i].val>dp[i])
                {
                    dp[i] = dp[j] + t[i].val;
                    pre[i] = j;
                }
            }
        }
    }
    int selectedCount=0;
    int best=-1;
    //find the best schedule
    for(int i=0;i<m;i++)
    {
         if(best<0 || dp[i]>dp[best]) //find the schedule that gives the highest preference value
         {
             best = i;
         }
    }
    for(int i=best;i>=0;i=pre[i]) //scan for previous interval and collect in array
    {
         arr[selectedCount] = t[i].id;
         selectedCount++;
    }
    //swap the elements in array
    for(int i=0;i<selectedCount/2;i++)
    {
         int temp = arr[i];
         arr[i] = arr[selectedCount-1-i];
         arr[selectedCount-1-i] = temp;
    }
    return selectedCount; //return number of interval can be selected
}
#endif
