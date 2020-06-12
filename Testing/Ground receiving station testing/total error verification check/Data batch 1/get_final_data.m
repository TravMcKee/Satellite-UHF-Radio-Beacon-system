clear; clc;

load final_verification_data.mat

%This is to check if there is any difference in the date or time
count=0;
count1=0;
for x=1:length(GR1data)
   if (GR1data(x,10) ~= GR2data(x,10))
       count=count+1;
   end
    
   if (GR1data(x,10) ~= GR2data(x,10))
       count1=count1+1;
   end
end

TOA_data1 = zeros(length(GR1data),7);
TOA_data2 = zeros(length(GR2data),7);

for x=1:length(GR1data)
    TOA_data1(x,1) = GR1data(x,1);
    TOA_data1(x,2) = GR1data(x,2);
    TOA_data1(x,3) = GR1data(x,3);
    TOA_data1(x,4) = GR1data(x,4);
    TOA_data1(x,5) = GR1data(x,5);
    TOA_data1(x,6) = GR1data(x,6);
    TOA_data1(x,7) = ((GR1data(x,9)-GR1data(x,6)))/3;
    
    TOA_data2(x,1) = GR2data(x,1);
    TOA_data2(x,2) = GR2data(x,2);
    TOA_data2(x,3) = GR2data(x,3);
    TOA_data2(x,4) = GR2data(x,4);
    TOA_data2(x,5) = GR2data(x,5);
    TOA_data2(x,6) = GR2data(x,6);
    TOA_data2(x,7) = ((GR2data(x,9)-GR2data(x,6)))/3;
end

TOA_diff = zeros(length(TOA_data1),5);
TOA_Time1 = zeros(length(TOA_data1),5);
TOA_Time2 = zeros(length(TOA_data1),5);

for x=1:length(TOA_data1)
    clk1 = 1/TOA_data1(x,7);    %length of station 1 clock cycle
    clk2 = 1/TOA_data2(x,7);    %length of station 2 clock cycle
    
    for y = 1:5
    TOA_Time1(x,y) = (TOA_data1(x,6)-TOA_data1(x,y))*clk1;
    TOA_Time2(x,y) = (TOA_data2(x,6)-TOA_data2(x,y))*clk2;
    TOA_diff(x,y) = (TOA_Time1(x,y) - TOA_Time2(x,y));
    end
    
end

