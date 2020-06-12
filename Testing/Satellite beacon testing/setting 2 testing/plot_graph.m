clear; clc; 

load total_CC.mat

figure
plot(Times6,LM3671_1ohm*1000,'lineWidth',1)
hold on; grid minor; ylim([0 100]); xlim([-54 54])
plot(Times7,LM3671_2ohm*500,'lineWidth',1)
ax=gca;ax.FontSize=16;
title('Satellite beacon total current consumption measurements','fontSize',24);
xlabel('Time, s','fontSize',18);
ylabel('Current, mA','fontSize',18);
legend('LM3671 regulator - 1\Omega resistor','LM3671 regulator - 2\Omega resistor','fontSize',12);
        