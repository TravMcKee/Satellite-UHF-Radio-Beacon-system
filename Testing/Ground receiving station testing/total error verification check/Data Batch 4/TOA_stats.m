clear; clc; close all;

load TDOA_data.mat

figure
plot(TOA_diff(:,1)*1000,'lineWidth',1);
hold on; grid minor; xlim([0 length(TOA_diff(:,1))]);
plot(TOA_diff(:,2)*1000,'lineWidth',1);
plot(TOA_diff(:,3)*1000,'lineWidth',1);
plot(TOA_diff(:,4)*1000,'lineWidth',1);
plot(TOA_diff(:,5)*1000,'lineWidth',1);
ax=gca;ax.FontSize=16;
title('Total error in timing measurement between 2 ground receiving stations','fontSize',24);
xlabel('Sample number (n)','fontSize',16);
ylabel('Time (ms)','fontSize',16);
legend('Address packet 1','Address packet 2','Address packet 3','Address packet 4','Telemetry packet','fontSize',12,'location','NorthEast');

TOA_mean = zeros(5,1);
TOA_StdDev = zeros(5,1);
TOA_abs_mean = zeros(5,1);
TOA_abs_StdDev = zeros(5,1);

for x=1:5
TOA_mean(x) = mean(TOA_diff(:,x));
TOA_abs_mean(x) = mean(abs(TOA_diff(:,x)));
TOA_StdDev(x) = std(TOA_diff(:,x));
TOA_abs_StdDev(x) = std(abs(TOA_diff(:,x)));
end

TOA_diff_no_spike = zeros(999,5);
count=1;

for i=1:length(TOA_diff)
    
    if ( abs(TOA_diff(i,1)) < 0.0025 )
        TOA_diff_no_spike(count,1)=TOA_diff(i,1);
        TOA_diff_no_spike(count,2)=TOA_diff(i,2);
        TOA_diff_no_spike(count,3)=TOA_diff(i,3);
        TOA_diff_no_spike(count,4)=TOA_diff(i,4);
        TOA_diff_no_spike(count,5)=TOA_diff(i,5);
        count=count+1;
    end
end

figure
plot(abs(TOA_diff_no_spike(:,1))*1000,'lineWidth',1);
hold on; grid minor; xlim([0 length(TOA_diff_no_spike(:,1))]);
plot(abs(TOA_diff_no_spike(:,2))*1000,'lineWidth',1);
plot(abs(TOA_diff_no_spike(:,3))*1000,'lineWidth',1);
plot(abs(TOA_diff_no_spike(:,4))*1000,'lineWidth',1);
plot(abs(TOA_diff_no_spike(:,5))*1000,'lineWidth',1);
ax=gca;ax.FontSize=16;
title('Absolute total error in timing measurement between 2 stations','fontSize',24);
xlabel('Sample number (n)','fontSize',16);
ylabel('Time (ms)','fontSize',16);
legend('Address packet 1','Address packet 2','Address packet 3','Address packet 4','Telemetry packet','fontSize',12,'location','SouthEast');

TOA_mean_NS = zeros(5,1);
TOA_StdDev_NS = zeros(5,1);
TOA_abs_mean_NS = zeros(5,1);
TOA_abs_StdDev_NS = zeros(5,1);

for x=1:5
TOA_mean_NS(x) = mean(TOA_diff_no_spike(:,x));
TOA_abs_mean_NS(x) = mean(abs(TOA_diff_no_spike(:,x)));
TOA_StdDev_NS(x) = std(TOA_diff_no_spike(:,x));
TOA_abs_StdDev_NS(x) = std(abs(TOA_diff_no_spike(:,x)));
end


for i=1:length(TOA_diff_no_spike)
average_time(i) = (abs(TOA_diff(i,1)) + abs(TOA_diff(i,2)) + abs(TOA_diff(i,3)) + abs(TOA_diff(i,4)) + abs(TOA_diff(i,5)))/5;
end

figure
plot(average_time*1000,'lineWidth',1);
hold on; grid minor; xlim([0 length(average_time)]);
ax=gca;ax.FontSize=16;
title('Averaged absolute error in timing measurement between 2 stations','fontSize',24);
xlabel('Sample number (n)','fontSize',16);
ylabel('Time (ms)','fontSize',16);


TOA_average_mean = mean(average_time)
TOA_average_StdDev = std(average_time)