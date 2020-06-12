clear; clc; 

load reg_measurements.mat

figure
plot(Times2,MC5205_1ohm*1000,'lineWidth',1)
hold on; grid minor; ylim([0 130]); xlim([-54 54])
plot(Times3,MC5205_2ohm*500,'lineWidth',1)
plot(Times4,LM1086_1ohm*1000,'lineWidth',1)
plot(Times5,LM1086_2ohm*500,'lineWidth',1)
plot(Times6,LM3671_1ohm*1000,'lineWidth',1)
plot(Times7,LM3671_2ohm*500,'lineWidth',1)
plot(Times8,TS2940CZ_1ohm*1000,'lineWidth',1)
plot(Times9,TS2940CZ_2ohm*500,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('Satellite beacon current consumption using various regulators','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('MC5205 regulator - 1\Omega resistor','MC5205 regulator - 2\Omega resistor',...
        'LM1086 regulator - 1\Omega resistor','LM1086 regulator - 2\Omega resistor',...
        'LM3671 regulator - 1\Omega resistor','LM3671 regulator - 2\Omega resistor',...
        'TS2940CZ regulator - 1\Omega resistor','TS2940CZ regulator - 2\Omega resistor','fontSize',12);
        