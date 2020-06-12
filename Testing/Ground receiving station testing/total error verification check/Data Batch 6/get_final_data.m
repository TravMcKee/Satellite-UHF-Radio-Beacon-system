clear; clc;

load final_verify_data.mat

%This is to check if there is any difference in the date or time
count=0;
count1=0;
  
for x=1:length(GR1data)
   if (GR1data(x,11) ~= GR2data(x,11))
        count=count+1;
   end
    
   if (GR1data(x,12) ~= GR2data(x,12))
       count1=count1+1;
       index(count1)=x;
   end
end


TOA_diff = zeros(length(GR1data),5);
TOA_Time1 = zeros(length(GR1data),5);
TOA_Time2 = zeros(length(GR1data),5);

for x=1:length(GR1data)
    
    for y = 1:5
    TOA_Time1(x,y) = (GR1data(x,6)-GR1data(x,y))*(1/GR1data(x,10));
    TOA_Time2(x,y) = (GR2data(x,6)-GR2data(x,y))*(1/GR2data(x,10));
    TOA_diff(x,y) = (TOA_Time2(x,y) - TOA_Time1(x,y));
    end
    
end

PPS_differences1 = zeros(length(GR1data),2);
PPS_differences2 = zeros(length(GR2data),2);

for x=1:length(GR1data)
    
PPS_differences1(x,1) = (GR1data(x,8)-GR1data(x,7))-(GR1data(x,7)-GR1data(x,6));
PPS_differences1(x,2) = (GR1data(x,9)-GR1data(x,8))-(GR1data(x,8)-GR1data(x,7));

PPS_differences2(x,1) = (GR2data(x,8)-GR2data(x,7))-(GR2data(x,7)-GR2data(x,6));
PPS_differences2(x,2) = (GR2data(x,9)-GR2data(x,8))-(GR2data(x,8)-GR2data(x,7));
    
end

save('TDOA_data.mat','GR1data','GR2data','TOA_diff');
