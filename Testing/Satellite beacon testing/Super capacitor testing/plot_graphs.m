% close all; clear; clc;
% load super_cap_charge.mat
% 
% figure
% plot(Times/60,Channel1V);
% hold on; grid on; ylim([0 7]); xlim([0 27]);
% plot(Times1/60,Channel1V1);
% plot(Times2/60,Channel1V2);
% plot(Times3/60,Channel1V3);
% 
% title('Satellite beacon capacitor charge time and voltage using solar power source','fontSize',24);
% xlabel('Time - Minutes','fontSize',16);
% ylabel('Voltage - Voltage','fontSize',16);
% legend('5F capacitor charged with 1 solar panel, sunny day 16*C','5F capacitor charged with 1 solar panel, sunny day 18*C'...
%     ,'5F capacitor charged with 2 solar panels, sunny day 14*C','5F capacitor charged with 2 solar panel2, sunny day 15*C');
% 
% clear;
% load super_cap_discharge.mat
% 
% %figure
% plot(Times/60,Channel1V);
% hold on; grid on; ylim([0 7]);
% plot(Times1/60,Channel1V1);
% plot(Times2/60,Channel1V2);
% plot(Times3/60,Channel1V3);
% 
% title('Satellite beacon capacitor discharge time and voltage with no power source','fontSize',24);
% xlabel('Time - Minutes','fontSize',16);
% ylabel('Voltage - Voltage','fontSize',16);
% 
% clear
% load full_software_check.mat
% 
% % figure
% plot(Times/60,Channel1V);
% hold on; grid on; ylim([0 7]);
% plot(Times1/60,Channel1V1);
% 
% title('Satellite beacon capacitor charge time and voltage for full software cycle','fontSize',24);
% xlabel('Time - Minutes','fontSize',16);
% ylabel('Voltage - Voltage','fontSize',16);

close all; clear; clc;
load super_cap_charge.mat

figure
plot(Times/60,Channel1V,'--','LineWidth',1);
hold on; grid on; ylim([0 7]); xlim([0 82]);
plot(Times1/60,Channel1V1,'--','LineWidth',1);
plot(Times2/60,Channel1V2,'--','LineWidth',1);
plot(Times3/60,Channel1V3,'--','LineWidth',1);
ax=gca;ax.FontSize=16;
title('Satellite beacon capacitor storage system charge and discharge time','fontSize',24);
xlabel('Time (min)','fontSize',16);
ylabel('Voltage (V)','fontSize',16);


clear;
load super_cap_discharge.mat
plot(Times/60,Channel1V,'LineWidth',1);
plot(Times1/60,Channel1V1,'LineWidth',1);
plot(Times2/60,Channel1V2,'LineWidth',1);
plot(Times3/60,Channel1V3,'LineWidth',1);

clear
load full_software_check.mat
plot(Times/60,Channel1V,'-.','LineWidth',1);
plot(Times1/60,Channel1V1,'-.','LineWidth',1);

legend('5F capacitor charged with no load, 1 solar panel, sunny day 16*C','5F capacitor charged with no load, 1 solar panel, sunny day 18*C'...
    ,'5F capacitor charged with no load, 2 solar panels, sunny day 14*C','5F capacitor charged with no load, 2 solar panel2, sunny day 15*C'...
    ,'no launch phase capacitor system discharge cycle 1','no launch phase capacitor system discharge cycle 2'...
    ,'no launch phase capacitor system discharge cycle 3','no launch phase capacitor system discharge cycle 4'...
    ,'full software cycle capcitor system discharge cycle 1','full software cycle capcitor system discharge cycle 1','fontSize',12);