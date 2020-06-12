clear; clc;
load solar_checks

figure
plot(Times,(Channel1V-0.01)*1000,'lineWidth',1)
hold on; grid minor; ylim([0 80]); xlim([-145 0])
plot(Times1,(Channel1V1-0.01)*1000,'lineWidth',1)
ax=gca;ax.FontSize=16;

title('Solar panel current generation measurements','fontSize',24);
xlabel('Time (sec)','fontSize',18);
ylabel('Current (mA)','fontSize',18);
legend('Solar panel curent with 3 panels','Solar panel curent with 2 panels','fontSize',12);

